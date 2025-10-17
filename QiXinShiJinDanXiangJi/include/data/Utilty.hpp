#pragma once
#include <QString>

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
}globalPath;


struct ClassId
{
public:
	static constexpr int Bad = 0;
	static constexpr int FengKou = 1;
	static constexpr int JiaoDai = 2;

	static constexpr int min = 0;
	static constexpr int max = 2;
	static constexpr int num = 3;
};

struct Utility
{
	static QString cameraIp1;
	static QString zmotionIp;
};

