#pragma once

#include"ime_ModelEngineFactory.h"

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <opencv2/opencv.hpp>
#include <vector>
#include <QThread>
#include <QPixmap>
#include <imgPro_ImageProcess.hpp>

#include "rqw_CameraObjectCore.hpp"


// 图片信息
struct MatInfo {
	cv::Mat image;	// 图片内容
	float location;	// 记录拍照瞬间的时间点
	size_t index;	// 拍照的相机的下标
};


class ImageProcessorDuckTongue : public QThread
{
	Q_OBJECT

public:
	ImageProcessorDuckTongue(QQueue<MatInfo>& queue,
		QMutex& mutex,
		QWaitCondition& condition,
		int workIndex,
		QObject* parent = nullptr);

protected:
	void run() override;

private:
	void run_debug(MatInfo& frame);				// 不开剔废时候的调试模式
private:
	void run_OpenRemoveFunc(MatInfo& frame);	// 开启剔废功能时的处理模式

	void run_OpenRemoveFunc_emitErrorInfo(bool isbad) const;
signals:
	void imageReady(QPixmap image);
	void imageNGReady(QPixmap image, size_t index, bool isbad);
	void updateMainWindowShowBtn();
private:
	std::unique_ptr<rw::imgPro::ImageProcess> _imgProcess;
public:
	// 构建模型引擎
	void buildDetModelEngine(const QString& enginePath);

	void iniIndexGetContext();
	void iniEliminationInfoFunc();
	void iniEliminationInfoGetContext();
	void iniDefectResultInfoFunc();
	void iniDefectResultGetContext();
	void iniDefectDrawConfig();
	void iniRunTextConfig();


public:
	// 在指定位置画横线
	void drawBoundariesLines(QImage& image);
	// 更新屏蔽线
	void updateShieldWires();

public slots:
	void updateDrawRec();
	void updateDrawText();
	void updateParamMapsFromGlobalStruct();
public:
	// 判断是否有缺陷
	static QMutex isBadVectorMutex;
	static std::vector<bool> isBadVector;
	void initial_isBadVector();
private:
	std::map<std::string, double> BadMap{};
	std::map<std::string, double> FengKouMap{};
	std::map<std::string, double> JiaoDaiMap{};

private:
	QQueue<MatInfo>& _queue;
	QMutex& _mutex;
	QWaitCondition& _condition;
	int _workIndex;
public:
	int imageProcessingModuleIndex;
	int topShieldWire{ -1 };
	int bottomShieldWire{ -1 };
};


class ImageProcessingModuleDuckTongue : public QObject {
	Q_OBJECT
public:
	QString modelEnginePath;

public:
	// 初始化图像处理模块
	void BuildModule();
public:
	ImageProcessingModuleDuckTongue(int numConsumers, QObject* parent = nullptr);

	~ImageProcessingModuleDuckTongue();

public slots:
	// 相机回调函数
	void onFrameCaptured(rw::rqw::MatInfo matInfo, size_t index);

signals:
	void imageReady(QPixmap image);
	void imageNGReady(QPixmap image, size_t index, bool isbad);
	void shibiekuangChanged();
	void wenziChanged();
	void paramMapsChanged();
	void updateMainWindowShowBtn();
public:
	std::vector<ImageProcessorDuckTongue*> getProcessors() const {
		return _processors;
	}

private:
	QQueue<MatInfo> _queue;
	QMutex _mutex;
	QWaitCondition _condition;
	std::vector<ImageProcessorDuckTongue*> _processors;
	int _numConsumers;
public:
	size_t index;
};


