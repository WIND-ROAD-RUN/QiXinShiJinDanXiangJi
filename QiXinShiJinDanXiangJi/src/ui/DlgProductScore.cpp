#include "DlgProductScore.h"

#include <QMessageBox>

#include "GlobalStruct.hpp"
#include "NumberKeyboard.h"
#include "ui_DlgProductScore.h"

DlgProductScore::DlgProductScore(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DlgProductScoreClass())
{
	ui->setupUi(this);

	build_ui();

	build_connect();
}

DlgProductScore::~DlgProductScore()
{
	delete ui;
}

void DlgProductScore::build_ui()
{
	read_config();
}

void DlgProductScore::read_config()
{
	auto& globalScoreConfig = GlobalData::getInstance().scoreConfig;
	// 初始化参数

	// 脏污
	ui->rbtn_NgEnable->setChecked(globalScoreConfig.Ng);
	ui->ptn_NgSimilarity->setText(QString::number(globalScoreConfig.NgScore));
	ui->ptn_NgArea->setText(QString::number(globalScoreConfig.NgArea));
}

void DlgProductScore::build_connect()
{
	QObject::connect(ui->pbtn_close, &QPushButton::clicked,
		this, &DlgProductScore::pbtn_close_clicked);
	QObject::connect(ui->rbtn_NgEnable, &QRadioButton::clicked,
		this, &DlgProductScore::rbtn_NgEnable_checked);
	QObject::connect(ui->ptn_NgSimilarity, &QPushButton::clicked,
		this, &DlgProductScore::ptn_NgSimilarity_clicked);
	QObject::connect(ui->ptn_NgArea, &QPushButton::clicked,
		this, &DlgProductScore::ptn_NgArea_clicked);
}

void DlgProductScore::pbtn_close_clicked()
{
	auto& GlobalStructData = GlobalFuncObject::getInstance();
	GlobalStructData.saveScoreConfig();
	emit scoreFormClosed();
	this->close();
}

void DlgProductScore::rbtn_NgEnable_checked()
{
	auto& globalScoreConfig = GlobalData::getInstance().scoreConfig;
	globalScoreConfig.Ng = ui->rbtn_NgEnable->isChecked();
}

void DlgProductScore::ptn_NgSimilarity_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大与0的数值");
			return;
		}
		auto& globalStructScoreConfig = GlobalData::getInstance().scoreConfig;
		ui->ptn_NgSimilarity->setText(value);
		globalStructScoreConfig.NgScore = value.toDouble();
	}
}

void DlgProductScore::ptn_NgArea_clicked()
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
		auto& globalStructScoreConfig = GlobalData::getInstance().scoreConfig;
		ui->ptn_NgArea->setText(value);
		globalStructScoreConfig.NgArea = value.toDouble();
	}
}