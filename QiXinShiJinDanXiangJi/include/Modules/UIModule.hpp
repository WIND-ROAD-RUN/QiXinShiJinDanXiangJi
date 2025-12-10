#pragma once

#include <QCheckBox>
#include <QObject>
#include "IModule.hpp"

class QiXinShiJinDanXiangJi;
class DlgProductSet;

class UIModule
	: public QObject, public IModule<void>
{
	Q_OBJECT
public:
	void build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	DlgProductSet* _dlgProductSet = nullptr;
	QiXinShiJinDanXiangJi* _qiXinShiJinDanXiangJi = nullptr;
};
