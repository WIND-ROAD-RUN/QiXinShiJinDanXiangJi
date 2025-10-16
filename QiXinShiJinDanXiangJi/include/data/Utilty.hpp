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
	static constexpr int Ng = 0;
private:
	static int max() { return Ng; }
	static int min() { return Ng; }
};

struct Utility
{
	static QString cameraIp1;
	static QString zmotionIp;
};

