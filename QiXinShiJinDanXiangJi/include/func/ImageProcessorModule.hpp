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
#include "rqw_ImageSaveEngine.h"


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

	// 存图
	void save_image(rw::rqw::ImageInfo& imageInfo, const QImage& image);
	void save_image_work(rw::rqw::ImageInfo& imageInfo, const QImage& image);

signals:
	void imageReady(QPixmap image);
	void imageNGReady(QPixmap image, size_t index, bool isbad);

private:
	std::unique_ptr<rw::imgPro::ImageProcess> _imgProcess;
public:
	// 构建模型引擎
	void buildSegModelEngine(const QString& enginePath);		// Segmentation 模型

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
private:
	// 判断是否有缺陷
	bool _isbad{ false };
	std::map<std::string, double> NgMap{};

private:
	QQueue<MatInfo>& _queue;
	QMutex& _mutex;
	QWaitCondition& _condition;
	int _workIndex;
public:
	int imageProcessingModuleIndex;
	int leftShieldWire{ -1 };
	int rightShieldWire{ -1 };
	int topShieldWire{ -1 };
	int bottomShieldWire{ -1 };
	double pixToWorld{ 0 };
	double tifeijuli{ 0 };

	int index1Num{ 0 };
	int index2Num{ 0 };
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
	void onFrameCaptured(cv::Mat frame, size_t index);

signals:
	void imageReady(QPixmap image);
	void imageNGReady(QPixmap image, size_t index, bool isbad);
	void shibiekuangChanged();
	void wenziChanged();
	void paramMapsChanged();

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


