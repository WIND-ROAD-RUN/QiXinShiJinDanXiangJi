#include "QiXinShiJinDanXiangJi.h"

#include <QMessageBox>
#include <QProcess>

#include "ui_QiXinShiJinDanXiangJi.h"
#include <QPushButton>
#include "GlobalStruct.hpp"
#include "NumberKeyboard.h"
#include "Utilty.hpp"

QiXinShiJinDanXiangJi::QiXinShiJinDanXiangJi(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::QiXinShiJinDanXiangJiClass())
{
	ui->setupUi(this);

	initializeComponents();
}

QiXinShiJinDanXiangJi::~QiXinShiJinDanXiangJi()
{
	destroyComponents();
	delete ui;
}

void QiXinShiJinDanXiangJi::build_ui()
{
	build_DuckTongueData();
	build_DlgProductScore();
	build_DlgProductSet();
	build_DlgCloseForm();
}

void QiXinShiJinDanXiangJi::build_connect()
{
	QObject::connect(ui->pbtn_exit, &QPushButton::clicked,
		this, &QiXinShiJinDanXiangJi::pbtn_exit_clicked);
	QObject::connect(ui->pbtn_set, &QPushButton::clicked,
		this, &QiXinShiJinDanXiangJi::pbtn_set_clicked);
	QObject::connect(ui->pbtn_start, &QPushButton::clicked,
		this, &QiXinShiJinDanXiangJi::pbtn_start_clicked);
	QObject::connect(ui->rbtn_debug, &QRadioButton::clicked,
		this, &QiXinShiJinDanXiangJi::rbtn_debug_checked);
	QObject::connect(ui->rbtn_removeFunc, &QRadioButton::clicked,
		this, &QiXinShiJinDanXiangJi::rbtn_removeFunc_checked);
	QObject::connect(ui->ckb_shibiekuang, &QCheckBox::clicked,
		this, &QiXinShiJinDanXiangJi::ckb_shibiekuang_checked);
	QObject::connect(ui->ckb_wenzi, &QCheckBox::clicked,
		this, &QiXinShiJinDanXiangJi::ckb_wenzi_checked);
	// 连接显示标题
	QObject::connect(clickableTitle, &rw::rqw::ClickableLabel::clicked,
		this, &QiXinShiJinDanXiangJi::lb_title_clicked);
}

void QiXinShiJinDanXiangJi::build_DuckTongueData()
{
	auto& globalStruct = GlobalData::getInstance();
	auto& paperCupsConfig = globalStruct.duckTongueConfig;
	paperCupsConfig.isDebug = false;
	paperCupsConfig.isDefect = true;		// 默认开启剔废
	paperCupsConfig.isshibiekuang = true;
	paperCupsConfig.iswenzi = false;

	ui->label_produceTotalValue->setText(QString::number(paperCupsConfig.totalProductionVolume));
	ui->label_wasteProductsValue->setText(QString::number(paperCupsConfig.totalDefectiveVolume));
	ui->rbtn_removeFunc->setChecked(paperCupsConfig.isDefect);
	rbtn_removeFunc_checked(paperCupsConfig.isDefect);
	ui->ckb_shibiekuang->setChecked(paperCupsConfig.isshibiekuang);
	ui->ckb_wenzi->setChecked(paperCupsConfig.iswenzi);

	ini_clickableTitle();
}

void QiXinShiJinDanXiangJi::build_DlgProductScore()
{
	_dlgProductScore = new DlgProductScore(this);
}

void QiXinShiJinDanXiangJi::build_DlgProductSet()
{
	_dlgProductSet = new DlgProductSet(this);
}

void QiXinShiJinDanXiangJi::ini_clickableTitle()
{
	// 初始化标题label
	clickableTitle = new rw::rqw::ClickableLabel(this);
	auto layoutTitle = ui->groupBox_head->layout();
	layoutTitle->replaceWidget(ui->label_title, clickableTitle);
	delete ui->label_title;
	clickableTitle->setText("鸭舌检测");
	clickableTitle->setStyleSheet("QLabel {font-size: 30px;font-weight: bold;color: rgb(255, 255, 255);padding: 5px 5px;border-bottom: 2px solid #cccccc;}");
}

void QiXinShiJinDanXiangJi::build_DlgCloseForm()
{
	_dlgCloseForm = new DlgCloseForm(this);
}

void QiXinShiJinDanXiangJi::read_config()
{
	auto& globalFuncObject = GlobalFuncObject::getInstance();
	globalFuncObject.buildConfigManager(rw::oso::StorageType::Xml);

	read_config_DuckTongueConfig();
	read_config_DlgProductScoreConfig();
	read_config_DlgProductSetConfig();
}

void QiXinShiJinDanXiangJi::read_config_DuckTongueConfig()
{
	auto& globalFunc = GlobalFuncObject::getInstance();
	auto& globalData = GlobalData::getInstance();

	globalFunc.storeContext->ensureFileExistsSafe(globalPath.duckTongueConfigPath.toStdString(), cdm::DuckTongueConfig());
	auto loadResult = globalFunc.storeContext->loadSafe(globalPath.duckTongueConfigPath.toStdString());
	if (!loadResult)
	{
		globalFunc.storeContext->saveSafe(cdm::DuckTongueConfig(), globalPath.duckTongueConfigPath.toStdString());
		return;
	}
	globalData.duckTongueConfig = *loadResult;
}

void QiXinShiJinDanXiangJi::read_config_DlgProductScoreConfig()
{
	auto& globalFunc = GlobalFuncObject::getInstance();
	auto& globalData = GlobalData::getInstance();

	globalFunc.storeContext->ensureFileExistsSafe(globalPath.scoreConfigPath.toStdString(), cdm::ScoreConfig());
	auto loadResult = globalFunc.storeContext->loadSafe(globalPath.scoreConfigPath.toStdString());
	if (!loadResult)
	{
		globalFunc.storeContext->saveSafe(cdm::ScoreConfig(), globalPath.scoreConfigPath.toStdString());
		return;
	}
	globalData.scoreConfig = *loadResult;
}

void QiXinShiJinDanXiangJi::read_config_DlgProductSetConfig()
{
	auto& globalFunc = GlobalFuncObject::getInstance();
	auto& globalData = GlobalData::getInstance();

	globalFunc.storeContext->ensureFileExistsSafe(globalPath.setConfigPath.toStdString(), cdm::SetConfig());
	auto loadResult = globalFunc.storeContext->loadSafe(globalPath.setConfigPath.toStdString());
	if (!loadResult)
	{
		globalFunc.storeContext->saveSafe(cdm::SetConfig(), globalPath.setConfigPath.toStdString());
		return;
	}
	globalData.setConfig = *loadResult;
}

void QiXinShiJinDanXiangJi::save_config()
{
	auto& globalFuncObject = GlobalFuncObject::getInstance();

	globalFuncObject.saveDuckTongueConfig();
	globalFuncObject.saveScoreConfig();
	globalFuncObject.saveSetConfig();
}

void QiXinShiJinDanXiangJi::start_Threads()
{
	auto& globalThread = GlobalThread::getInstance();
	// 启动异步剔废线程
	globalThread.detachDefectThreadDuckTongue->startThread();
	// 启动相机重连线程
	globalThread.cameraAndCardStateThreadDuckTongue->startThread();
}

void QiXinShiJinDanXiangJi::stop_Threads()
{
	auto& globalThread = GlobalThread::getInstance();

	globalThread.detachDefectThreadDuckTongue->stopThread();

	globalThread.cameraAndCardStateThreadDuckTongue->stopThread();
}

void QiXinShiJinDanXiangJi::initializeComponents()
{
	auto& globalThread = GlobalThread::getInstance();

	read_config();

	build_ui();

	build_ImageProcessingModule();

	build_camera();

	build_PriorityQueue();

	build_DetachDefectThreadDuckTongue();

	build_CameraAndBoardReconnectThread();

	build_connect();

	start_Threads();

#ifndef BUILD_WITHOUT_HARDWARE
	start_camera();
#endif

}

void QiXinShiJinDanXiangJi::destroyComponents()
{
	auto& globalThread = GlobalThread::getInstance();

	stop_Threads();

	destroy_CameraAndBoardReconnectThread();

	destroy_DetachDefectThreadDuckTongue();

	destroy_PriorityQueue();

	destroy_ImageProcessingModule();

	destroy_camera();

	save_config();
}

void QiXinShiJinDanXiangJi::build_camera()
{
	auto& globalThread = GlobalThread::getInstance();

	globalThread.cameraIp1 = "1";

	auto build1Result = globalThread.buildCamera1();
	updateCameraLabelState(1, build1Result);
}

void QiXinShiJinDanXiangJi::start_camera()
{
	auto& globalThread = GlobalThread::getInstance();
	if (globalThread.camera1 != nullptr)
	{
		globalThread.camera1->startMonitor();
	}
}

void QiXinShiJinDanXiangJi::destroy_camera()
{
	auto& globalThread = GlobalThread::getInstance();
	globalThread.destroyCamera();
}

void QiXinShiJinDanXiangJi::build_ImageProcessingModule()
{
	auto& globalThread = GlobalThread::getInstance();
	auto& globalData = GlobalData::getInstance();

	QDir dir;

	QString enginePathFull = globalPath.modelPath;

	QFileInfo engineFile(enginePathFull);

	if (!engineFile.exists()) {
		QMessageBox::critical(this, "Error", "Engine file or Name file does not exist. The application will now exit.");
		QApplication::quit();
		return;
	}

	globalThread.buildImageProcessorModules(enginePathFull);

	QObject::connect(globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::imageReady, this, &QiXinShiJinDanXiangJi::onCamera1Display);
	QObject::connect(globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::imageNGReady, this, &QiXinShiJinDanXiangJi::onCameraNGDisplay);
	QObject::connect(this, &QiXinShiJinDanXiangJi::shibiekuangChanged, globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::shibiekuangChanged);
	QObject::connect(this, &QiXinShiJinDanXiangJi::wenziChanged, globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::wenziChanged);
	QObject::connect(_dlgProductSet, &DlgProductSet::pixToWorldChanged, globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::paramMapsChanged);
	QObject::connect(_dlgProductSet, &DlgProductSet::tifeijuliChanged, globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::paramMapsChanged);
	QObject::connect(_dlgProductScore, &DlgProductScore::scoreFormClosed, globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::paramMapsChanged);
}

void QiXinShiJinDanXiangJi::destroy_ImageProcessingModule()
{
	auto& globalThread = GlobalThread::getInstance();
	globalThread.destroyImageProcessingModule();
}

void QiXinShiJinDanXiangJi::build_CameraAndBoardReconnectThread()
{
	auto& globalThread = GlobalThread::getInstance();
	globalThread.build_CameraAndCardStateThreadDuckTongue();

	QObject::connect(&globalThread, &GlobalThread::emit_updateUiLabels,
		this, &QiXinShiJinDanXiangJi::updateCameraLabelState);
}

void QiXinShiJinDanXiangJi::destroy_CameraAndBoardReconnectThread()
{
	auto& globalThread = GlobalThread::getInstance();
	globalThread.destory_CameraAndCardStateThreadDuckTongue();
}

void QiXinShiJinDanXiangJi::build_PriorityQueue()
{
	auto& globalThread = GlobalThread::getInstance();
	globalThread.build_PriorityQueue();
}

void QiXinShiJinDanXiangJi::destroy_PriorityQueue()
{
	auto& globalThread = GlobalThread::getInstance();
	globalThread.destroy_PriorityQueue();
}

void QiXinShiJinDanXiangJi::build_DetachDefectThreadDuckTongue()
{
	auto& globalThread = GlobalThread::getInstance();
	globalThread.build_DetachDefectThreadDuckTongue();
}

void QiXinShiJinDanXiangJi::destroy_DetachDefectThreadDuckTongue()
{
	auto& globalThread = GlobalThread::getInstance();
	globalThread.destroy_DetachDefectThreadDuckTongue();
}

void QiXinShiJinDanXiangJi::updateCameraLabelState(int cameraIndex, bool state)
{
	switch (cameraIndex)
	{
	case 0:
		if (state) {
			ui->label_cardState->setText("连接成功");
			ui->label_cardState->setStyleSheet(QString("QLabel{color:rgb(0, 230, 0);} "));
		}
		else {
			ui->label_cardState->setText("连接失败");
			ui->label_cardState->setStyleSheet(QString("QLabel{color:rgb(230, 0, 0);} "));
		}
		break;
	case 1:
		if (state) {
			ui->label_camera1State->setText("连接成功");
			ui->label_camera1State->setStyleSheet(QString("QLabel{color:rgb(0, 230, 0);} "));
		}
		else {
			ui->label_camera1State->setText("连接失败");
			ui->label_camera1State->setStyleSheet(QString("QLabel{color:rgb(230, 0, 0);} "));
		}
		break;
	default:
		break;
	}
}

void QiXinShiJinDanXiangJi::onCamera1Display(QPixmap image)
{
	ui->label_imgDisplay_1->setPixmap(image.scaled(ui->label_imgDisplay_1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void QiXinShiJinDanXiangJi::onCameraNGDisplay(QPixmap image, size_t index, bool isbad)
{
	if (index == 1)
	{
		ui->label_imgDisplay_1->setPixmap(image.scaled(ui->label_imgDisplay_1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
		if (isbad)
		{
			ui->label_imgNgDisplay->setPixmap(image.scaled(ui->label_imgNgDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
		}
	}
}

void QiXinShiJinDanXiangJi::lb_title_clicked()
{
	if (0 != minimizeCount)
	{
		minimizeCount--;
	}
	else if (0 >= minimizeCount)
	{
		// 最小化主窗体
		this->showMinimized();

		// 最小化所有子窗体（如果已创建且可见）
		if (_dlgProductSet && _dlgProductSet->isVisible())
			_dlgProductSet->showMinimized();
		if (_dlgProductScore && _dlgProductScore->isVisible())
			_dlgProductScore->showMinimized();

		minimizeCount = 3; // 重置最小化计数器
	}

}

void QiXinShiJinDanXiangJi::pbtn_exit_clicked()
{
#ifdef NDEBUG
	if (_dlgCloseForm)
	{
		_dlgCloseForm->exec();
	}
#else
	this->close();
#endif
}

void QiXinShiJinDanXiangJi::pbtn_set_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		if (numKeyBord.getValue() == "1234") {
			_dlgProductSet->setFixedSize(this->width(), this->height());
			_dlgProductSet->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
			_dlgProductSet->exec();
		}
		else {
			QMessageBox::warning(this, "Error", "密码错误，请重新输入");
		}
	}
}

void QiXinShiJinDanXiangJi::pbtn_start_clicked()
{

}

void QiXinShiJinDanXiangJi::rbtn_debug_checked(bool checked)
{
	auto isRuning = ui->rbtn_removeFunc->isChecked();

	auto& globalThread = GlobalThread::getInstance();
	auto& globalData = GlobalData::getInstance();
	if (!isRuning) {
		if (checked) {
			globalData.runningState = RunningState::Debug;
			if (globalThread.camera1)
			{
				globalThread.camera1->setTriggerState(false);
				globalThread.camera1->setFrameRate(5);
			}
		}
		else {
			globalData.runningState = RunningState::Stop;
		}
		ui->ckb_shibiekuang->setVisible(checked);
		ui->ckb_wenzi->setVisible(checked);
	}
	else {
		ui->rbtn_debug->setChecked(false);
	}
}

void QiXinShiJinDanXiangJi::rbtn_removeFunc_checked(bool checked)
{
	auto& globalData = GlobalData::getInstance();
	auto& globalThread = GlobalThread::getInstance();
	if (checked)
	{
		globalData.runningState = RunningState::OpenRemoveFunc;
		if (globalThread.camera1)
		{
			globalThread.camera1->setTriggerState(true);
			globalThread.camera1->setFrameRate(50);
		}
		ui->rbtn_debug->setChecked(false);
		ui->ckb_shibiekuang->setVisible(false);
		ui->ckb_wenzi->setVisible(false);
	}
	else
	{
		globalData.runningState = RunningState::Stop;
	}
}

void QiXinShiJinDanXiangJi::ckb_shibiekuang_checked(bool checked)
{
	auto& globalData = GlobalData::getInstance();
	globalData.duckTongueConfig.isshibiekuang = ui->ckb_shibiekuang->isChecked();

	emit shibiekuangChanged();
}

void QiXinShiJinDanXiangJi::ckb_wenzi_checked(bool checked)
{
	auto& globalData = GlobalData::getInstance();
	globalData.duckTongueConfig.iswenzi = ui->ckb_wenzi->isChecked();

	emit wenziChanged();
}