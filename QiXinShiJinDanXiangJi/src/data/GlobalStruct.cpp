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

void GlobalThread::buildImageProcessorModules(const QString& path)
{
	modelCamera1 = std::make_unique<ImageProcessingModule>(1);

	modelCamera1->modelEnginePath = path;

	modelCamera1->index = 1;

	modelCamera1->BuildModule();
}

void GlobalThread::destroyImageProcessingModule()
{
	modelCamera1.reset();
}

void GlobalThread::build_PriorityQueue()
{
	auto compareNodeEqual = [](const float& a, const float& b) {
		return a == b;
		};
	auto compareNodePriority = [](const float& a, const float& b) {
		return a < b;
		};

	priorityQueue = std::make_unique<rw::dsl::ThreadSafeHeap<bool>>();
}

void GlobalThread::destroy_PriorityQueue()
{
	priorityQueue.reset();
}

void GlobalThread::build_DetachDefectThread()
{
	detachDefectThreadQiXinShiJin = new DetachDefectThreadQiXinShiJin(this);
}

void GlobalThread::destroy_DetachDefectThread()
{
	if (detachDefectThreadQiXinShiJin)
	{
		delete detachDefectThreadQiXinShiJin;
		detachDefectThreadQiXinShiJin = nullptr;
	}
}

void GlobalThread::build_DetachUtiltyThread()
{
	detachUtiltyThread = new DetachUtiltyThread(this);
}

void GlobalThread::destroy_DetachUtiltyThread()
{
	if (detachUtiltyThread)
	{
		delete detachUtiltyThread;
		detachUtiltyThread = nullptr;
	}
}

void GlobalThread::build_CameraAndCardStateThread()
{
	cameraAndCardStateThreadQiXinShiJin = new CameraAndCardStateThreadQiXinShiJin(this);
	// 更新UI界面
	QObject::connect(cameraAndCardStateThreadQiXinShiJin, &CameraAndCardStateThreadQiXinShiJin::updateCameraLabelState,
		this, &GlobalThread::emit_updateUiLabels, Qt::QueuedConnection);
	// 相机重连
	QObject::connect(cameraAndCardStateThreadQiXinShiJin, &CameraAndCardStateThreadQiXinShiJin::buildCamera1,
		this, &GlobalThread::rebuild_Camera1, Qt::QueuedConnection);
	// 相机销毁
	QObject::connect(cameraAndCardStateThreadQiXinShiJin, &CameraAndCardStateThreadQiXinShiJin::destroyCamera1,
		this, &GlobalThread::destroy_Camera1, Qt::QueuedConnection);
	// 运动控制器重连
	QObject::connect(cameraAndCardStateThreadQiXinShiJin, &CameraAndCardStateThreadQiXinShiJin::buildZMotion,
		this, &GlobalThread::rebuild_ZMotion, Qt::QueuedConnection);
	// 运动控制器销毁
	QObject::connect(cameraAndCardStateThreadQiXinShiJin, &CameraAndCardStateThreadQiXinShiJin::destroyZMotion,
		this, &GlobalThread::destroy_ZMotion, Qt::QueuedConnection);
}

void GlobalThread::destroy_CameraAndCardStateThread()
{
	if (cameraAndCardStateThreadQiXinShiJin)
	{
		delete cameraAndCardStateThreadQiXinShiJin;
		cameraAndCardStateThreadQiXinShiJin = nullptr;
	}
}

bool GlobalThread::build_ZMotion()
{
	zmotion = std::make_shared<rw::rqw::ZMotion>(Utility::zmotionIp);
	zmotion->setIp(Utility::zmotionIp);
	bool isConnected = zmotion->connect();
	return isConnected;
}

void GlobalThread::Destroy_ZMotion()
{
	if (zmotion)
	{
		bool isDisconnect = zmotion->disConnect();
		zmotion.reset();
	}
}

void GlobalThread::rebuild_Camera1()
{
	
}

void GlobalThread::destroy_Camera1()
{
	
}

void GlobalThread::rebuild_ZMotion()
{
	build_ZMotion();
}

void GlobalThread::destroy_ZMotion()
{
	Destroy_ZMotion();
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

void GlobalFuncObject::saveQiXinShiJinDanXiangJiConfig()
{
	auto& globalStruct = GlobalData::getInstance();
	std::string generalConfigPath = globalPath.qiXinShiJinDanXiangJiConfigPath.toStdString();
	storeContext->saveSafe(globalStruct.qiXinShiJinDanXiangJiConfig, generalConfigPath);
}

void GlobalFuncObject::saveSetConfig()
{
	auto& globalStruct = GlobalData::getInstance();
	std::string setConfigPath = globalPath.setConfigPath.toStdString();
	storeContext->saveSafe(globalStruct.setConfig, setConfigPath);
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
