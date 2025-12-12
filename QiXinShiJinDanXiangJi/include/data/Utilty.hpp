#pragma once
#include <QString>

#include "imgPro_ImageProcessUtilty.hpp"

inline struct GlobalPath
{
public:
	QString projectHome = R"(D:\zfkjData\QiXinShiJinDanXiangJi\)";
public:
	QString configRootPath = projectHome + R"(config\)";
	QString modelRootPath = projectHome + R"(model\)";
	QString qiXinShiJinDanXiangJiConfigPath = configRootPath + R"(QiXinShiJinDanXiangJiConfig.xml)";
	QString setConfigPath = configRootPath + R"(SetConfig.xml)";
	QString modelPath = modelRootPath + R"(QiXinShiJin.engine)";
public:
	QString imageSaveRootPath = R"(D:\Images\)";
}globalPath;


struct ClassId
{
public:
	static constexpr int Bad = 0;
	static constexpr int FengKou = 1;
	static constexpr int JiaoDai = 2;

	static constexpr int minNum = 0;
	static constexpr int maxNum = 2;
	static constexpr int num = 3;

	static std::unordered_map<rw::imgPro::ClassId, rw::imgPro::ClassIdName> classIdNameMap;
	static std::vector<rw::imgPro::ClassId> classids;
};

struct Utility
{
	static QString cameraIp1;
	static QString zmotionIp;
};

