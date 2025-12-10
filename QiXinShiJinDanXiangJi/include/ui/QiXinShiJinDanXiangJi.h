#pragma once

#include <QMainWindow>
#include <HalconCpp/HalconCpp.h>
#include <memory>
#include <atomic>
#include "rqw_LabelClickable.h"
#include "DlgCloseForm.h"
#include "oso_func.hpp"
#include "oso_StorageContext.hpp"
#include "rqw_RunEnvCheck.hpp"

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
	void build_QiXinShiJinDanXiangJiData();
	void ini_clickableTitle();
	void build_DlgCloseForm();
public:
	void read_config();
	void read_config_QiXinShiJinDanXiangJiConfig();
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
public:
	void build_ImageProcessingModule();
	void destroy_ImageProcessingModule();
public:
	void build_CameraAndBoardReconnectThread();
	void destroy_CameraAndBoardReconnectThread();
public:
	void build_PriorityQueue();
	void destroy_PriorityQueue();
public:
	void build_DetachDefectThreadDuckTongue();
	void destroy_DetachDefectThreadDuckTongue();
public:
	void build_DetachUtiltyThread();
	void destroy_DetachUtiltyThread();
public:
	void build_zmotion();
	void destroy_zmotion();
public slots:
	void changeLanguage(int index);
public slots:
	void updateCameraLabelState(int cameraIndex, bool state);

	void onUpdateStatisticalInfoUI();

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
	void pbtn_bagLength_clicked();
	void pbtn_bagWidth_clicked();
	void pbtn_resetProduct_clicked();

signals:
	void shibiekuangChanged();
	void wenziChanged();

public:
	static bool check();
	template<class TypeCanToAssembly>
	static void checkFileExistAndFormat(const QString& path, const rw::oso::StorageContext& context);
	static bool EnsureDirectoryExists(const QString& dirPath);

public:
	static void setModelImage(const HalconCpp::HObject& img);
	static std::shared_ptr<const HalconCpp::HObject> getModelImage();
	static void setIsModelImageLoaded(bool isLoaded);
	static bool getIsModelImageLoaded();
private:
	static std::shared_ptr<const HalconCpp::HObject> modelImage;
	static std::atomic_bool isModelImageLoaded;
private:
	void createButtonsOnWidget(QWidget* container);

	void cameraGetImageOnce();
public slots:
	void updateDefectButtonsFromVector();
private:
	rw::rqw::ClickableLabel* clickableTitle = nullptr;
	DlgCloseForm* _dlgCloseForm = nullptr;
private:
	Ui::QiXinShiJinDanXiangJiClass *ui;
	int minimizeCount{ 3 };
};

template <class TypeCanToAssembly>
void QiXinShiJinDanXiangJi::checkFileExistAndFormat(const QString& path, const rw::oso::StorageContext& context)
{
	if (rw::rqw::RunEnvCheck::isFileExist(path))
	{
		if (!rw::rqw::RunEnvCheck::isFileFormatCorrectWithSafe<TypeCanToAssembly>(path, context))
		{
			auto assembly = context.load(path.toStdString());
			bool isMerge{ false };
			auto mergeAssembly = rw::oso::AssemblyMergeTool::Merge(TypeCanToAssembly(), *assembly, isMerge);
			if (isMerge)
			{
				context.saveSafe(mergeAssembly, path.toStdString());
			}
			else
			{
				context.saveSafe(TypeCanToAssembly(), path.toStdString());
			}
		}
	}
	else
	{
		context.saveSafe(TypeCanToAssembly(), path.toStdString());
	}
}

