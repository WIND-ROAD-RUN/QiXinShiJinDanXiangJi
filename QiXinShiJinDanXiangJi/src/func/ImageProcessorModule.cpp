#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "ImageProcessorModule.hpp"

#include <qfuture.h>
#include <qtconcurrentrun.h>
#include <atomic>
#include "GlobalStruct.hpp"
#include "Utilty.hpp"
#include "halconcpp/HalconCpp.h"
#include "Halcon.h"
#include <QPainter>
#include <QPen>
#include <cmath>
#include <algorithm>
#include "QiXinShiJinDanXiangJi.h"

QMutex ImageProcessorDuckTongue::isBadVectorMutex;
std::vector<bool> ImageProcessorDuckTongue::isBadVector{ false };

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


ImageProcessorDuckTongue::ImageProcessorDuckTongue(QQueue<MatInfo>& queue, QMutex& mutex, QWaitCondition& condition, int workIndex, QObject* parent)
	: QThread(parent), _queue(queue), _mutex(mutex), _condition(condition), _workIndex(workIndex)
{

}

void ImageProcessorDuckTongue::run()
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

		auto& globalData = GlobalData::getInstance();

		// 获取当前时间点
		auto now = std::chrono::system_clock::now();
		// 转换为time_t格式
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		// 转换为本地时间
		std::tm* local_time = std::localtime(&now_time);

		auto currentRunningState = globalData.runningState.load();
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

void ImageProcessorDuckTongue::run_debug(MatInfo& frame)
{
	auto& imgPro = *_imgProcess;
	imgPro(frame.image);
	// 更新屏蔽线
	updateShieldWires();
	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();

	drawBoundariesLines(maskImg);

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

	auto& setConfig = GlobalData::getInstance().setConfig;
	auto& QiXinShiJinConfig = GlobalData::getInstance().qiXinShiJinDanXiangJiConfig;

	auto modelimage = QiXinShiJinDanXiangJi::getModelImage();



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


void ImageProcessorDuckTongue::run_OpenRemoveFunc(MatInfo& frame)
{
	auto& imgPro = *_imgProcess;
	auto& qiXinShiJinConfig = GlobalData::getInstance().qiXinShiJinDanXiangJiConfig;
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

	// 更新屏蔽线
	updateShieldWires();
	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();

	auto& context = _imgProcess->context();

	if (defectResult.isBad || 
		positiveIsBad || 
		(length > qiXinShiJinConfig.setBagLength) || 
		(width > qiXinShiJinConfig.setBagWidth))
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

	run_OpenRemoveFunc_emitErrorInfo(isBad);

	drawBoundariesLines(maskImg);
	DrawRotatedRectangle(maskImg, R1, C1, length, width, angle, QColor(0, 255, 0), 3);

	emit imageNGReady(QPixmap::fromImage(maskImg), frame.index, defectResult.isBad);
	emit updateMainWindowShowBtn();
}

void ImageProcessorDuckTongue::run_OpenRemoveFunc_emitErrorInfo(bool isbad) const
{
	auto& globalStruct = GlobalData::getInstance();
	auto& globalThread = GlobalThread::getInstance();

	if (isbad)
	{
		++globalStruct.statisticalInfo.wasteCount;
	}

	if (imageProcessingModuleIndex == 1 || imageProcessingModuleIndex == 2)
	{
		++globalStruct.statisticalInfo.produceCount;
	}

	if (isbad)
	{
		globalThread.priorityQueue->push(true);
	}
}

void ImageProcessorDuckTongue::buildDetModelEngine(const QString& enginePath)
{
	rw::ModelEngineConfig modelEngineConfig;
	modelEngineConfig.conf_threshold = 0.1f;
	modelEngineConfig.nms_threshold = 0.1f;
	modelEngineConfig.imagePretreatmentPolicy = rw::ImagePretreatmentPolicy::LetterBox;
	modelEngineConfig.letterBoxColor = cv::Scalar(114, 114, 114);
	modelEngineConfig.modelPath = enginePath.toStdString();
	auto engine = rw::ModelEngineFactory::createModelEngine(modelEngineConfig, rw::ModelType::Yolov11_Det_CudaAcc, rw::ModelEngineDeployType::TensorRT);

	_imgProcess = std::make_unique<rw::imgPro::ImageProcess>(engine);

	iniIndexGetContext();
	iniEliminationInfoFunc();
	iniEliminationInfoGetContext();
	iniDefectResultInfoFunc();
	iniDefectResultGetContext();
	iniDefectDrawConfig();
	iniRunTextConfig();

	initial_isBadVector();
}

void ImageProcessorDuckTongue::iniIndexGetContext()
{
	auto& context = _imgProcess->context();

	context.indexGetContext.removeIndicesIfByInfo = [this](const rw::DetectionRectangleInfo& info, const rw::imgPro::ImageProcessContext& imageProcessContext) {
		bool isInShieldWires = false;
		if (-1 == topShieldWire || -1 == bottomShieldWire)
		{
			return false;
		}

		if (info.center_y > topShieldWire && info.center_y < bottomShieldWire)
		{
			isInShieldWires = true;
		}
		return !isInShieldWires;
		};
}

void ImageProcessorDuckTongue::iniEliminationInfoFunc()
{
	updateParamMapsFromGlobalStruct();
}

void ImageProcessorDuckTongue::iniEliminationInfoGetContext()
{
	auto& context = _imgProcess->context();

	context.eliminationInfoGetContext.getEliminationItemFuncSpecialOperator = [this](rw::imgPro::EliminationItem& item,
		const rw::DetectionRectangleInfo& info,
		const rw::imgPro::EliminationInfoGetConfig& cfg) {

		};
}

void ImageProcessorDuckTongue::iniDefectResultInfoFunc()
{
	auto& context = _imgProcess->context();

	rw::imgPro::DefectResultInfoFunc::Config defectConfig;
	rw::imgPro::DefectResultInfoFunc::ClassIdWithConfigMap defectConfigs;
	defectConfig.isEnable = BadMap["enable"];
	defectConfigs[ClassId::Bad] = defectConfig;
	defectConfig.isEnable = FengKouMap["enable"];
	defectConfigs[ClassId::FengKou] = defectConfig;
	defectConfig.isEnable = JiaoDaiMap["enable"];
	defectConfigs[ClassId::JiaoDai] = defectConfig;
	context.defectCfg = defectConfigs;
}

void ImageProcessorDuckTongue::iniDefectResultGetContext()
{
	auto& context = _imgProcess->context();
	context.defectResultGetContext.getDefectResultExtraOperate = [this](const rw::imgPro::EliminationItem& item, const rw::DetectionRectangleInfo& detectionRectangleInfo) {

		};
}

void ImageProcessorDuckTongue::iniDefectDrawConfig()
{
	auto& context = _imgProcess->context();

	rw::imgPro::DefectDrawFunc::ConfigDefectDraw drawConfig;
	updateDrawRec();
	drawConfig.setAllIdsWithSameColor({ 0,1,2 }, rw::rqw::RQWColor::Green, true);
	drawConfig.setAllIdsWithSameColor({ 0,1,2 }, rw::rqw::RQWColor::Red, false);
	drawConfig.fontSize = 30;
	drawConfig.classIdNameMap[0] = "Bad";
	drawConfig.classIdNameMap[1] = "封口";
	drawConfig.classIdNameMap[2] = "胶带";
	context.defectDrawCfg = drawConfig;
}

void ImageProcessorDuckTongue::iniRunTextConfig()
{
	updateDrawText();
}

void ImageProcessorDuckTongue::drawBoundariesLines(QImage& image)
{
	auto& index = imageProcessingModuleIndex;
	auto& setConfig = GlobalData::getInstance().setConfig;
	rw::imgPro::ConfigDrawLine configDrawLine;
	configDrawLine.color = rw::imgPro::Color::Red;
	configDrawLine.thickness = 3;
	if (index == 1)
	{
		configDrawLine.position = setConfig.shangxianwei;
		rw::imgPro::ImagePainter::drawHorizontalLine(image, configDrawLine);
		configDrawLine.position = setConfig.xiaxianwei;
		rw::imgPro::ImagePainter::drawHorizontalLine(image, configDrawLine);
	}
}

void ImageProcessorDuckTongue::updateShieldWires()
{
	auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
	auto& index = imageProcessingModuleIndex;
	if (1 == index)
	{
		topShieldWire = globalStructSetConfig.shangxianwei;
		bottomShieldWire = globalStructSetConfig.xiaxianwei;
	}

}

void ImageProcessorDuckTongue::updateDrawRec()
{
	auto& globalStruct = GlobalData::getInstance();
	auto& context = _imgProcess->context();
	if (globalStruct.qiXinShiJinDanXiangJiConfig.isshibiekuang)
	{
		context.defectDrawCfg.isDrawDefects = true;
		context.defectDrawCfg.isDrawDisableDefects = true;
		context.defectDrawCfg.isDisAreaText = true;
		context.defectDrawCfg.isDisScoreText = true;
	}
	else
	{
		context.defectDrawCfg.isDrawDefects = false;
		context.defectDrawCfg.isDrawDisableDefects = false;
		context.defectDrawCfg.isDisAreaText = false;
		context.defectDrawCfg.isDisScoreText = false;
	}
}

void ImageProcessorDuckTongue::updateDrawText()
{
	auto& globalStruct = GlobalData::getInstance();
	auto& context = _imgProcess->context();
	if (globalStruct.qiXinShiJinDanXiangJiConfig.iswenzi)
	{
		context.runTextCfg.isDrawExtraText = true;
	}
	else
	{
		context.runTextCfg.isDrawExtraText = false;
	}
}

void ImageProcessorDuckTongue::updateParamMapsFromGlobalStruct()
{
	auto& context = _imgProcess->context();
	auto& globalStruct = GlobalData::getInstance();

	BadMap["classId"] = 0;
	BadMap["maxArea"] = 0;
	BadMap["maxScore"] = globalStruct.setConfig.score;
	BadMap["enable"] = true;
	if (1 == imageProcessingModuleIndex)
	{
		BadMap["pixToWorld"] = globalStruct.setConfig.xiangsudangliang;
	}

	FengKouMap["classId"] = 1;
	FengKouMap["maxArea"] = 0;
	FengKouMap["maxScore"] = globalStruct.setConfig.score;
	FengKouMap["enable"] = true;
	if (1 == imageProcessingModuleIndex)
	{
		FengKouMap["pixToWorld"] = globalStruct.setConfig.xiangsudangliang;
	}

	JiaoDaiMap["classId"] = 2;
	JiaoDaiMap["maxArea"] = 0;
	JiaoDaiMap["maxScore"] = globalStruct.setConfig.score;
	JiaoDaiMap["enable"] = true;
	if (1 == imageProcessingModuleIndex)
	{
		JiaoDaiMap["pixToWorld"] = globalStruct.setConfig.xiangsudangliang;
	}

	rw::imgPro::EliminationInfoFunc::ClassIdWithConfigMap eliminationInfoGetConfigs;
	rw::imgPro::EliminationInfoGetConfig NgEliminationInfoGetConfig;
	rw::imgPro::EliminationInfoGetConfig FengKouEliminationInfoGetConfig;
	rw::imgPro::EliminationInfoGetConfig JiaoDaiEliminationInfoGetConfig;

	NgEliminationInfoGetConfig.areaFactor = BadMap["pixToWorld"];//这里设置为像素当量
	NgEliminationInfoGetConfig.scoreFactor = 100;//这里设置为百分比当量
	NgEliminationInfoGetConfig.isUsingArea = false;//这里设置为使用面积
	NgEliminationInfoGetConfig.isUsingScore = true;//这里设置为使用分数
	NgEliminationInfoGetConfig.scoreRange = { 0,BadMap["maxScore"] };
	NgEliminationInfoGetConfig.areaRange = { 0,BadMap["maxArea"] };
	NgEliminationInfoGetConfig.scoreIsUsingComplementarySet = false;
	eliminationInfoGetConfigs[ClassId::Bad] = NgEliminationInfoGetConfig;

	FengKouEliminationInfoGetConfig.areaFactor = FengKouMap["pixToWorld"];//这里设置为像素当量
	FengKouEliminationInfoGetConfig.scoreFactor = 100;//这里设置为百分比当量
	FengKouEliminationInfoGetConfig.isUsingArea = false;//这里设置为使用面积
	FengKouEliminationInfoGetConfig.isUsingScore = true;//这里设置为使用分数
	FengKouEliminationInfoGetConfig.scoreRange = { 0,FengKouMap["maxScore"] };
	FengKouEliminationInfoGetConfig.areaRange = { 0,FengKouMap["maxArea"] };
	FengKouEliminationInfoGetConfig.scoreIsUsingComplementarySet = false;
	eliminationInfoGetConfigs[ClassId::Bad] = FengKouEliminationInfoGetConfig;

	JiaoDaiEliminationInfoGetConfig.areaFactor = JiaoDaiMap["pixToWorld"];//这里设置为像素当量
	JiaoDaiEliminationInfoGetConfig.scoreFactor = 100;//这里设置为百分比当量
	JiaoDaiEliminationInfoGetConfig.isUsingArea = false;//这里设置为使用面积
	JiaoDaiEliminationInfoGetConfig.isUsingScore = true;//这里设置为使用分数
	JiaoDaiEliminationInfoGetConfig.scoreRange = { 0,JiaoDaiMap["maxScore"] };
	JiaoDaiEliminationInfoGetConfig.areaRange = { 0,JiaoDaiMap["maxArea"] };
	JiaoDaiEliminationInfoGetConfig.scoreIsUsingComplementarySet = false;
	eliminationInfoGetConfigs[ClassId::Bad] = JiaoDaiEliminationInfoGetConfig;

	context.eliminationCfg = eliminationInfoGetConfigs;

	iniDefectResultInfoFunc();
}

void ImageProcessorDuckTongue::initial_isBadVector()
{
	{
		QMutexLocker locker(&isBadVectorMutex);
		isBadVector.assign(50, false);
	}
}

void ImageProcessingModuleDuckTongue::BuildModule()
{
	for (int i = 0; i < _numConsumers; ++i) {
		static size_t workIndexCount = 0;
		ImageProcessorDuckTongue* processor = new ImageProcessorDuckTongue(_queue, _mutex, _condition, workIndexCount, this);
		workIndexCount++;
		processor->buildDetModelEngine(modelEnginePath);
		processor->imageProcessingModuleIndex = index;
		connect(processor, &ImageProcessorDuckTongue::imageReady, this, &ImageProcessingModuleDuckTongue::imageReady, Qt::QueuedConnection);
		connect(processor, &ImageProcessorDuckTongue::imageNGReady, this, &ImageProcessingModuleDuckTongue::imageNGReady, Qt::QueuedConnection);
		connect(processor, &ImageProcessorDuckTongue::updateMainWindowShowBtn, this, &ImageProcessingModuleDuckTongue::updateMainWindowShowBtn, Qt::QueuedConnection);

		connect(this, &ImageProcessingModuleDuckTongue::shibiekuangChanged, processor, &ImageProcessorDuckTongue::updateDrawRec, Qt::QueuedConnection);
		connect(this, &ImageProcessingModuleDuckTongue::wenziChanged, processor, &ImageProcessorDuckTongue::updateDrawText, Qt::QueuedConnection);
		connect(this, &ImageProcessingModuleDuckTongue::paramMapsChanged, processor, &ImageProcessorDuckTongue::updateParamMapsFromGlobalStruct, Qt::QueuedConnection);
		_processors.push_back(processor);
		processor->start();
	}
}

ImageProcessingModuleDuckTongue::ImageProcessingModuleDuckTongue(int numConsumers, QObject* parent)
	: QObject(parent), _numConsumers(numConsumers)
{

}

ImageProcessingModuleDuckTongue::~ImageProcessingModuleDuckTongue()
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

void ImageProcessingModuleDuckTongue::onFrameCaptured(rw::rqw::MatInfo matInfo, size_t index)
{
	// 防抖动处理
	auto& setConfig = GlobalData::getInstance().setConfig;
	const long long debounceMs = static_cast<long long>(std::max(0.0, setConfig.xiangjiguangdianpingbishijian));
	const auto minInterval = std::chrono::milliseconds(debounceMs);

	static std::atomic<long long> lastCamNs{ 0 };

	if (!AllowOncePer(lastCamNs, minInterval)) {
		return;
	}

	// 手动读取本地图片
	std::string imagePath = R"(C:\Users\zfkj4090\Desktop\Image_20241024165512138.jpg)"; // 替换为你的图片路径
	cv::Mat frame1 = cv::imread(imagePath, cv::IMREAD_COLOR);
	matInfo.mat = frame1.clone();
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
		// 手动读取本地图片
		std::string imagePath = R"(C:\Users\zfkj4090\Desktop\Image_202410241655121382.jpg)"; // 替换为你的图片路径
		cv::Mat frame1 = cv::imread(imagePath, cv::IMREAD_COLOR);
		HalconCpp::HObject hImage = MatToHImage(frame1);
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
