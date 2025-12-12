#include "ImgProModule.hpp"

#include "Modules.hpp"
#include "osoFIleUtiltyFunc.hpp"
#include "Utilty.hpp"

bool ImgProModule::build()
{
	buildImgProContext();
	buildImageProcessingModule(imgProSignalWorkThreadNum);
	for (int i = 0; i < 4; i++)
	{
		imgProIsUpdate[i] = true;
	}
	return true;
}

void ImgProModule::destroy()
{
	destroyImageProcessingModule();
}

void ImgProModule::start()
{

}

void ImgProModule::stop()
{

}

void ImgProModule::buildImgProContext()
{
	buildImgProContextMain();
}

void ImgProModule::buildImgProContextMain()
{
	auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
	auto& qiXinShiJinDanXiangJiConfig = Modules::getInstance().configManagerModule.qiXinShiJinDanXiangJiConfig;

#pragma region  build base
	imageProcessContext_Main.imageProcessPrepare = [this, &runningState, &qiXinShiJinDanXiangJiConfig](rw::imgPro::ImageProcessContext& context)
		{
			auto& setConfig = Modules::getInstance().configManagerModule.setConfig;

			if (context.customFields.find("ImgProcessIndex") == context.customFields.end()) {
				return;
			}
			auto ImgProcessIndex = std::any_cast<int>(context.customFields["ImgProcessIndex"]);

			if (context.customFields.find("stationIdx") == context.customFields.end()) {
				return;
			}
			auto stationIdx = std::any_cast<int>(context.customFields["stationIdx"]);

			double currentPixToWorld{ 0 };

			//update pixToWorld
			{
				if (1 == ImgProcessIndex)
				{
					currentPixToWorld = setConfig.xiangsudangliang;
				}
				context.customFields["CurrentPixToWorld"] = static_cast<double>(currentPixToWorld);
			}

			//update Config
			{
				if (imgProIsUpdate[stationIdx])
				{
					context.eliminationCfg = cdm::ScoreConfigConvert::toClassIdWithEliConfigMap(setConfig, currentPixToWorld * currentPixToWorld, 100);
					context.defectCfg = cdm::ScoreConfigConvert::toClassIdWithDefConfigMap(setConfig);
					imgProIsUpdate[stationIdx] = false;
				}
			}

			// update limite
			{
				int limitTop{ 0 };
				int limitBottom{ 0 };

				if (1 == ImgProcessIndex)
				{
					limitTop = static_cast<int>(setConfig.shangxianwei);
					limitBottom = static_cast<int>(setConfig.xiaxianwei);
				}

				context.customFields["LimitTop"] = static_cast<int>(limitTop);
				context.customFields["LimitBottom"] = static_cast<int>(limitBottom);
			}

			// update drawConfig
			{
				if (RunningState::Debug == runningState)
				{
					context.defectDrawCfg.textLocate = rw::imgPro::ConfigDrawRect::TextLocate::LeftTopIn;

					if (qiXinShiJinDanXiangJiConfig.isshibiekuang)
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

					if (qiXinShiJinDanXiangJiConfig.iswenzi)
					{
						context.runTextCfg.isDrawExtraText = true;
					}
					else
					{
						context.runTextCfg.isDrawExtraText = false;
					}
				}
				else if (RunningState::OpenRemoveFunc == runningState)
				{
					context.defectDrawCfg.isDrawDefects = true;
					context.defectDrawCfg.isDrawDisableDefects = true;
					context.defectDrawCfg.isDisAreaText = true;
					context.defectDrawCfg.isDisScoreText = true;

					context.runTextCfg.isDrawExtraText = false;
				}
			}
		};
#pragma endregion

#pragma region build index get
	imageProcessContext_Main.indexGetContext.removeIndicesIfByInfo = [this](const rw::DetectionRectangleInfo& info,
		rw::imgPro::ImageProcessContext& context)
		{
			bool isInShieldWires = false;
			int limitTop{ -1 };
			int limitBottom{ -1 };

			if (context.customFields.find("LimitTop") != context.customFields.end()) {
				limitTop = std::any_cast<int>(context.customFields["LimitTop"]);
			}
			if (context.customFields.find("LimitBottom") != context.customFields.end()) {
				limitBottom = std::any_cast<int>(context.customFields["LimitBottom"]);
			}

			if (-1 == limitTop || -1 == limitBottom)
			{
				return false;
			}

			if (info.center_y > limitTop && info.center_y < limitBottom)
			{
				isInShieldWires = true;
			}
			return !isInShieldWires;
		};
#pragma endregion

#pragma region build elimination config

#pragma endregion

#pragma region build defect config

#pragma endregion

#pragma region build defect draw
	imageProcessContext_Main.defectDrawCfg.classIdNameMap = ClassId::classIdNameMap;

	rw::imgPro::DefectDrawConfigItem drawItemConfig;

	drawItemConfig.fontSize = 20;
	drawItemConfig.textLocate = rw::imgPro::ConfigDrawRect::TextLocate::LeftTopIn;
	drawItemConfig.isDisAreaText = false;
	drawItemConfig.isDisScoreText = false;

	for (size_t i = ClassId::minNum; i <= ClassId::maxNum; i++)
	{
		imageProcessContext_Main.defectDrawCfg.classIdWithConfigMap[i] = drawItemConfig;
	}

	imageProcessContext_Main.defectDrawFuncContext.postOperateFunc = [](
		QImage& img,
		rw::imgPro::ImageProcessContext& context)
		{

			int limitTop{ 0 };
			int limitBottom{ 0 };

			if (context.customFields.find("LimitTop") != context.customFields.end()) {
				limitTop = std::any_cast<int>(context.customFields["LimitTop"]);
			}
			if (context.customFields.find("LimitBottom") != context.customFields.end()) {
				limitBottom = std::any_cast<int>(context.customFields["LimitBottom"]);
			}

			rw::imgPro::ConfigDrawLine configDrawLine;
			configDrawLine.color = rw::imgPro::Color::Red;
			configDrawLine.thickness = 5;

			configDrawLine.position = limitTop;
			rw::imgPro::ImagePainter::drawHorizontalLine(img, configDrawLine);
			configDrawLine.position = limitBottom;
			rw::imgPro::ImagePainter::drawHorizontalLine(img, configDrawLine);
		};
#pragma endregion

#pragma region build running time text
	imageProcessContext_Main.runTextCfg.isDisProcessImgTime = true;
	imageProcessContext_Main.runTextCfg.isDrawExtraText = false;
#pragma endregion
}

void ImgProModule::resetImgProIsUpdate(bool state)
{
	for (auto& flag : imgProIsUpdate) {
		flag.store(state);
	}
}

void ImgProModule::buildImageProcessingModule(size_t num)
{
	imageProcessingModule1 = std::make_unique<ImageProcessingModule>(num, this);
	imageProcessingModule1->modelEnginePath = globalPath.modelPath;
	imageProcessingModule1->index = 1;
	imageProcessingModule1->BuildModule();
}

void ImgProModule::destroyImageProcessingModule()
{
	imageProcessingModule1.reset();
}

void ImgProModule::onUpdateImgProContext()
{
	buildImgProContext();
	resetImgProIsUpdate(true);
}
