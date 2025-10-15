#pragma once

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class DlgProductSetClass; };
QT_END_NAMESPACE

class DlgProductSet : public QDialog
{
	Q_OBJECT

public:
	DlgProductSet(QWidget* parent = nullptr);
	~DlgProductSet();

public:
	void build_ui();
	void read_config();
	void build_connect();

signals:
	void pixToWorldChanged();
	void tifeijuliChanged();

private slots:
	void pbtn_close_clicked();

	void pbtn_tifeichixushijian1_clicked();
	void pbtn_tifeijuli1_clicked();
	void pbtn_tifeichixushijian2_clicked();
	void pbtn_tifeijuli2_clicked();
	void pbtn_shangxianwei1_clicked();
	void pbtn_xiaxianwei1_clicked();
	void pbtn_zuoxianwei1_clicked();
	void pbtn_youxianwei1_clicked();
	void pbtn_baoguang1_clicked();
	void pbtn_zengyi1_clicked();
	void pbtn_xiangsudangliang1_clicked();
	void pbtn_shangxianwei2_clicked();
	void pbtn_xiaxianwei2_clicked();
	void pbtn_zuoxianwei2_clicked();
	void pbtn_youxianwei2_clicked();
	void pbtn_baoguang2_clicked();
	void pbtn_zengyi2_clicked();
	void pbtn_xiangsudangliang2_clicked();

	void cBox_takeNgPictures_checked();
	void cBox_takeMaskPictures_checked();
	void cBox_takeOkPictures_checked();
	void cBox_takeCamera1Pictures_checked();
	void cBox_takeCamera2Pictures_checked();
	void cbox_qiyongerxiangji_checked();
	void cbox_qiyongxiangjichonglian_checked();

private:
	Ui::DlgProductSetClass* ui;
};

