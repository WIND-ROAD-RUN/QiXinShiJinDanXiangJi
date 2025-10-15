#include "GlobalStruct.hpp"

#include "QiXinShiJinDanXiangJi.h"
#include "Utilty.hpp"
#include <QRegularExpression>

GlobalThread& GlobalThread::getInstance()
{
	static GlobalThread instance;
	return instance;
}

GlobalThread::GlobalThread()
{
}

GlobalThread::~GlobalThread()
{
}

void GlobalThread::buildCamera()
{
	buildCamera1();
	buildCamera2();
}

void GlobalThread::destroyCamera()
{
	destroyCamera1();
	destroyCamera2();
}

bool GlobalThread::buildCamera1()
{
	auto cameraList = rw::rqw::CheckCameraList();

	auto cameraMetaData1 = cameraMetaDataCheck(cameraIp1, cameraList);

	auto& globalDataSetConfig = GlobalData::getInstance().setConfig;

	if (cameraMetaData1.ip != "0")
	{
		try
		{
			camera1 = std::make_unique<rw::rqw::CameraPassiveThread>(this);
			camera1->initCamera(cameraMetaData1, rw::rqw::CameraObjectTrigger::Hardware);
			camera1->setTriggerState(true);
			camera1->cameraIndex = 1;
			camera1->setFrameRate(50);
			camera1->setHeartbeatTime(5000);
			camera1->setExposureTime(static_cast<size_t>(globalDataSetConfig.baoguang1));
			camera1->setGain(static_cast<size_t>(globalDataSetConfig.zengyi1));

			QObject::connect(camera1.get(), &rw::rqw::CameraPassiveThread::frameCaptured,
				modelCamera1.get(), &ImageProcessingModuleDuckTongue::onFrameCaptured, Qt::DirectConnection);

			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}
	return false;
}

bool GlobalThread::buildCamera2()
{
	auto cameraList = rw::rqw::CheckCameraList();

	auto cameraMetaData2 = cameraMetaDataCheck(cameraIp2, cameraList);

	auto& globalDataSetConfig = GlobalData::getInstance().setConfig;

	if (cameraMetaData2.ip != "0")
	{
		try
		{
			camera2 = std::make_unique<rw::rqw::CameraPassiveThread>(this);
			camera2->initCamera(cameraMetaData2, rw::rqw::CameraObjectTrigger::Hardware);
			camera2->setTriggerState(true);
			camera2->cameraIndex = 2;
			camera2->setFrameRate(50);
			camera2->setHeartbeatTime(5000);
			camera2->setExposureTime(static_cast<size_t>(globalDataSetConfig.baoguang2));
			camera2->setGain(static_cast<size_t>(globalDataSetConfig.zengyi2));

			QObject::connect(camera2.get(), &rw::rqw::CameraPassiveThread::frameCaptured,
				modelCamera2.get(), &ImageProcessingModuleDuckTongue::onFrameCaptured, Qt::DirectConnection);

			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}
	return false;
}

void GlobalThread::destroyCamera1()
{
	QObject::disconnect(camera1.get(), &rw::rqw::CameraPassiveThread::frameCaptured,
		modelCamera1.get(), &ImageProcessingModuleDuckTongue::onFrameCaptured);
	camera1.reset();
}

void GlobalThread::destroyCamera2()
{
	QObject::disconnect(camera2.get(), &rw::rqw::CameraPassiveThread::frameCaptured,
		modelCamera2.get(), &ImageProcessingModuleDuckTongue::onFrameCaptured);
	camera2.reset();
}

void GlobalThread::buildImageProcessorModules(const QString& path)
{
	modelCamera1 = std::make_unique<ImageProcessingModuleDuckTongue>(1);
	modelCamera2 = std::make_unique<ImageProcessingModuleDuckTongue>(1);

	modelCamera1->modelEnginePath = path;
	modelCamera2->modelEnginePath = path;

	modelCamera1->index = 1;
	modelCamera2->index = 2;

	modelCamera1->BuildModule();
	modelCamera2->BuildModule();
}

void GlobalThread::destroyImageProcessingModule()
{
	modelCamera1.reset();
	modelCamera2.reset();
}

void GlobalThread::build_PriorityQueue()
{
	auto compareNodeEqual = [](const float& a, const float& b) {
		return a == b;
		};
	auto compareNodePriority = [](const float& a, const float& b) {
		return a < b;
		};

	priorityQueue = std::make_unique<rw::dsl::ThreadSafeHeap<float>>();
}

void GlobalThread::destroy_PriorityQueue()
{
	priorityQueue.reset();
}

void GlobalThread::build_DetachDefectThreadDuckTongue()
{
	detachDefectThreadDuckTongue = new DetachDefectThreadDuckTongue(this);
}

void GlobalThread::destroy_DetachDefectThreadDuckTongue()
{
	if (detachDefectThreadDuckTongue)
	{
		delete detachDefectThreadDuckTongue;
		detachDefectThreadDuckTongue = nullptr;
	}
}

void GlobalThread::build_CameraAndCardStateThreadDuckTongue()
{
	cameraAndCardStateThreadDuckTongue = new CameraAndCardStateThreadDuckTongue(this);
	// 更新UI界面
	QObject::connect(cameraAndCardStateThreadDuckTongue, &CameraAndCardStateThreadDuckTongue::updateCameraLabelState,
		this, &GlobalThread::emit_updateUiLabels, Qt::QueuedConnection);
	// 相机重连
	QObject::connect(cameraAndCardStateThreadDuckTongue, &CameraAndCardStateThreadDuckTongue::buildCamera1,
		this, &GlobalThread::rebuild_Camera1, Qt::QueuedConnection);
	QObject::connect(cameraAndCardStateThreadDuckTongue, &CameraAndCardStateThreadDuckTongue::buildCamera2,
		this, &GlobalThread::rebuild_Camera2, Qt::QueuedConnection);
	// 相机销毁
	QObject::connect(cameraAndCardStateThreadDuckTongue, &CameraAndCardStateThreadDuckTongue::destroyCamera1,
		this, &GlobalThread::destroy_Camera1, Qt::QueuedConnection);
	QObject::connect(cameraAndCardStateThreadDuckTongue, &CameraAndCardStateThreadDuckTongue::destroyCamera2,
		this, &GlobalThread::destroy_Camera2, Qt::QueuedConnection);
}

void GlobalThread::destory_CameraAndCardStateThreadDuckTongue()
{
	if (cameraAndCardStateThreadDuckTongue)
	{
		delete cameraAndCardStateThreadDuckTongue;
		cameraAndCardStateThreadDuckTongue = nullptr;
	}
}

void GlobalThread::rebuild_Camera1()
{
	buildCamera1();
	if (camera1)
	{
		camera1->startMonitor();
	}
}

void GlobalThread::rebuild_Camera2()
{
	buildCamera2();
	if (camera2)
	{
		camera2->startMonitor();
	}
}

void GlobalThread::destroy_Camera1()
{
	destroyCamera1();
}

void GlobalThread::destroy_Camera2()
{
	destroyCamera2();
}

GlobalFuncObject& GlobalFuncObject::getInstance()
{
	static GlobalFuncObject instance;
	return instance;
}

GlobalFuncObject::GlobalFuncObject()
{
}

GlobalFuncObject::~GlobalFuncObject()
{
}

GlobalData::GlobalData()
{

}

GlobalData::~GlobalData()
{

}

void GlobalFuncObject::buildConfigManager(rw::oso::StorageType type)
{
	storeContext = std::make_unique<rw::oso::StorageContext>(type);
}

void GlobalFuncObject::saveDuckTongueConfig()
{
	auto& globalStruct = GlobalData::getInstance();
	std::string generalConfigPath = globalPath.duckTongueConfigPath.toStdString();
	storeContext->saveSafe(globalStruct.duckTongueConfig, generalConfigPath);
}

void GlobalFuncObject::saveScoreConfig()
{
	auto& globalStruct = GlobalData::getInstance();
	std::string scoreConfigPath = globalPath.scoreConfigPath.toStdString();
	storeContext->saveSafe(globalStruct.scoreConfig, scoreConfigPath);
}

void GlobalFuncObject::saveSetConfig()
{
	auto& globalStruct = GlobalData::getInstance();
	std::string setConfigPath = globalPath.setConfigPath.toStdString();
	storeContext->saveSafe(globalStruct.setConfig, setConfigPath);
}

void GlobalFuncObject::buildImageSaveEngine()
{
	imageSaveEngine = std::make_unique<rw::rqw::ImageSaveEngine>(this, 2);
}

void GlobalFuncObject::destroyImageSaveEngine()
{
	if (imageSaveEngine)
	{
		imageSaveEngine->stop();
		imageSaveEngine.reset();
	}
}


bool GlobalThread::isTargetCamera(const QString& cameraIndex, const QString& targetName)
{
	QRegularExpression regex(R"((\d+)\.(\d+)\.(\d+)\.(\d+))");
	QRegularExpressionMatch match = regex.match(targetName);

	if (match.hasMatch()) {
		auto matchString = match.captured(3);

		return cameraIndex == matchString;
	}

	return false;
}

rw::rqw::CameraMetaData GlobalThread::cameraMetaDataCheck(const QString& cameraIndex,
	const QVector<rw::rqw::CameraMetaData>& cameraInfo)
{
	for (const auto& cameraMetaData : cameraInfo) {
		if (isTargetCamera(cameraIndex, cameraMetaData.ip)) {
			return cameraMetaData;
		}
	}
	rw::rqw::CameraMetaData error;
	error.ip = "0";
	return error;
}
