#pragma once
#include <QString>

inline struct GlobalPath
{
public:
	QString projectHome = R"(D:\zfkjData\DuckTongue\)";
public:
	QString configRootPath = projectHome + R"(config\)";
	QString modelRootPath = projectHome + R"(model\)";
	QString duckTongueConfigPath = configRootPath + R"(DuckTongueConfig.xml)";
	QString scoreConfigPath = configRootPath + R"(ScoreConfig.xml)";
	QString setConfigPath = configRootPath + R"(SetConfig.xml)";
	QString modelPath = modelRootPath + R"(DuckTongue.engine)";
public:
	QString imageSaveRootPath = projectHome + R"(SavedImages\)";

}globalPath;


struct ClassId
{
public:
	static constexpr int Ng = 0;
private:
	static int max() { return Ng; }
	static int min() { return Ng; }
};

