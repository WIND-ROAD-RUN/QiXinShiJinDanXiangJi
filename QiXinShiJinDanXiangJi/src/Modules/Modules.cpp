#include "Modules.hpp"

#include <QDir>
#include <QMessageBox>

#include "DlgProductSet.h"
#include "GlobalStruct.hpp"
#include "QiXinShiJinDanXiangJi.h"
#include "QiXinShiJinDanXiangJi.hpp"
#include "rqw_RunEnvCheck.hpp"
#include "SetConfig.hpp"
#include "Utilty.hpp"


Modules::Modules()
{

}

Modules::~Modules()
{

}

bool Modules::build()
{
	// 读取配置
	auto configManagerBuild = configManagerModule.build();

	// 构建相机
	auto cameraBuild = cameraModule.build();

	// 构建显示信息模块
	auto runtimeInfoModuleBuild = runtimeInfoModule.build();

	// 构建UI模块
	uiModule.build();

	// 构建重连模块
	reconnectModule.build();

	// 构建运动控制模块
	auto motionControllerModuleBuild = motionControllerModule.build();

	return true;
}

void Modules::destroy()
{
	runtimeInfoModule.destroy();
	cameraModule.destroy();
	configManagerModule.destroy();
	uiModule.destroy();
	reconnectModule.destroy();
	motionControllerModule.destroy();
}

void Modules::start()
{
	uiModule.start();
	configManagerModule.start();
	motionControllerModule.start();
	runtimeInfoModule.start();
	cameraModule.start();
	reconnectModule.start();
}

void Modules::stop()
{
	reconnectModule.stop();
	cameraModule.stop();
	runtimeInfoModule.stop();
	motionControllerModule.stop();
	configManagerModule.stop();
	uiModule.stop();
}

void Modules::connect()
{
#pragma region connect camera and imgProModule
	QObject::connect(&cameraModule, &CameraModule::frameCaptured1,
		GlobalThread::getInstance().modelCamera1.get(), &ImageProcessingModule::onFrameCaptured, Qt::DirectConnection);
#pragma endregion

#pragma region connect zmotion and ReconnectModule
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThreadQiXinShiJin::buildZMotion,
		&motionControllerModule, &MotionControllerModule::onBuildZMotion);
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThreadQiXinShiJin::destroyZMotion,
		&motionControllerModule, &MotionControllerModule::onDestroyZMotion);
#pragma endregion

#pragma region connect UIModule and ReconnectModule
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThreadQiXinShiJin::updateCameraLabelState,
		uiModule._qiXinShiJinDanXiangJi, &QiXinShiJinDanXiangJi::updateCameraLabelState);
#pragma endregion

#pragma region connect UIModule and imgProModule
	QObject::connect(uiModule._dlgProductSet,&DlgProductSet::paramsChanged,
		GlobalThread::getInstance().modelCamera1.get(), &ImageProcessingModule::paramMapsChanged);
#pragma endregion

#pragma region connect UIModules
	QObject::connect(uiModule._dlgProductSet, &DlgProductSet::emit_changeLanguage,
		uiModule._qiXinShiJinDanXiangJi, &QiXinShiJinDanXiangJi::changeLanguage);
#pragma endregion

#pragma region connect camera and ReconnectModule
	// 相机重连
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThreadQiXinShiJin::buildCamera,
		&cameraModule, &CameraModule::onBuildCamera);
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThreadQiXinShiJin::startMonitor,
		&cameraModule, &CameraModule::onStartCamera);
	// 相机销毁
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThreadQiXinShiJin::destroyCamera,
		&cameraModule, &CameraModule::onDestroyCamera);
#pragma endregion

#pragma region connect imgProModule and RuntimeInfoModule
	QObject::connect(GlobalThread::getInstance().modelCamera1.get(), &ImageProcessingModule::updateStatisticalInfo,
		runtimeInfoModule.detachUtiltyThread.get(), &DetachUtiltyThread::updateStatisticalInfo);
#pragma endregion

#pragma region connect UIModule and RuntimeInfoModule
	QObject::connect(runtimeInfoModule.detachUtiltyThread.get(), &DetachUtiltyThread::updateStatisticalInfo,
		uiModule._qiXinShiJinDanXiangJi, &QiXinShiJinDanXiangJi::onUpdateStatisticalInfoUI, Qt::QueuedConnection);
#pragma endregion

}

bool Modules::check()
{
#pragma region check single instance
	if (!rw::rqw::RunEnvCheck::isSingleInstance("QiXinShiJinDanXiangJi.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "已经有程序在运行，请勿多次打开");
		return false;
	}
#pragma endregion

#pragma region check run env
	if (rw::rqw::RunEnvCheck::isProcessRunning("MVS.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "检测到海康威视软件正在运行，请先关闭后再启动本程序。");
		return false;
	}

	if (rw::rqw::RunEnvCheck::isProcessRunning("BasedCam3.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "检测到度申相机平台软件正在运行，请先关闭后再启动本程序。");
		return false;
	}
#pragma endregion

#pragma region check directory exist
	EnsureDirectoryExists(globalPath.projectHome);
	EnsureDirectoryExists(globalPath.configRootPath);
	EnsureDirectoryExists(globalPath.modelRootPath);
#pragma endregion

#pragma region check model exist
	if (!rw::rqw::RunEnvCheck::isFileExist(globalPath.modelPath))
	{
		QMessageBox::warning(nullptr, "错误", "模型文件缺失");
		return false;
	}
#pragma endregion

#pragma region check config format and exist
	rw::oso::StorageContext storageContext(rw::oso::StorageType::Xml);

	checkFileExistAndFormat<cdm::QiXinShiJinDanXiangJiConfig>(globalPath.qiXinShiJinDanXiangJiConfigPath, storageContext);
	checkFileExistAndFormat<cdm::SetConfig>(globalPath.setConfigPath, storageContext);
#pragma endregion

	return true;
}

bool Modules::EnsureDirectoryExists(const QString& dirPath)
{
	QDir dir(dirPath);
	if (!dir.exists()) {
		return dir.mkpath(".");
	}
	return true;
}
