#include "QiXinShiJinDanXiangJi.h"

#include <QMessageBox>
#include <QProcess>

#include "ui_QiXinShiJinDanXiangJi.h"
#include <QPushButton>
#include "GlobalStruct.hpp"
#include "NumberKeyboard.h"
#include "Utilty.hpp"

QiXinShiJinDanXiangJi::QiXinShiJinDanXiangJi(QWidget *parent)
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
	QObject::connect(ui->pbtn_score, &QPushButton::clicked,
		this, &QiXinShiJinDanXiangJi::pbtn_score_clicked);
	QObject::connect(ui->rbtn_debug, &QRadioButton::clicked,
		this, &QiXinShiJinDanXiangJi::rbtn_debug_checked);
	QObject::connect(ui->pbtn_openSaveLocation, &QPushButton::clicked,
		this, &QiXinShiJinDanXiangJi::pbtn_openSaveLocation_clicked);
	QObject::connect(ui->rbtn_takePicture, &QRadioButton::clicked,
		this, &QiXinShiJinDanXiangJi::rbtn_takePicture_checked);
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
	ui->label_productionYieldValue->setText(QString::number(paperCupsConfig.productionYield));
	ui->rbtn_takePicture->setChecked(paperCupsConfig.isSaveImg);
	ui->rbtn_removeFunc->setChecked(paperCupsConfig.isDefect);
	rbtn_removeFunc_checked(paperCupsConfig.isDefect);
	ui->ckb_shibiekuang->setChecked(paperCupsConfig.isshibiekuang);
	ui->ckb_wenzi->setChecked(paperCupsConfig.iswenzi);

	globalStruct.isTakePictures = ui->rbtn_takePicture->isChecked();

	// 初始化图像查看器
	_picturesViewer = new PictureViewerThumbnails(this);

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

	build_ImageSaveEngine();

	build_ImageEnlargedDisplay();

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

	destroy_ImageEnlargedDisplay();

	destroy_ImageSaveEngine();

	save_config();
}

void QiXinShiJinDanXiangJi::build_camera()
{
	auto& globalThread = GlobalThread::getInstance();

	globalThread.cameraIp1 = "1";
	globalThread.cameraIp2 = "2";

	auto build1Result = globalThread.buildCamera1();
	updateCameraLabelState(1, build1Result);

	auto build2Result = globalThread.buildCamera2();
	updateCameraLabelState(2, build2Result);
}

void QiXinShiJinDanXiangJi::start_camera()
{
	auto& globalThread = GlobalThread::getInstance();
	if (globalThread.camera1 != nullptr)
	{
		globalThread.camera1->startMonitor();
	}
	if (globalThread.camera2 != nullptr)
	{
		globalThread.camera2->startMonitor();
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
	QObject::connect(globalThread.modelCamera2.get(), &ImageProcessingModuleDuckTongue::imageReady, this, &QiXinShiJinDanXiangJi::onCamera2Display);
	QObject::connect(globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::imageNGReady, this, &QiXinShiJinDanXiangJi::onCameraNGDisplay);
	QObject::connect(globalThread.modelCamera2.get(), &ImageProcessingModuleDuckTongue::imageNGReady, this, &QiXinShiJinDanXiangJi::onCameraNGDisplay);
	QObject::connect(this, &QiXinShiJinDanXiangJi::shibiekuangChanged, globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::shibiekuangChanged);
	QObject::connect(this, &QiXinShiJinDanXiangJi::shibiekuangChanged, globalThread.modelCamera2.get(), &ImageProcessingModuleDuckTongue::shibiekuangChanged);
	QObject::connect(this, &QiXinShiJinDanXiangJi::wenziChanged, globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::wenziChanged);
	QObject::connect(this, &QiXinShiJinDanXiangJi::wenziChanged, globalThread.modelCamera2.get(), &ImageProcessingModuleDuckTongue::wenziChanged);
	QObject::connect(_dlgProductSet, &DlgProductSet::pixToWorldChanged, globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::paramMapsChanged);
	QObject::connect(_dlgProductSet, &DlgProductSet::pixToWorldChanged, globalThread.modelCamera2.get(), &ImageProcessingModuleDuckTongue::paramMapsChanged);
	QObject::connect(_dlgProductSet, &DlgProductSet::tifeijuliChanged, globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::paramMapsChanged);
	QObject::connect(_dlgProductSet, &DlgProductSet::tifeijuliChanged, globalThread.modelCamera2.get(), &ImageProcessingModuleDuckTongue::paramMapsChanged);
	QObject::connect(_dlgProductScore, &DlgProductScore::scoreFormClosed, globalThread.modelCamera1.get(), &ImageProcessingModuleDuckTongue::paramMapsChanged);
	QObject::connect(_dlgProductScore, &DlgProductScore::scoreFormClosed, globalThread.modelCamera2.get(), &ImageProcessingModuleDuckTongue::paramMapsChanged);
}

void QiXinShiJinDanXiangJi::destroy_ImageProcessingModule()
{
	auto& globalThread = GlobalThread::getInstance();
	globalThread.destroyImageProcessingModule();
}

void QiXinShiJinDanXiangJi::build_ImageSaveEngine()
{
	QDir dir;
	QString imageSavePath = globalPath.imageSaveRootPath;
	//清理旧的数据

	//获取当前日期并设置保存路径
	QString currentDate = QDate::currentDate().toString("yyyy_MM_dd");
	auto& globalFunc = GlobalFuncObject::getInstance();
	globalFunc.buildImageSaveEngine();
	QString imageSaveEnginePath = imageSavePath + currentDate;

	QString imagesFilePathFilePathFull = dir.absoluteFilePath(imageSaveEnginePath);
	globalFunc.imageSaveEngine->setRootPath(imagesFilePathFilePathFull);
	globalFunc.imageSaveEngine->startEngine();
}

void QiXinShiJinDanXiangJi::destroy_ImageSaveEngine()
{
	auto& globalFunc = GlobalFuncObject::getInstance();
	globalFunc.destroyImageSaveEngine();
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

void QiXinShiJinDanXiangJi::build_ImageEnlargedDisplay()
{
	imgDis1 = new rw::rqw::ClickableLabel(this);
	imgDis1->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

	imgDis2 = new rw::rqw::ClickableLabel(this);
	imgDis2->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

	imgNgDis1 = new rw::rqw::ClickableLabel(this);
	imgNgDis1->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

	imgNgDis2 = new rw::rqw::ClickableLabel(this);
	imgNgDis2->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

	ui->gBoix_ImageDisplay->layout()->replaceWidget(ui->label_imgDisplay_1, imgDis1);
	ui->gBoix_ImageDisplay->layout()->replaceWidget(ui->label_imgDisplay_3, imgDis2);
	ui->gBoix_ImageDisplay->layout()->replaceWidget(ui->label_imgDisplay_2, imgNgDis1);
	ui->gBoix_ImageDisplay->layout()->replaceWidget(ui->label_imgDisplay_4, imgNgDis2);

	delete ui->label_imgDisplay_1;
	delete ui->label_imgDisplay_3;
	delete ui->label_imgDisplay_2;
	delete ui->label_imgDisplay_4;

	QObject::connect(imgDis1, &rw::rqw::ClickableLabel::clicked
		, this, &QiXinShiJinDanXiangJi::imgDis1_clicked);
	QObject::connect(imgDis2, &rw::rqw::ClickableLabel::clicked
		, this, &QiXinShiJinDanXiangJi::imgDis2_clicked);
	QObject::connect(imgNgDis1, &rw::rqw::ClickableLabel::clicked
		, this, &QiXinShiJinDanXiangJi::imgNgDis1_clicked);
	QObject::connect(imgNgDis2, &rw::rqw::ClickableLabel::clicked
		, this, &QiXinShiJinDanXiangJi::imgNgDis2_clicked);

	_workStationTitleMap = {
		{0,"一号工位"},
		{1,"二号工位"},
		{2,"一号NG工位"},
		{3,"二号NG工位"}
	};

	_imageEnlargedDisplay = new ImageEnlargedDisplay(this);
	_imageEnlargedDisplay->setMonitorValue(&_isImageEnlargedDisplay);
	_imageEnlargedDisplay->setMonitorDisImgIndex(&_currentImageEnlargedDisplayIndex);
	_imageEnlargedDisplay->initWorkStationTitleMap(_workStationTitleMap);
	_imageEnlargedDisplay->setNum(2);
	_imageEnlargedDisplay->show();
	_imageEnlargedDisplay->close();
}

void QiXinShiJinDanXiangJi::destroy_ImageEnlargedDisplay()
{
	if (_imageEnlargedDisplay)
	{
		_imageEnlargedDisplay->close();
		delete _imageEnlargedDisplay;
		_imageEnlargedDisplay = nullptr;
	}
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
	case 2:
		if (state) {
			ui->label_camera2State->setText("连接成功");
			ui->label_camera2State->setStyleSheet(QString("QLabel{color:rgb(0, 230, 0);} "));
		}
		else {
			ui->label_camera2State->setText("连接失败");
			ui->label_camera2State->setStyleSheet(QString("QLabel{color:rgb(230, 0, 0);} "));
		}
		break;
	default:
		break;
	}
}

void QiXinShiJinDanXiangJi::onCamera1Display(QPixmap image)
{
	if (!_isImageEnlargedDisplay)
	{
		imgDis1->setPixmap(image.scaled(imgDis1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}
	else
	{
		if (0 == _currentImageEnlargedDisplayIndex) {
			_imageEnlargedDisplay->setShowImg(image);
		}
	}
	_lastImage1 = image;
}

void QiXinShiJinDanXiangJi::onCamera2Display(QPixmap image)
{
	if (!_isImageEnlargedDisplay)
	{
		imgDis2->setPixmap(image.scaled(imgDis2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}
	else
	{
		if (1 == _currentImageEnlargedDisplayIndex) {
			_imageEnlargedDisplay->setShowImg(image);
		}
	}
	_lastImage2 = image;
}

void QiXinShiJinDanXiangJi::onCameraNGDisplay(QPixmap image, size_t index, bool isbad)
{
	if (index == 1)
	{
		if (!_isImageEnlargedDisplay)
		{
			imgDis1->setPixmap(image.scaled(imgDis1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
			if (isbad)
			{
				imgNgDis1->setPixmap(image.scaled(imgNgDis1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
				_lastNgImage1 = image;
			}
		}
		else
		{
			if (0 == _currentImageEnlargedDisplayIndex)
			{
				_imageEnlargedDisplay->setShowImg(image);
			}
			if (isbad && 2 == _currentImageEnlargedDisplayIndex)
			{
				_imageEnlargedDisplay->setShowImg(image);
				_lastNgImage1 = image;
			}
		}
	}
	else if (index == 2)
	{
		if (!_isImageEnlargedDisplay)
		{
			imgDis2->setPixmap(image.scaled(imgDis2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
			if (isbad)
			{
				imgNgDis2->setPixmap(image.scaled(imgNgDis2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
				_lastNgImage2 = image;
			}
		}
		else
		{
			if (1 == _currentImageEnlargedDisplayIndex)
			{
				_imageEnlargedDisplay->setShowImg(image);
			}
			if (isbad && 3 == _currentImageEnlargedDisplayIndex)
			{
				_imageEnlargedDisplay->setShowImg(image);
				_lastNgImage2 = image;
			}
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
		if (_picturesViewer && _picturesViewer->isVisible())
			_picturesViewer->showMinimized();
		if (_imageEnlargedDisplay && _imageEnlargedDisplay->isVisible())
			_imageEnlargedDisplay->showMinimized();

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

void QiXinShiJinDanXiangJi::pbtn_score_clicked()
{
	_dlgProductScore->setFixedSize(this->width(), this->height());
	_dlgProductScore->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	_dlgProductScore->exec();
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
			if (globalThread.camera2)
			{
				globalThread.camera2->setTriggerState(false);
				globalThread.camera2->setFrameRate(5);
			}
			ui->rbtn_takePicture->setChecked(false);
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

void QiXinShiJinDanXiangJi::pbtn_openSaveLocation_clicked()
{
	auto& globalFunc = GlobalFuncObject::getInstance();
	QString imageSavePath = globalFunc.imageSaveEngine->getRootPath();

	_picturesViewer->setRootPath(imageSavePath);
	_picturesViewer->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	_picturesViewer->show();
}

void QiXinShiJinDanXiangJi::rbtn_takePicture_checked()
{
	if (ui->rbtn_debug->isChecked() == true)
	{
		ui->rbtn_takePicture->setChecked(false);
	}
	auto& duckTongueConfig = GlobalData::getInstance().duckTongueConfig;
	auto& globalStruct = GlobalData::getInstance();
	duckTongueConfig.isSaveImg = ui->rbtn_takePicture->isChecked();
	globalStruct.isTakePictures = ui->rbtn_takePicture->isChecked();
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
		if (globalThread.camera2)
		{
			globalThread.camera2->setTriggerState(true);
			globalThread.camera2->setFrameRate(50);
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

void QiXinShiJinDanXiangJi::imgDis1_clicked()
{
	if (!_lastImage1.isNull())
	{
		_imageEnlargedDisplay->setShowImg(_lastImage1);
	}
	else
	{
		_imageEnlargedDisplay->clearImgDis();
	}
	_currentImageEnlargedDisplayIndex = 0;
	_imageEnlargedDisplay->setGboxTitle(_workStationTitleMap[_currentImageEnlargedDisplayIndex]);
	_imageEnlargedDisplay->show();
}

void QiXinShiJinDanXiangJi::imgDis2_clicked()
{
	if (!_lastImage2.isNull())
	{
		_imageEnlargedDisplay->setShowImg(_lastImage2);
	}
	else
	{
		_imageEnlargedDisplay->clearImgDis();
	}
	_currentImageEnlargedDisplayIndex = 1;
	_imageEnlargedDisplay->setGboxTitle(_workStationTitleMap[_currentImageEnlargedDisplayIndex]);
	_imageEnlargedDisplay->show();
}

void QiXinShiJinDanXiangJi::imgNgDis1_clicked()
{
	if (!_lastNgImage1.isNull())
	{
		_imageEnlargedDisplay->setShowImg(_lastNgImage1);
	}
	else
	{
		_imageEnlargedDisplay->clearImgDis();
	}
	_currentImageEnlargedDisplayIndex = 2;
	_imageEnlargedDisplay->setGboxTitle(_workStationTitleMap[_currentImageEnlargedDisplayIndex]);
	_imageEnlargedDisplay->show();
}

void QiXinShiJinDanXiangJi::imgNgDis2_clicked()
{
	if (!_lastNgImage2.isNull())
	{
		_imageEnlargedDisplay->setShowImg(_lastNgImage2);
	}
	else
	{
		_imageEnlargedDisplay->clearImgDis();
	}
	_currentImageEnlargedDisplayIndex = 3;
	_imageEnlargedDisplay->setGboxTitle(_workStationTitleMap[_currentImageEnlargedDisplayIndex]);
	_imageEnlargedDisplay->show();
}

