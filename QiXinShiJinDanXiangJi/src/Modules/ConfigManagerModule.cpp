#include "ConfigManagerModule.hpp"

#include "Modules.hpp"
#include "Utilty.hpp"

bool ConfigManagerModule::build()
{
	storeContext = std::make_unique<rw::oso::StorageContext>(rw::oso::StorageType::Xml);
	//auto& runtimeModule = Modules::getInstance().runtimeInfoModule;

#pragma region readHandleScannerCfg
	auto loadMainWindowConfig = storeContext->loadSafe(globalPath.qiXinShiJinDanXiangJiConfigPath.toStdString());
	if (loadMainWindowConfig)
	{
		qiXinShiJinDanXiangJiConfig = *loadMainWindowConfig;
		/*runtimeModule.isTakePictures = handleScannerConfig.isSaveImg;
		runtimeModule.statisticalInfo.produceCount = handleScannerConfig.totalProductionVolume;
		runtimeModule.statisticalInfo.wasteCount = handleScannerConfig.totalDefectiveVolume;
		runtimeModule.statisticalInfo.productionYield = handleScannerConfig.productionYield;
		runtimeModule.statisticalInfo.handleCountForStop = handleScannerConfig.tingjigeshu;*/
	}
#pragma endregion

#pragma region readsetCfg
	loadMainWindowConfig = storeContext->loadSafe(globalPath.setConfigPath.toStdString());
	if (loadMainWindowConfig)
	{
		setConfig = *loadMainWindowConfig;
	}
#pragma endregion

	return true;
}

void ConfigManagerModule::destroy()
{
	storeContext->saveSafe(qiXinShiJinDanXiangJiConfig, globalPath.qiXinShiJinDanXiangJiConfigPath.toStdString());
	storeContext->saveSafe(setConfig, globalPath.setConfigPath.toStdString());
	storeContext.reset();
}

void ConfigManagerModule::start()
{
	
}

void ConfigManagerModule::stop()
{

}
