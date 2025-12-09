#include "Modules.hpp"

#include <QDir>
#include <QMessageBox>

#include "GlobalStruct.hpp"
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
	// 构建相机
	auto cameraBuild = cameraModule.build();

	return true;
}

void Modules::destroy()
{
	cameraModule.destroy();
}

void Modules::start()
{
	cameraModule.start();
}

void Modules::stop()
{
	cameraModule.stop();
}

void Modules::connect()
{
#pragma region connect camera and imgProModule
	QObject::connect(&cameraModule, &CameraModule::frameCaptured1,
		GlobalThread::getInstance().modelCamera1.get(), &ImageProcessingModule::onFrameCaptured, Qt::DirectConnection);
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
