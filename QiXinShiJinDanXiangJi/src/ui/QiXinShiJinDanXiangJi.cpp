#include "QiXinShiJinDanXiangJi.h"

#include <QDir>
#include <QMessageBox>
#include <QProcess>

#include "ui_QiXinShiJinDanXiangJi.h"
#include <QPushButton>

#include "DlgProductSet.h"
#include "Modules.hpp"
#include "NumberKeyboard.h"
#include "rqw_RunEnvCheck.hpp"
#include "Utilty.hpp"

std::shared_ptr<const HalconCpp::HObject> QiXinShiJinDanXiangJi::modelImage{};
std::atomic_bool QiXinShiJinDanXiangJi::isModelImageLoaded{ false };


QiXinShiJinDanXiangJi::QiXinShiJinDanXiangJi(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::QiXinShiJinDanXiangJiClass())
{
	ui->setupUi(this);

	initializeComponents();

	const auto fixedSize = size();
	setMinimumSize(fixedSize);
	setMaximumSize(fixedSize);
}

QiXinShiJinDanXiangJi::~QiXinShiJinDanXiangJi()
{
	Modules::getInstance().stop();
	Modules::getInstance().destroy();
	delete ui;
}

#ifdef BUILD_WITHOUT_HARDWARE
void QiXinShiJinDanXiangJi::cBox_testPushImg_checked(bool checked)
{
	if (checked)
	{
		Modules::getInstance().test_module.testImgPush = true;
	}
	else
	{
		Modules::getInstance().test_module.testImgPush = false;
	}
}
#endif

void QiXinShiJinDanXiangJi::build_ui()
{
	build_QiXinShiJinDanXiangJiData();
	build_DlgCloseForm();

#ifdef BUILD_WITHOUT_HARDWARE
	cBox_testPushImg = new QCheckBox(this);
	cBox_testPushImg->setText("图像推送状态");
	ui->gBox_infor->layout()->addWidget(cBox_testPushImg);
	QObject::connect(cBox_testPushImg, &QCheckBox::clicked,
		this, &QiXinShiJinDanXiangJi::cBox_testPushImg_checked);
#endif
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
	QObject::connect(ui->pbtn_bagLength, &QPushButton::clicked,
		this, &QiXinShiJinDanXiangJi::pbtn_bagLength_clicked);
	QObject::connect(ui->pbtn_bagWidth, &QPushButton::clicked,
		this, &QiXinShiJinDanXiangJi::pbtn_bagWidth_clicked);
	QObject::connect(ui->pbtn_resetProduct, &QPushButton::clicked,
		this, &QiXinShiJinDanXiangJi::pbtn_resetProduct_clicked);
	// 连接显示标题
	QObject::connect(clickableTitle, &rw::rqw::ClickableLabel::clicked,
		this, &QiXinShiJinDanXiangJi::lb_title_clicked);
}

void QiXinShiJinDanXiangJi::build_QiXinShiJinDanXiangJiData()
{
	auto& qiXinShiJinDanXiangJiConfig = Modules::getInstance().configManagerModule.qiXinShiJinDanXiangJiConfig;
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	qiXinShiJinDanXiangJiConfig.isDebug = false;
	qiXinShiJinDanXiangJiConfig.isDefect = true;		// 默认开启剔废
	qiXinShiJinDanXiangJiConfig.isshibiekuang = true;
	qiXinShiJinDanXiangJiConfig.iswenzi = false;

	ui->label_produceTotalValue->setText(QString::number(qiXinShiJinDanXiangJiConfig.totalProductionVolume));
	ui->label_wasteProductsValue->setText(QString::number(qiXinShiJinDanXiangJiConfig.totalDefectiveVolume));
	ui->pbtn_bagLength->setText(QString::number(qiXinShiJinDanXiangJiConfig.setBagLength));
	ui->pbtn_bagWidth->setText(QString::number(qiXinShiJinDanXiangJiConfig.setBagWidth));
	ui->rbtn_removeFunc->setChecked(qiXinShiJinDanXiangJiConfig.isDefect);
	
	ui->ckb_shibiekuang->setChecked(qiXinShiJinDanXiangJiConfig.isshibiekuang);
	ui->ckb_wenzi->setChecked(qiXinShiJinDanXiangJiConfig.iswenzi);

	rbtn_removeFunc_checked(true);

	ini_clickableTitle();
	createButtonsOnWidget(ui->widget_showBtn);
	changeLanguage(setConfig.changeLanguageIndex);
}

void QiXinShiJinDanXiangJi::ini_clickableTitle()
{
	// 初始化标题label
	clickableTitle = new rw::rqw::ClickableLabel(this);
	auto layoutTitle = ui->groupBox_head->layout();
	layoutTitle->replaceWidget(ui->label_title, clickableTitle);
	delete ui->label_title;
	clickableTitle->setText("湿巾检测");
	clickableTitle->setStyleSheet("QLabel {font-size: 30px;font-weight: bold;color: rgb(255, 255, 255);padding: 5px 5px;border-bottom: 2px solid #cccccc;}");
}

void QiXinShiJinDanXiangJi::build_DlgCloseForm()
{
	_dlgCloseForm = new DlgCloseForm(this);
}

void QiXinShiJinDanXiangJi::initializeComponents()
{
	build_ui();

	build_camera();

	build_zmotion();

	build_connect();

#ifndef BUILD_WITHOUT_HARDWARE
#endif

	pbtn_start_clicked();
}

void QiXinShiJinDanXiangJi::build_camera()
{
	auto& cameraModules = Modules::getInstance().cameraModule;
	auto errors = cameraModules.getBuildResults();
	updateCameraLabelState(1, true);
	updateCameraLabelState(2, true);

	for (const auto& error : errors)
	{
		auto index = static_cast<int>(error);
		updateCameraLabelState(index, false);
	}
}

void QiXinShiJinDanXiangJi::build_zmotion()
{
	auto& motionControllerModule = Modules::getInstance().motionControllerModule;
	auto isBuildZmotion = motionControllerModule.zmotion->connect();
	updateCameraLabelState(0, isBuildZmotion);
}

void QiXinShiJinDanXiangJi::changeLanguage(int index)
{
	// 中文
	if (0 == index)
	{
		clickableTitle->setText("湿巾检测");
		ui->label_cameraStateTitle->setText("相机状态");
		ui->label_info->setText("统计信息");
		ui->pbtn_resetProduct->setText("产量清零");
		ui->label_produceTotal->setText("生产总量");
		ui->label_wasteProducts->setText("废品总量");
		ui->label_bagLengthTXT->setText("袋子长度");
		ui->label_bagWidthTXT->setText("袋子宽度");
		ui->rbtn_debug->setText("调试模式");
		ui->ckb_shibiekuang->setText("识别框");
		ui->ckb_wenzi->setText("文字");
		ui->rbtn_removeFunc->setText("剔除功能");
		ui->pbtn_start->setText("启动");
		ui->pbtn_set->setText("设置");
	}
	// 英文
	else if (1 == index)
	{
		clickableTitle->setText("WetPaper\nDetection");
		ui->label_cameraStateTitle->setText("CameraState");
		ui->label_info->setText("Statistics");
		ui->pbtn_resetProduct->setText("Clear");
		ui->label_produceTotal->setText("TotalProduction");
		ui->label_wasteProducts->setText("TotalWaste");
		ui->label_bagLengthTXT->setText("BagLength");
		ui->label_bagWidthTXT->setText("BagWidth");
		ui->rbtn_debug->setText("DebugMode");
		ui->ckb_shibiekuang->setText("IdentificationBox");
		ui->ckb_wenzi->setText("Word");
		ui->rbtn_removeFunc->setText("RejectionFunction");
		ui->pbtn_start->setText("Start");
		ui->pbtn_set->setText("Set");
	}
}

void QiXinShiJinDanXiangJi::updateCameraLabelState(int cameraIndex, bool state)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	switch (cameraIndex)
	{
	/*case 0:
		if (state) {
			if (0 == setConfig.changeLanguageIndex)
			{
				ui->label_cardState->setText("连接成功");
			}
			else if (1 == setConfig.changeLanguageIndex)
			{
				ui->label_cardState->setText("Connected");
			}
			ui->label_cardState->setStyleSheet(QString("QLabel{color:rgb(0, 230, 0);font-size: 18px;font - weight: bold;padding: 5px 5px;} "));
		}
		else {
			if (0 == setConfig.changeLanguageIndex)
			{
				ui->label_cardState->setText("连接失败");
			}
			else if (1 == setConfig.changeLanguageIndex)
			{
				ui->label_cardState->setText("Disconnected");
			}
			ui->label_cardState->setStyleSheet(QString("QLabel{color:rgb(230, 0, 0);font-size: 18px;font - weight: bold;padding: 5px 5px;} "));
		}
		break;*/
	case 1:
		if (state) {
			if (0 == setConfig.changeLanguageIndex)
			{
				ui->label_camera1State->setText("连接成功");
			}
			else if (1 == setConfig.changeLanguageIndex)
			{
				ui->label_camera1State->setText("Connected");
			}
			ui->label_camera1State->setStyleSheet(QString("QLabel{color:rgb(0, 230, 0);font-size: 18px;font - weight: bold;padding: 5px 5px;} "));
		}
		else {
			if (0 == setConfig.changeLanguageIndex)
			{
				ui->label_camera1State->setText("连接失败");
			}
			else if (1 == setConfig.changeLanguageIndex)
			{
				ui->label_camera1State->setText("Disconnected");
			}
			ui->label_camera1State->setStyleSheet(QString("QLabel{color:rgb(230, 0, 0);font-size: 18px;font - weight: bold;padding: 5px 5px;} "));
		}
		break;
	default:
		break;
	}
}

void QiXinShiJinDanXiangJi::onUpdateStatisticalInfoUI()
{
	auto& statisticalInfo = Modules::getInstance().runtimeInfoModule.statisticalInfo;
	ui->label_produceTotalValue->setText(QString::number(statisticalInfo.produceCount.load()));
	ui->label_wasteProductsValue->setText(QString::number(statisticalInfo.wasteCount.load()));
	ui->label_bagLength->setText(QString::number(statisticalInfo.bagLength.load()));
	ui->label_bagWidth->setText(QString::number(statisticalInfo.bagWidth.load()));
}

void QiXinShiJinDanXiangJi::onCamera1Display(QPixmap image)
{
	ui->label_imgDisplay_1->setPixmap(image.scaled(ui->label_imgDisplay_1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
	auto& _dlgProductSet = Modules::getInstance().uiModule._dlgProductSet;
	_dlgProductSet->setFixedSize(this->width(), this->height());
	_dlgProductSet->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	_dlgProductSet->exec();
}

void QiXinShiJinDanXiangJi::pbtn_start_clicked()
{
	setIsModelImageLoaded(false);

	auto& camera = Modules::getInstance().cameraModule.camera1;
	if (camera)
	{
		camera->softwareTrigger();
		camera->setTriggerState(true);
		camera->setTriggerSource(rw::rqw::TriggerSource::Line0);
	}
}

void QiXinShiJinDanXiangJi::rbtn_debug_checked(bool checked)
{
	auto isRuning = ui->rbtn_removeFunc->isChecked();

	auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
	auto& camera1 = Modules::getInstance().cameraModule.camera1;
	if (!isRuning) {
		if (checked) {
			runningState = RunningState::Debug;
			if (camera1)
			{
				camera1->setTriggerState(false);
				camera1->setFrameRate(5);
			}
		}
		else {
			runningState = RunningState::Stop;
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
	auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
	auto& camera1 = Modules::getInstance().cameraModule.camera1;
	if (checked)
	{
		runningState = RunningState::OpenRemoveFunc;
		if (camera1)
		{
			camera1->setTriggerState(true);
			camera1->setTriggerSource(rw::rqw::TriggerSource::Line0);
			camera1->setFrameRate(50);
		}
		ui->rbtn_debug->setChecked(false);
		ui->ckb_shibiekuang->setVisible(false);
		ui->ckb_wenzi->setVisible(false);
	}
	else
	{
		runningState = RunningState::Stop;
	}
}

void QiXinShiJinDanXiangJi::ckb_shibiekuang_checked(bool checked)
{
	auto& qiXinShiJinDanXiangJiConfig = Modules::getInstance().configManagerModule.qiXinShiJinDanXiangJiConfig;
	qiXinShiJinDanXiangJiConfig.isshibiekuang = ui->ckb_shibiekuang->isChecked();

	emit shibiekuangChanged();
}

void QiXinShiJinDanXiangJi::ckb_wenzi_checked(bool checked)
{
	auto& qiXinShiJinDanXiangJiConfig = Modules::getInstance().configManagerModule.qiXinShiJinDanXiangJiConfig;
	qiXinShiJinDanXiangJiConfig.iswenzi = ui->ckb_wenzi->isChecked();

	emit wenziChanged();
}

void QiXinShiJinDanXiangJi::pbtn_bagLength_clicked()
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
		auto& qiXinShiJinDanXiangJiConfig = Modules::getInstance().configManagerModule.qiXinShiJinDanXiangJiConfig;
		ui->pbtn_bagLength->setText(value);
		qiXinShiJinDanXiangJiConfig.setBagLength = value.toDouble();
	}
}

void QiXinShiJinDanXiangJi::pbtn_bagWidth_clicked()
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
		auto& qiXinShiJinDanXiangJiConfig = Modules::getInstance().configManagerModule.qiXinShiJinDanXiangJiConfig;
		ui->pbtn_bagWidth->setText(value);
		qiXinShiJinDanXiangJiConfig.setBagWidth = value.toDouble();
	}
}

void QiXinShiJinDanXiangJi::pbtn_resetProduct_clicked()
{
	auto& qiXinShiJinDanXiangJiConfig = Modules::getInstance().configManagerModule.qiXinShiJinDanXiangJiConfig;

	qiXinShiJinDanXiangJiConfig.totalProductionVolume = 0;
	qiXinShiJinDanXiangJiConfig.totalDefectiveVolume = 0;

	onUpdateStatisticalInfoUI();
}

bool QiXinShiJinDanXiangJi::check()
{
#pragma region check single instance
	if (!rw::rqw::RunEnvCheck::isSingleInstance("QiXinShiJinDanXiangJi.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "已经有程序在运行，请勿多次打开");
		return false;
	}
#pragma endregion

#pragma region check run env
	if (rw::rqw::RunEnvCheck::isProcessRunning("MVS.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "检测到海康威视软件正在运行，请先关闭后再启动本程序。");
		return false;
	}

	if (rw::rqw::RunEnvCheck::isProcessRunning("BasedCam3.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "检测到度申相机平台软件正在运行，请先关闭后再启动本程序。");
		return false;
	}
#pragma endregion

#pragma region check directory exist
	EnsureDirectoryExists(globalPath.projectHome);
	EnsureDirectoryExists(globalPath.configRootPath);
	EnsureDirectoryExists(globalPath.modelRootPath);
#pragma endregion

#pragma region check model exist
	if (!rw::rqw::RunEnvCheck::isFileExist(globalPath.modelPath))
	{
		QMessageBox::warning(nullptr, "错误", "模型文件缺失");
		return false;
	}
#pragma endregion

#pragma region check config format and exist
	rw::oso::StorageContext storageContext(rw::oso::StorageType::Xml);

	checkFileExistAndFormat<cdm::QiXinShiJinDanXiangJiConfig>(globalPath.qiXinShiJinDanXiangJiConfigPath, storageContext);
	checkFileExistAndFormat<cdm::SetConfig>(globalPath.setConfigPath, storageContext);
#pragma endregion

	return true;
}

bool QiXinShiJinDanXiangJi::EnsureDirectoryExists(const QString& dirPath)
{
	QDir dir(dirPath);
	if (!dir.exists()) {
		return dir.mkpath(".");
	}
	return true;
}

void QiXinShiJinDanXiangJi::setModelImage(const HalconCpp::HObject& img)
{
	auto sp = std::make_shared<HalconCpp::HObject>(img);
	std::shared_ptr<const HalconCpp::HObject> csp = sp;

	std::atomic_store_explicit(&modelImage, csp, std::memory_order_release);
	isModelImageLoaded.store(img.IsInitialized(), std::memory_order_relaxed);
}

std::shared_ptr<const HalconCpp::HObject> QiXinShiJinDanXiangJi::getModelImage()
{
	return std::atomic_load_explicit(&modelImage, std::memory_order_acquire);
}

void QiXinShiJinDanXiangJi::setIsModelImageLoaded(bool isLoaded)
{
	isModelImageLoaded.store(isLoaded, std::memory_order_relaxed);
}

bool QiXinShiJinDanXiangJi::getIsModelImageLoaded()
{
	return isModelImageLoaded.load(std::memory_order_relaxed);
}

void QiXinShiJinDanXiangJi::createButtonsOnWidget(QWidget* container)
{
	if (!container) return;

	if (auto* oldLayout = container->layout())
	{
		QLayoutItem* item = nullptr;
		while ((item = oldLayout->takeAt(0)) != nullptr)
		{
			if (auto* w = item->widget()) w->deleteLater();
			if (auto* childLayout = item->layout()) delete childLayout;
			delete item;
		}
		delete oldLayout;
	}

	auto* grid = new QGridLayout(container);
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setHorizontalSpacing(6);
	grid->setVerticalSpacing(6);

	constexpr int total = 20;
	constexpr int cols = 10;
	for (int i = 0; i < total; ++i)
	{
		auto* btn = new QPushButton(QString::number(i + 1), container);
		btn->setObjectName(QStringLiteral("btn_%1").arg(i + 1));
		auto sp = btn->sizePolicy();
		sp.setVerticalPolicy(QSizePolicy::Minimum);
		btn->setSizePolicy(sp);
		btn->setStyleSheet("QPushButton { background-color: rgb(0,170,0); }");
		grid->addWidget(btn, i / cols, i % cols);
	}

	container->setLayout(grid);
}

void QiXinShiJinDanXiangJi::cameraGetImageOnce()
{
	auto& camera = Modules::getInstance().cameraModule.camera1;
	if (camera)
	{
		camera->softwareTrigger();
	}
}

void QiXinShiJinDanXiangJi::updateDefectButtonsFromVector()
{
	// 若在工作线程被调用，切回到主线程再执行
	if (QThread::currentThread() != qApp->thread())
	{
		QMetaObject::invokeMethod(this, [this]() { updateDefectButtonsFromVector(); }, Qt::QueuedConnection);
		return;
	}

	// 线程安全地拷贝前20个标记
	std::array<bool, 20> flags{};
	{
		QMutexLocker locker(&ImageProcessor::isBadVectorMutex);
		const auto& vec = ImageProcessor::isBadVector;
		for (size_t i = 0; i < flags.size(); ++i)
			flags[i] = (i < vec.size()) ? vec[i] : false;
	}

	// 主线程更新 UI
	for (int i = 0; i < 20; ++i)
	{
		if (auto* btn = ui->widget_showBtn->findChild<QPushButton*>(QStringLiteral("btn_%1").arg(i + 1)))
		{
			btn->setStyleSheet(flags[i]
				? "QPushButton { background-color: rgb(220,0,0); }"   // true -> 红色
				: "QPushButton { background-color: rgb(0,170,0); }"   // false -> 绿色
			);
		}
	}
}
