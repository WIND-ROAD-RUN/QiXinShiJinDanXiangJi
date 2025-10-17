#include "ImageProcessorModule.hpp"
#include "GlobalStruct.hpp"
#include "Utilty.hpp"

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

void ImageProcessorDuckTongue::run_OpenRemoveFunc(MatInfo& frame)
{
	_isbad = false;
	auto& imgPro = *_imgProcess;
	imgPro(frame.image);
	// 更新屏蔽线
	updateShieldWires();
	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();

	auto imageRealLocation = frame.location;
	auto& context = _imgProcess->context();

	run_OpenRemoveFunc_emitErrorInfo(defectResult.isBad);

	drawBoundariesLines(maskImg);

	emit imageNGReady(QPixmap::fromImage(maskImg), frame.index, defectResult.isBad);

	if (defectResult.isBad)
	{
		_isbad = true;
	}
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
		//globalThread.priorityQueue->push(realLeftLocationDifference);
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

	auto& globalThread = GlobalThread::getInstance();

	QMutexLocker locker(&_mutex);
	MatInfo mat;
	mat.image = matInfo.mat;
	mat.index = index;
	if (index == 1)
	{
		//mat.location = globalThread.zmotion.getModbus(2, 1);	// 获取拍照的位置
	}
	else
	{
		//mat.location = globalThread.zmotion.getModbus(6, 1);	// 获取拍照的位置
	}
	_queue.enqueue(mat);
	_condition.wakeOne();
}
