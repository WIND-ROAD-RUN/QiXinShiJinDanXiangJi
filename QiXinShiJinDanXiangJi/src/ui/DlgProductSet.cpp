#include "DlgProductSet.h"
#include "ui_DlgProductSet.h"

#include <QMessageBox>
#include <QTabWidget>
#include <QtConcurrent/qtconcurrentrun.h>

#include "GlobalStruct.hpp"
#include "NumberKeyboard.h"

DlgProductSet::DlgProductSet(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::DlgProductSetClass())
{
	ui->setupUi(this);

	build_ui();

	build_connect();
}

DlgProductSet::~DlgProductSet()
{
	delete ui;
}

void DlgProductSet::build_ui()
{
	read_config();
}

void DlgProductSet::read_config()
{
	auto& globalConfig = GlobalData::getInstance().setConfig;

	// 剔废时间
	ui->pbtn_tifeichixushijian1->setText(QString::number(globalConfig.tiFeiChiXuShiJian1));
	ui->pbtn_tifeijuli1->setText(QString::number(globalConfig.tifeijuli1));
	ui->pbtn_tifeichixushijian2->setText(QString::number(globalConfig.tiFeiChiXuShiJian2));
	ui->pbtn_tifeijuli2->setText(QString::number(globalConfig.tifeijuli2));

	// 采图
	ui->cBox_takeCamera1Pictures->setChecked(globalConfig.takeWork1Pictures);
	ui->cBox_takeCamera2Pictures->setChecked(globalConfig.takeWork2Pictures);

	// 存图
	ui->cBox_takeNgPictures->setChecked(globalConfig.saveNGImg);
	ui->cBox_takeMaskPictures->setChecked(globalConfig.saveMaskImg);
	ui->cBox_takeOkPictures->setChecked(globalConfig.saveOKImg);

	// 一工位
	ui->pbtn_shangxianwei1->setText(QString::number(globalConfig.shangXianWei1));
	ui->pbtn_xiaxianwei1->setText(QString::number(globalConfig.xiaXianWei1));
	ui->pbtn_zuoxianwei1->setText(QString::number(globalConfig.zuoXianWei1));
	ui->pbtn_youxianwei1->setText(QString::number(globalConfig.youXianWei1));
	ui->pbtn_baoguang1->setText(QString::number(globalConfig.baoguang1));
	ui->pbtn_zengyi1->setText(QString::number(globalConfig.zengyi1));
	ui->pbtn_xiangsudangliang1->setText(QString::number(globalConfig.xiangSuDangLiang1));

	// 二工位
	ui->pbtn_shangxianwei2->setText(QString::number(globalConfig.shangXianWei2));
	ui->pbtn_xiaxianwei2->setText(QString::number(globalConfig.xiaXianWei2));
	ui->pbtn_zuoxianwei2->setText(QString::number(globalConfig.zuoXianWei2));
	ui->pbtn_youxianwei2->setText(QString::number(globalConfig.youXianWei2));
	ui->pbtn_baoguang2->setText(QString::number(globalConfig.baoguang2));
	ui->pbtn_zengyi2->setText(QString::number(globalConfig.zengyi2));
	ui->pbtn_xiangsudangliang2->setText(QString::number(globalConfig.xiangSuDangLiang2));

	// 基本功能
	ui->cbox_qiyongerxiangji->setChecked(globalConfig.qiyongerxiangji);
	ui->cbox_qiyongxiangjichonglian->setChecked(globalConfig.qiyongxiangjichonglian);

	// 默认显示第一个
	ui->tabWidget->setCurrentIndex(0);
}

void DlgProductSet::build_connect()
{
	auto& globalThread = GlobalThread::getInstance();

	QObject::connect(ui->pbtn_tifeichixushijian1, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_tifeichixushijian1_clicked);
	QObject::connect(ui->pbtn_tifeijuli1, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_tifeijuli1_clicked);
	QObject::connect(ui->pbtn_tifeichixushijian2, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_tifeichixushijian2_clicked);
	QObject::connect(ui->pbtn_tifeijuli2, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_tifeijuli2_clicked);
	QObject::connect(ui->pbtn_shangxianwei1, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_shangxianwei1_clicked);
	QObject::connect(ui->pbtn_xiaxianwei1, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_xiaxianwei1_clicked);
	QObject::connect(ui->pbtn_zuoxianwei1, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_zuoxianwei1_clicked);
	QObject::connect(ui->pbtn_youxianwei1, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_youxianwei1_clicked);
	QObject::connect(ui->pbtn_baoguang1, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_baoguang1_clicked);
	QObject::connect(ui->pbtn_zengyi1, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_zengyi1_clicked);
	QObject::connect(ui->pbtn_xiangsudangliang1, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_xiangsudangliang1_clicked);
	QObject::connect(ui->pbtn_shangxianwei2, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_shangxianwei2_clicked);
	QObject::connect(ui->pbtn_xiaxianwei2, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_xiaxianwei2_clicked);
	QObject::connect(ui->pbtn_zuoxianwei2, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_zuoxianwei2_clicked);
	QObject::connect(ui->pbtn_youxianwei2, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_youxianwei2_clicked);
	QObject::connect(ui->pbtn_baoguang2, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_baoguang2_clicked);
	QObject::connect(ui->pbtn_zengyi2, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_zengyi2_clicked);
	QObject::connect(ui->pbtn_xiangsudangliang2, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_xiangsudangliang2_clicked);
	QObject::connect(ui->cBox_takeNgPictures, &QCheckBox::clicked,
		this, &DlgProductSet::cBox_takeNgPictures_checked);
	QObject::connect(ui->cBox_takeMaskPictures, &QCheckBox::clicked,
		this, &DlgProductSet::cBox_takeMaskPictures_checked);
	QObject::connect(ui->cBox_takeOkPictures, &QCheckBox::clicked,
		this, &DlgProductSet::cBox_takeOkPictures_checked);
	QObject::connect(ui->pbtn_close, &QPushButton::clicked,
		this, &DlgProductSet::pbtn_close_clicked);
	QObject::connect(ui->cBox_takeCamera1Pictures, &QCheckBox::clicked,
		this, &DlgProductSet::cBox_takeCamera1Pictures_checked);
	QObject::connect(ui->cBox_takeCamera2Pictures, &QCheckBox::clicked,
		this, &DlgProductSet::cBox_takeCamera2Pictures_checked);
	QObject::connect(ui->cbox_qiyongerxiangji, &QCheckBox::clicked,
		this, &DlgProductSet::cbox_qiyongerxiangji_checked);
}

void DlgProductSet::pbtn_close_clicked()
{
	auto& globalFunc = GlobalFuncObject::getInstance();

	globalFunc.saveSetConfig();
	this->close();
}

void DlgProductSet::pbtn_tifeichixushijian1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_tifeichixushijian1->setText(value);
		globalStructSetConfig.tiFeiChiXuShiJian1 = value.toDouble();
	}
}

void DlgProductSet::pbtn_tifeijuli1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_tifeijuli1->setText(value);
		globalStructSetConfig.tifeijuli1 = value.toDouble();
		emit tifeijuliChanged();
	}
}

void DlgProductSet::pbtn_tifeichixushijian2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_tifeichixushijian2->setText(value);
		globalStructSetConfig.tiFeiChiXuShiJian2 = value.toDouble();
	}
}

void DlgProductSet::pbtn_tifeijuli2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_tifeijuli2->setText(value);
		globalStructSetConfig.tifeijuli2 = value.toDouble();
		emit tifeijuliChanged();
	}
}

void DlgProductSet::pbtn_shangxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_shangxianwei1->setText(value);
		globalStructSetConfig.shangXianWei1 = value.toDouble();
	}
}

void DlgProductSet::pbtn_xiaxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_xiaxianwei1->setText(value);
		globalStructSetConfig.xiaXianWei1 = value.toDouble();
	}
}

void DlgProductSet::pbtn_zuoxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_zuoxianwei1->setText(value);
		globalStructSetConfig.zuoXianWei1 = value.toDouble();
	}
}

void DlgProductSet::pbtn_youxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_youxianwei1->setText(value);
		globalStructSetConfig.youXianWei1 = value.toDouble();
	}
}

void DlgProductSet::pbtn_baoguang1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		auto& globalThread = GlobalThread::getInstance();
		ui->pbtn_baoguang1->setText(value);
		globalStructSetConfig.baoguang1 = value.toDouble();
		if (globalThread.camera1)
		{
			globalThread.camera1->setExposureTime(static_cast<size_t>(value.toDouble()));
		}
	}
}

void DlgProductSet::pbtn_zengyi1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		auto& globalThread = GlobalThread::getInstance();
		ui->pbtn_zengyi1->setText(value);
		globalStructSetConfig.zengyi1 = value.toDouble();
		if (globalThread.camera1)
		{
			globalThread.camera1->setGain(static_cast<size_t>(value.toDouble()));
		}
	}
}

void DlgProductSet::pbtn_xiangsudangliang1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_xiangsudangliang1->setText(value);
		globalStructSetConfig.xiangSuDangLiang1 = value.toDouble();
		emit pixToWorldChanged();
	}
}

void DlgProductSet::pbtn_shangxianwei2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_shangxianwei2->setText(value);
		globalStructSetConfig.shangXianWei2 = value.toDouble();
	}
}

void DlgProductSet::pbtn_xiaxianwei2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_xiaxianwei2->setText(value);
		globalStructSetConfig.xiaXianWei2 = value.toDouble();
	}
}

void DlgProductSet::pbtn_zuoxianwei2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_zuoxianwei2->setText(value);
		globalStructSetConfig.zuoXianWei2 = value.toDouble();
	}
}

void DlgProductSet::pbtn_youxianwei2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_youxianwei2->setText(value);
		globalStructSetConfig.youXianWei2 = value.toDouble();
	}
}

void DlgProductSet::pbtn_baoguang2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_baoguang2->setText(value);
		globalStructSetConfig.baoguang2 = value.toDouble();
	}
}

void DlgProductSet::pbtn_zengyi2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_zengyi2->setText(value);
		globalStructSetConfig.zengyi2 = value.toDouble();
	}
}

void DlgProductSet::pbtn_xiangsudangliang2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
		ui->pbtn_xiangsudangliang2->setText(value);
		globalStructSetConfig.xiangSuDangLiang2 = value.toDouble();
		emit pixToWorldChanged();
	}
}

void DlgProductSet::cBox_takeNgPictures_checked()
{
	auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
	globalStructSetConfig.saveNGImg = ui->cBox_takeNgPictures->isChecked();
}

void DlgProductSet::cBox_takeMaskPictures_checked()
{
	auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
	globalStructSetConfig.saveMaskImg = ui->cBox_takeMaskPictures->isChecked();
}

void DlgProductSet::cBox_takeOkPictures_checked()
{
	auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
	globalStructSetConfig.saveOKImg = ui->cBox_takeOkPictures->isChecked();
}

void DlgProductSet::cBox_takeCamera1Pictures_checked()
{
	auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
	globalStructSetConfig.takeWork1Pictures = ui->cBox_takeCamera1Pictures->isChecked();
}

void DlgProductSet::cBox_takeCamera2Pictures_checked()
{
	auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
	globalStructSetConfig.takeWork2Pictures = ui->cBox_takeCamera2Pictures->isChecked();
}

void DlgProductSet::cbox_qiyongerxiangji_checked()
{
	auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
	globalStructSetConfig.qiyongerxiangji = ui->cbox_qiyongerxiangji->isChecked();
}

void DlgProductSet::cbox_qiyongxiangjichonglian_checked()
{
	auto& globalStructSetConfig = GlobalData::getInstance().setConfig;
	globalStructSetConfig.qiyongxiangjichonglian = ui->cbox_qiyongxiangjichonglian->isChecked();
}



