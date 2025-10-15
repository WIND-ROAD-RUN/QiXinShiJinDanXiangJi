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
	rw::rqw::ImageInfo imageInfo(rw::rqw::cvMatToQImage(frame.image));

	// 记得删除
	if (1 == imageProcessingModuleIndex)
	{
		index1Num++;
		qDebug() << "index1Num: " << index1Num;
	}
	else if (2 == imageProcessingModuleIndex)
	{
		index2Num++;
		qDebug() << "index2Num: " << index2Num;
	}

	save_image(imageInfo, maskImg);
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

	if (imageProcessingModuleIndex == 1)
	{
		++globalStruct.statisticalInfo.produceCount1;
	}
	else if (imageProcessingModuleIndex == 2)
	{
		++globalStruct.statisticalInfo.produceCount2;
	}

	if (isbad)
	{
		//globalThread.priorityQueue->push(realLeftLocationDifference);
	}
}

void ImageProcessorDuckTongue::save_image(rw::rqw::ImageInfo& imageInfo, const QImage& image)
{
	auto& globalStruct = GlobalData::getInstance();
	auto& setConfig = globalStruct.setConfig;

	if (!globalStruct.isTakePictures)
	{
		return;
	}

	if ((imageProcessingModuleIndex == 1 && setConfig.takeWork1Pictures) || (imageProcessingModuleIndex == 2 && setConfig.takeWork2Pictures))
	{
		save_image_work(imageInfo, image);
	}
}

void ImageProcessorDuckTongue::save_image_work(rw::rqw::ImageInfo& imageInfo, const QImage& image)
{
	auto& globalFunc = GlobalFuncObject::getInstance();
	auto& setConfig = GlobalData::getInstance().setConfig;
	if (_isbad) {
		if (setConfig.saveNGImg)
		{
			if (1 == imageProcessingModuleIndex)
			{
				imageInfo.classify = "NG1";
				globalFunc.imageSaveEngine->pushImage(imageInfo);
			}
			else if (2 == imageProcessingModuleIndex)
			{
				imageInfo.classify = "NG2";
				globalFunc.imageSaveEngine->pushImage(imageInfo);
			}
		}
		if (setConfig.saveMaskImg)
		{
			rw::rqw::ImageInfo mask(image);
			if (1 == imageProcessingModuleIndex)
			{
				mask.classify = "Mask1";
				globalFunc.imageSaveEngine->pushImage(mask);
			}
			else if (2 == imageProcessingModuleIndex)
			{
				mask.classify = "Mask2";
				globalFunc.imageSaveEngine->pushImage(mask);
			}
		}
	}
	else {
		if (setConfig.saveOKImg)
		{
			if (1 == imageProcessingModuleIndex)
			{
				imageInfo.classify = "OK1";
				globalFunc.imageSaveEngine->pushImage(imageInfo);
			}
			else if (2 == imageProcessingModuleIndex)
			{
				imageInfo.classify = "OK2";
				globalFunc.imageSaveEngine->pushImage(imageInfo);
			}
		}
	}
}

void ImageProcessorDuckTongue::buildSegModelEngine(const QString& enginePath)
{
	rw::ModelEngineConfig modelEngineConfig;
	modelEngineConfig.conf_threshold = 0.1f;
	modelEngineConfig.nms_threshold = 0.1f;
	modelEngineConfig.imagePretreatmentPolicy = rw::ImagePretreatmentPolicy::LetterBox;
	modelEngineConfig.letterBoxColor = cv::Scalar(114, 114, 114);
	modelEngineConfig.modelPath = enginePath.toStdString();
	auto engine = rw::ModelEngineFactory::createModelEngine(modelEngineConfig, rw::ModelType::Yolov11_Seg_CudaAcc, rw::ModelEngineDeployType::TensorRT);

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

	context.indexGetContext.removeIndicesIfByInfo = [this](const rw::DetectionRectangleInfo& info,const rw::imgPro::ImageProcessContext& imageProcessContext) {
		bool isInShieldWires = false;
		if (-1 == leftShieldWire || -1 == rightShieldWire || -1 == topShieldWire || -1 == bottomShieldWire)
		{
			return false;
		}
		if (info.center_x < rightShieldWire && info.center_x > leftShieldWire)
		{
			if (info.center_y > topShieldWire && info.center_y < bottomShieldWire)
			{
				isInShieldWires = true;
			}
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
	defectConfig.isEnable = NgMap["enable"];
	defectConfigs[ClassId::Ng] = defectConfig;
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
	drawConfig.fontSize = 105;
	drawConfig.classIdNameMap[0] = "重包";
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
	configDrawLine.thickness = 20;
	if (index == 1)
	{
		configDrawLine.position = setConfig.shangXianWei1;
		rw::imgPro::ImagePainter::drawHorizontalLine(image, configDrawLine);
		configDrawLine.position = setConfig.xiaXianWei1;
		rw::imgPro::ImagePainter::drawHorizontalLine(image, configDrawLine);
		configDrawLine.position = setConfig.zuoXianWei1;
		rw::imgPro::ImagePainter::drawVerticalLine(image, configDrawLine);
		configDrawLine.position = setConfig.youXianWei1;
		rw::imgPro::ImagePainter::drawVerticalLine(image, configDrawLine);
	}
	else if (index == 2)
	{
		configDrawLine.position = setConfig.shangXianWei2;
		rw::imgPro::ImagePainter::drawHorizontalLine(image, configDrawLine);
		configDrawLine.position = setConfig.xiaXianWei2;
		rw::imgPro::ImagePainter::drawHorizontalLine(image, configDrawLine);
		configDrawLine.position = setConfig.zuoXianWei2;
		rw::imgPro::ImagePainter::drawVerticalLine(image, configDrawLine);
		configDrawLine.position = setConfig.youXianWei2;
		rw::imgPro::ImagePainter::drawVerticalLine(image, configDrawLine);
	}
}

void ImageProcessorDuckTongue::updateShieldWires()
{
	auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
	auto& index = imageProcessingModuleIndex;
	if (1 == index)
	{
		leftShieldWire = globalStructSetConfig.zuoXianWei1;
		rightShieldWire = globalStructSetConfig.youXianWei1;
		topShieldWire = globalStructSetConfig.shangXianWei1;
		bottomShieldWire = globalStructSetConfig.xiaXianWei1;
	}
	else
	{
		leftShieldWire = globalStructSetConfig.zuoXianWei2;
		rightShieldWire = globalStructSetConfig.youXianWei2;
		topShieldWire = globalStructSetConfig.shangXianWei2;
		bottomShieldWire = globalStructSetConfig.xiaXianWei2;
	}

}

void ImageProcessorDuckTongue::updateDrawRec()
{
	auto& globalStruct = GlobalData::getInstance();
	auto& context = _imgProcess->context();
	if (globalStruct.duckTongueConfig.isshibiekuang)
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
	if (globalStruct.duckTongueConfig.iswenzi)
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

	NgMap["classId"] = 0;
	NgMap["maxArea"] = globalStruct.scoreConfig.NgArea;
	NgMap["maxScore"] = globalStruct.scoreConfig.NgScore;
	NgMap["enable"] = globalStruct.scoreConfig.Ng;
	if (1 == imageProcessingModuleIndex)
	{
		NgMap["pixToWorld"] = globalStruct.setConfig.xiangSuDangLiang1;
		pixToWorld = globalStruct.setConfig.xiangSuDangLiang1;
		tifeijuli = globalStruct.setConfig.tifeijuli1;
	}
	else
	{
		NgMap["pixToWorld"] = globalStruct.setConfig.xiangSuDangLiang2;
		pixToWorld = globalStruct.setConfig.xiangSuDangLiang2;
		tifeijuli = globalStruct.setConfig.tifeijuli2;
	}

	rw::imgPro::EliminationInfoFunc::ClassIdWithConfigMap eliminationInfoGetConfigs;
	rw::imgPro::EliminationInfoGetConfig NgEliminationInfoGetConfig;

	NgEliminationInfoGetConfig.areaFactor = NgMap["pixToWorld"];//这里设置为像素当量
	NgEliminationInfoGetConfig.scoreFactor = 100;//这里设置为百分比当量
	NgEliminationInfoGetConfig.isUsingArea = true;//这里设置为使用面积
	NgEliminationInfoGetConfig.isUsingScore = true;//这里设置为使用分数
	NgEliminationInfoGetConfig.scoreRange = { 0,NgMap["maxScore"] };
	NgEliminationInfoGetConfig.areaRange = { 0,NgMap["maxArea"] };
	NgEliminationInfoGetConfig.scoreIsUsingComplementarySet = false;
	eliminationInfoGetConfigs[ClassId::Ng] = NgEliminationInfoGetConfig;

	context.eliminationCfg = eliminationInfoGetConfigs;

	iniDefectResultInfoFunc();
}

void ImageProcessingModuleDuckTongue::BuildModule()
{
	for (int i = 0; i < _numConsumers; ++i) {
		static size_t workIndexCount = 0;
		ImageProcessorDuckTongue* processor = new ImageProcessorDuckTongue(_queue, _mutex, _condition, workIndexCount, this);
		workIndexCount++;
		processor->buildSegModelEngine(modelEnginePath);
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

void ImageProcessingModuleDuckTongue::onFrameCaptured(cv::Mat frame, size_t index)
{
	// 手动读取本地图片
	//std::string imagePath = "C:\\Users\\zzw\\Desktop\\saveimage\\123.jpg"; // 替换为你的图片路径
	//cv::Mat frame1 = cv::imread(imagePath, cv::IMREAD_COLOR);
	//frame = frame1.clone();
	if (frame.channels() == 4) {
		cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);
	}
	if (frame.type() != CV_8UC3) {
		frame.convertTo(frame, CV_8UC3);
	}

	if (frame.empty()) {
		return; // 跳过空帧
	}

	auto& globalThread = GlobalThread::getInstance();

	QMutexLocker locker(&_mutex);
	MatInfo mat;
	mat.image = frame;
	mat.index = index;
	if (index==1)
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
