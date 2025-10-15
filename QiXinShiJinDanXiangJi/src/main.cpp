#include <QMessageBox>
#include <QtWidgets/QApplication>
#include "rqw_RunEnvCheck.hpp"
#include "Utilty.hpp"
#include "QiXinShiJinDanXiangJi.h"
#include "DuckTongue.hpp"
#include "oso_func.hpp"
#include "ScoreConfig.hpp"
#include "SetConfig.hpp"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QiXinShiJinDanXiangJi w;
	// 运行前检查
#pragma region running software check
	if (!rw::rqw::RunEnvCheck::isSingleInstance("WetPapers.exe"))
	{
		QMessageBox::warning(nullptr, "提示", "请勿重复开启软件。");
		return -1;
	}
	if (rw::rqw::RunEnvCheck::isProcessRunning("MVS.exe"))
	{
		QMessageBox::warning(nullptr, "提示", "检测到海康威视软件正在运行，请先关闭后再启动本程序。");
		return -2;
	}
	if (rw::rqw::RunEnvCheck::isProcessRunning("BasedCam3.exe"))
	{
		QMessageBox::warning(nullptr, "提示", "检测到度申相机平台软件正在运行，请先关闭后再启动本程序。");
		return -3;
	}
	if (!rw::rqw::RunEnvCheck::isFileExist(globalPath.configRootPath))
	{
		QDir dir;
		dir.mkpath(globalPath.configRootPath);
	}
#pragma endregion


#pragma region check config file if format is error
	rw::oso::StorageContext storageContext(rw::oso::StorageType::Xml);

	{
		auto path = globalPath.duckTongueConfigPath;
		std::shared_ptr<rw::oso::ObjectStoreAssembly> loadData = nullptr;
		try
		{
			if (QFile::exists(path)) {
				loadData = storageContext.loadSafe(path.toStdString());
				if (!loadData)
				{
					QFile::remove(path);
				}

				cdm::DuckTongueConfig config = *loadData;
			}
		}
		catch (std::runtime_error& e)
		{
			if (loadData)
			{
				rw::oso::ObjectStoreAssembly oldAssembly = *loadData;
				rw::oso::ObjectStoreAssembly newAssembly = cdm::DuckTongueConfig();
				rw::oso::AssemblyMergeTool::Merge(newAssembly, oldAssembly);
				auto isSuccess = storageContext.saveSafe(newAssembly, globalPath.duckTongueConfigPath.toStdString());
				if (isSuccess)
				{
					qDebug() << "配置文件duckTongueConfig.xml更新成功";
				}
			}
		}
	}

	{
		auto path = globalPath.scoreConfigPath;
		std::shared_ptr<rw::oso::ObjectStoreAssembly> loadData = nullptr;
		try
		{
			if (QFile::exists(path)) {
				loadData = storageContext.loadSafe(path.toStdString());
				if (!loadData)
				{
					QFile::remove(path);
				}

				cdm::ScoreConfig config = *loadData;
			}
		}
		catch (std::runtime_error& e)
		{
			if (loadData)
			{
				rw::oso::ObjectStoreAssembly oldAssembly = *loadData;
				rw::oso::ObjectStoreAssembly newAssembly = cdm::ScoreConfig();
				rw::oso::AssemblyMergeTool::Merge(newAssembly, oldAssembly);
				auto isSuccess = storageContext.saveSafe(newAssembly, globalPath.scoreConfigPath.toStdString());
				if (isSuccess)
				{
					qDebug() << "配置文件scoreConfig.xml更新成功";
				}
			}
		}
	}

	{
		auto path = globalPath.setConfigPath;
		std::shared_ptr<rw::oso::ObjectStoreAssembly> loadData = nullptr;
		try
		{
			if (QFile::exists(path)) {
				loadData = storageContext.loadSafe(path.toStdString());
				if (!loadData)
				{
					QFile::remove(path);
				}

				cdm::SetConfig config = *loadData;
			}
		}
		catch (std::runtime_error& e)
		{
			if (loadData)
			{
				rw::oso::ObjectStoreAssembly oldAssembly = *loadData;
				rw::oso::ObjectStoreAssembly newAssembly = cdm::SetConfig();
				rw::oso::AssemblyMergeTool::Merge(newAssembly, oldAssembly);
				auto isSuccess = storageContext.saveSafe(newAssembly, globalPath.setConfigPath.toStdString());
				if (isSuccess)
				{
					qDebug() << "配置文件setConfig.xml更新成功";
				}
			}
		}
	}

#pragma endregion

#ifdef NDEBUG
	w.showFullScreen();
#else
	w.show();
#endif
    return a.exec();
}
