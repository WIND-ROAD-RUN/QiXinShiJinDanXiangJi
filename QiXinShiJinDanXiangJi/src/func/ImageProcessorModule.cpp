#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "ImageProcessorModule.hpp"

#include <qfuture.h>
#include <qtconcurrentrun.h>
#include <atomic>
#include "Utilty.hpp"
#include "halconcpp/HalconCpp.h"
#include "Halcon.h"
#include <QPainter>
#include <QPen>
#include <cmath>
#include <algorithm>

#include "Modules.hpp"
#include "QiXinShiJinDanXiangJi.h"

QMutex ImageProcessor::isBadVectorMutex;
std::vector<bool> ImageProcessor::isBadVector{ false };

namespace {
	// 在给定最小间隔内只放行一次调用：成功返回 true，其他并发/过快的调用返回 false
	inline bool AllowOncePer(std::atomic<long long>& lastNs, std::chrono::nanoseconds minInterval)
	{
		using clock = std::chrono::steady_clock;
		const auto nowNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
			clock::now().time_since_epoch()).count();

		auto prev = lastNs.load(std::memory_order_relaxed);
		if (nowNs - prev < minInterval.count())
			return false; // 距上次放行未到间隔，拒绝

		// 只有一个线程能成功更新 lastNs，其他并发线程会失败并返回 false
		return lastNs.compare_exchange_strong(prev, nowNs, std::memory_order_relaxed);
	}
} // namespace


ImageProcessor::ImageProcessor(QQueue<MatInfo>& queue, QMutex& mutex, QWaitCondition& condition, int workIndex, QObject* parent)
	: QThread(parent), _queue(queue), _mutex(mutex), _condition(condition), _workIndex(workIndex)
{

}

void ImageProcessor::run()
{
	while (!QThread::currentThread()->isInterruptionRequested()) {
		MatInfo frame;
		{
			QMutexLocker locker(&_mutex);
			if (_queue.isEmpty()) {
				_condition.wait(&_mutex);
				if (QThread::currentThread()->isInterruptionRequested()) {
					break;
				}
			}
			if (!_queue.isEmpty()) {
				frame = _queue.dequeue();
			}
			else {
				continue; // 如果队列仍为空，跳过本次循环
			}
		}

		// 检查 frame 是否有效
		if (frame.image.empty()) {
			continue; // 跳过空帧
		}

		auto currentRunningState = Modules::getInstance().runtimeInfoModule.runningState.load();
		switch (currentRunningState)
		{
		case RunningState::Debug:
			run_debug(frame);
			break;
		case RunningState::OpenRemoveFunc:
			run_OpenRemoveFunc(frame);
			break;
		default:
			break;
		}
	}
}

void ImageProcessor::run_debug(MatInfo& frame)
{
	auto& imgPro = *_imgProcess;
	imgPro(frame.image);
	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();

	emit imageReady(QPixmap::fromImage(maskImg));
}
static HalconCpp::HObject MatToHImage(cv::Mat& cv_img)
{
	HalconCpp::HObject H_img;

	if (cv_img.channels() == 1)
	{
		int height = cv_img.rows, width = cv_img.cols;
		int size = height * width;
		uchar* temp = new uchar[size];

		memcpy(temp, cv_img.data, size);
		HalconCpp::GenImage1(&H_img, "byte", width, height, (Hlong)(temp));

		delete[] temp;
	}
	else if (cv_img.channels() == 3)
	{
		int height = cv_img.rows, width = cv_img.cols;
		int size = height * width;
		uchar* B = new uchar[size];
		uchar* G = new uchar[size];
		uchar* R = new uchar[size];

		for (int i = 0; i < height; i++)
		{
			uchar* p = cv_img.ptr<uchar>(i);
			for (int j = 0; j < width; j++)
			{
				B[i * width + j] = p[3 * j];
				G[i * width + j] = p[3 * j + 1];
				R[i * width + j] = p[3 * j + 2];
			}
		}
		HalconCpp::GenImage3(&H_img, "byte", width, height, (Hlong)(R), (Hlong)(G), (Hlong)(B));

		delete[] R;
		delete[] G;
		delete[] B;
	}
	return H_img;
}

// 在图像上绘制 Halcon rectangle2 定义的旋转矩形：中心(row,col)，半长(length1,length2)，角度angle(弧度)
static void DrawRotatedRectangle(QImage& image,
	double row, double col,
	double length1, double length2,
	double angleRad,
	const QColor& color = Qt::green,
	int thickness = 3)
{
	if (length1 <= 0.0 || length2 <= 0.0)
		return;

	// Halcon 坐标: col 为 x，row 为 y；angle 为相对行轴(向右为列轴，向下为行轴)的旋转角(弧度)
	const double c = col;
	const double r = row;
	const double cosA = std::cos(angleRad);
	const double sinA = std::sin(angleRad);

	// 主轴向量 u 对应 length1，副轴向量 v 对应 length2
	const double uc = length1 * cosA;   // 列方向增量
	const double ur = length1 * sinA;   // 行方向增量
	const double vc = -length2 * sinA;  // 列方向增量（旋转 +90°）
	const double vr = length2 * cosA;  // 行方向增量（旋转 +90°）

	auto makePoint = [&](double dc, double dr) -> QPointF {
		return QPointF(c + dc, r + dr); // QPointF(x=col, y=row)
		};

	QPolygonF poly;
	poly << makePoint(uc + vc, ur + vr)
		<< makePoint(-uc + vc, -ur + vr)
		<< makePoint(-uc - vc, -ur - vr)
		<< makePoint(uc - vc, ur - vr);

	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(color, thickness));
	painter.drawPolygon(poly);
}

void halconPRocess(cv::Mat image, double& R1, double& C1, double& length, double& width, double& angle, bool& isBad)
{
	isBad = false;

	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& QiXinShiJinConfig = Modules::getInstance().configManagerModule.qiXinShiJinDanXiangJiConfig;

	auto modelimage = QiXinShiJinDanXiangJi::getModelImage();

	if (!modelimage)
	{
		return;
	}

	auto image1 = MatToHImage(image);

	HalconCpp::Rgb1ToGray(image1, &image1);
	HalconCpp::MeanImage(image1, &image1, 3, 3);

	HalconCpp::HObject ho_ImageSub, ho_Regions, ho_ConnectedRegions, ho_SelectedRegions, ho_RegionTrans;
	SubImage(*modelimage, image1, &ho_ImageSub, 30, 255);

	double daichang = QiXinShiJinConfig.setBagLength;
	double daikuan = QiXinShiJinConfig.setBagWidth;
	double pixtoworld = setConfig.xiangsudangliang;


	double pixchang = daichang * pixtoworld;
	double pixkuan = daikuan * pixtoworld;

	Threshold(ho_ImageSub, &ho_Regions, 0, 60);
	HalconCpp::HObject rectangle;
	double shangxianwei = setConfig.shangxianwei;
	double xiaxianwei = setConfig.xiaxianwei;

	if (setConfig.shangxianwei > setConfig.xiaxianwei)
	{
		shangxianwei = setConfig.xiaxianwei;
		xiaxianwei = setConfig.shangxianwei;
	}

	if (shangxianwei < 0)
	{
		shangxianwei = 0;
	}
	if (xiaxianwei < 0)
	{
		xiaxianwei = 0;
	}
	if (shangxianwei > image.rows)
	{
		shangxianwei = image.rows;
	}
	if (xiaxianwei > image.rows)
	{
		xiaxianwei = image.rows;
	}

	GenRectangle1(&rectangle, setConfig.shangxianwei, 0, setConfig.xiaxianwei, 1280);

	Intersection(rectangle, ho_Regions, &ho_Regions);
	HalconCpp::DilationRectangle1(ho_Regions, &ho_Regions, 30, 1);

	Connection(ho_Regions, &ho_ConnectedRegions);
	HalconCpp::HObject ho_MaxRegion;
	SelectShapeStd(ho_ConnectedRegions, &ho_MaxRegion, "max_area", 0);

	HalconCpp::HTuple hv_Area, hv_Row2, hv_Column2, hv_Row1, hv_Column1, hv_Phi1, hv_Length1, hv_Length2;

	ShapeTrans(ho_MaxRegion, &ho_RegionTrans, "rectangle2");

	SmallestRectangle2(ho_RegionTrans, &hv_Row1, &hv_Column1, &hv_Phi1, &hv_Length1,
		&hv_Length2);

	AreaCenter(ho_RegionTrans, &hv_Area, &hv_Row2, &hv_Column2);

	R1 = hv_Row1;
	C1 = hv_Column1;
	length = hv_Length1;
	width = hv_Length2;
	angle = -hv_Phi1;

	length = hv_Length1 * 2;
	width = hv_Length2 * 2;

	//连包缺陷
	if (0 != (HalconCpp::HTuple(int((hv_Area.TupleLength()) > 0)).TupleAnd(int(hv_Area > 200))))
	{
		double kuan = 0;
		double chang = 0;
		if (hv_Length1 > hv_Length2)
		{
			chang = hv_Length1 * 2;
			kuan = hv_Length2 * 2;
		}
		else
		{
			chang = hv_Length2 * 2;
			kuan = hv_Length1 * 2;
		}

		if (chang > pixchang || kuan > pixkuan)
		{
			isBad = true;
		}
	}
	else
	{
		isBad = true;
	}//连包缺陷
}


void ImageProcessor::run_OpenRemoveFunc(MatInfo& frame)
{
	auto& imgPro = *_imgProcess;
	double R1 = 0;
	double C1 = 0;
	double length = 0;
	double width = 0;
	double angle = 0;

	QFuture<bool> positiveIsBadFuture;

	positiveIsBadFuture = QtConcurrent::run(
		[this, &positiveIsBadFuture, &frame, &R1, &C1, &length, &width, &angle]()
		{
			bool isBad = false;

			halconPRocess(frame.image, R1, C1, length, width, angle, isBad);

			return isBad;
		});

	imgPro(frame.image);
	bool positiveIsBad{ false };

	positiveIsBadFuture.waitForFinished();
	positiveIsBad = positiveIsBadFuture.result();

	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& statisticalInfo = Modules::getInstance().runtimeInfoModule.statisticalInfo;
	statisticalInfo.bagLength = length / setConfig.xiangsudangliang;
	statisticalInfo.bagWidth = width / setConfig.xiangsudangliang;

	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();

	auto& context = _imgProcess->context();

	if (defectResult.isBad ||
		positiveIsBad)
	{
		{
			QMutexLocker locker(&isBadVectorMutex);
			isBadVector[0] = true;
		}
	}

	bool isBad = false;
	{
		QMutexLocker locker(&isBadVectorMutex);
		isBad = isBadVector[0];
	}

	emit updateMainWindowShowBtn();

	run_OpenRemoveFunc_emitErrorInfo(isBad);

	DrawRotatedRectangle(maskImg, R1, C1, length / 2, width / 2, angle, QColor(0, 255, 0), 3);

	emit imageReady(QPixmap::fromImage(maskImg));
	emit updateStatisticalInfo();

	if (isBad)
	{
		QMutexLocker locker(&isBadVectorMutex);
		{
			isBadVector.erase(isBadVector.begin());
			isBadVector.push_back(false);
		}
	}
}

void ImageProcessor::run_OpenRemoveFunc_emitErrorInfo(bool isbad)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& statisticalInfo = Modules::getInstance().runtimeInfoModule.statisticalInfo;

	if (isbad)
	{
		++statisticalInfo.wasteCount;
	}
	else
	{
		++productGoodCount;
		if (setConfig.fenliaojishu == productGoodCount)
		{
			auto& camera = Modules::getInstance().cameraModule.camera1;
			rw::rqw::OutTriggerConfig outTriggerConfig;
			outTriggerConfig.lineSelector = 2;
			outTriggerConfig.lineMode = 8;
			outTriggerConfig.lineSource = 5;
			outTriggerConfig.durationValue = 500 * 1000; // 500ms
			outTriggerConfig.strobeEnable = true;
			if (camera)
			{
				camera->setOutTriggerConfig(outTriggerConfig);
				camera->outTrigger();
			}

			productGoodCount = 0;
		}
	}

	if (imageProcessingModuleIndex == 1)
	{
		++statisticalInfo.produceCount;
	}

	if (isbad)
	{
		Modules::getInstance().eliminateModule.productPriorityQueue1->push(true);
	}
}

void ImageProcessor::buildDetModelEngine(const QString& enginePath)
{
	rw::ModelEngineConfig modelEngineConfig;
	modelEngineConfig.conf_threshold = 0.1f;
	modelEngineConfig.nms_threshold = 0.1f;
	modelEngineConfig.imagePretreatmentPolicy = rw::ImagePretreatmentPolicy::LetterBox;
	modelEngineConfig.letterBoxColor = cv::Scalar(114, 114, 114);
	modelEngineConfig.modelPath = enginePath.toStdString();
	auto engine = rw::ModelEngineFactory::createModelEngine(modelEngineConfig, rw::ModelType::Yolov11_Det_CudaAcc, rw::ModelEngineDeployType::TensorRT);
	_imgProcess = std::make_unique<rw::imgPro::ImageProcess>(engine);
	_imgProcess->context() = Modules::getInstance().imgProModule.imageProcessContext_Main;
	_imgProcess->context().customFields["ImgProcessIndex"] = static_cast<int>(imageProcessingModuleIndex);
	_imgProcess->context().customFields["stationIdx"] = static_cast<int>(_workIndex);

	initial_isBadVector();
}

void ImageProcessor::initial_isBadVector()
{
	{
		QMutexLocker locker(&isBadVectorMutex);
		isBadVector.assign(50, false);
	}
}

void ImageProcessingModule::BuildModule()
{
	for (int i = 0; i < _numConsumers; ++i) {
		static size_t workIndexCount = 0;
		ImageProcessor* processor = new ImageProcessor(_queue, _mutex, _condition, workIndexCount, this);
		workIndexCount++;
		processor->imageProcessingModuleIndex = index;
		processor->buildDetModelEngine(modelEnginePath);
		connect(processor, &ImageProcessor::imageReady, this, &ImageProcessingModule::imageReady, Qt::QueuedConnection);
		connect(processor, &ImageProcessor::updateMainWindowShowBtn, this, &ImageProcessingModule::updateMainWindowShowBtn, Qt::QueuedConnection);
		connect(processor, &ImageProcessor::updateStatisticalInfo, this, &ImageProcessingModule::updateStatisticalInfo, Qt::QueuedConnection);

		_processors.push_back(processor);
		processor->start();
	}
}

ImageProcessingModule::ImageProcessingModule(int numConsumers, QObject* parent)
	: QObject(parent), _numConsumers(numConsumers)
{

}

ImageProcessingModule::~ImageProcessingModule()
{
	// 通知所有线程退出
	for (auto processor : _processors) {
		processor->requestInterruption();
	}

	// 唤醒所有等待的线程
	{
		QMutexLocker locker(&_mutex);
		_condition.wakeAll();
	}

	// 等待所有线程退出
	for (auto processor : _processors) {
		if (processor->isRunning()) {
			processor->wait(1000); // 使用超时机制，等待1秒
		}
		delete processor;
	}
}

void ImageProcessingModule::onFrameCaptured(rw::rqw::MatInfo matInfo, size_t index)
{
	// 防抖动处理
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	const long long debounceMs = static_cast<long long>(std::max(0.0, setConfig.xiangjiguangdianpingbishijian));
	const auto minInterval = std::chrono::milliseconds(debounceMs);

	static std::atomic<long long> lastCamNs{ 0 };

	if (!AllowOncePer(lastCamNs, minInterval)) {
		return;
	}

	if (matInfo.mat.channels() == 4) {
		cv::cvtColor(matInfo.mat, matInfo.mat, cv::COLOR_BGRA2BGR);
	}
	if (matInfo.mat.type() != CV_8UC3) {
		matInfo.mat.convertTo(matInfo.mat, CV_8UC3);
	}

	if (matInfo.mat.empty()) {
		return; // 跳过空帧
	}

	auto isModelNeedGet = QiXinShiJinDanXiangJi::getIsModelImageLoaded();
	if (!isModelNeedGet)
	{
		HalconCpp::HObject hImage = MatToHImage(matInfo.mat);
		HalconCpp::Rgb1ToGray(hImage, &hImage);
		HalconCpp::MeanImage(hImage, &hImage, 3, 3);
		QiXinShiJinDanXiangJi::setModelImage(hImage);
		QiXinShiJinDanXiangJi::setIsModelImageLoaded(true);
	}

	QMutexLocker locker(&_mutex);
	MatInfo mat;
	mat.image = matInfo.mat;
	mat.index = index;

	_queue.enqueue(mat);
	_condition.wakeOne();
}
