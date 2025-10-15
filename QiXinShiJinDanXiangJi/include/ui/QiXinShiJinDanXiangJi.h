#pragma once

#include <QMainWindow>

#include "DlgProductScore.h"
#include "DlgProductSet.h"
#include "ImageEnlargedDisplay.h"
#include "PictureViewerThumbnails.h"
#include "rqw_LabelClickable.h"
#include "DlgCloseForm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QiXinShiJinDanXiangJiClass; };
QT_END_NAMESPACE

class QiXinShiJinDanXiangJi : public QMainWindow
{
	Q_OBJECT

public:
	QiXinShiJinDanXiangJi(QWidget *parent = nullptr);
	~QiXinShiJinDanXiangJi();
public:
	void build_ui();
	void build_connect();
	void build_DuckTongueData();
	void build_DlgProductScore();
	void build_DlgProductSet();
	void ini_clickableTitle();
	void build_DlgCloseForm();
public:
	void read_config();
	void read_config_DuckTongueConfig();
	void read_config_DlgProductScoreConfig();
	void read_config_DlgProductSetConfig();
public:
	void save_config();
public:
	void start_Threads();
	void stop_Threads();
public:
	void initializeComponents();
	void destroyComponents();
public:
	void build_camera();
	void start_camera();
	void destroy_camera();
public:
	void build_ImageProcessingModule();
	void destroy_ImageProcessingModule();
public:
public:
	void build_CameraAndBoardReconnectThread();
	void destroy_CameraAndBoardReconnectThread();
public:
public:
	void build_PriorityQueue();
	void destroy_PriorityQueue();
public:
	void build_DetachDefectThreadDuckTongue();
	void destroy_DetachDefectThreadDuckTongue();
private slots:
	void updateCameraLabelState(int cameraIndex, bool state);

	void onCamera1Display(QPixmap image);

	void onCameraNGDisplay(QPixmap image, size_t index, bool isbad);

	void lb_title_clicked();
private slots:
	void pbtn_exit_clicked();
	void pbtn_set_clicked();
	void pbtn_start_clicked();
	void rbtn_debug_checked(bool checked);
	void rbtn_removeFunc_checked(bool checked);
	void ckb_shibiekuang_checked(bool checked);
	void ckb_wenzi_checked(bool checked);

signals:
	void shibiekuangChanged();
	void wenziChanged();

private:
	rw::rqw::ClickableLabel* clickableTitle = nullptr;
public:
	DlgProductScore* _dlgProductScore = nullptr;
	DlgProductSet* _dlgProductSet = nullptr;
	DlgCloseForm* _dlgCloseForm = nullptr;
private:
	Ui::QiXinShiJinDanXiangJiClass *ui;
	int minimizeCount{ 3 };
};

