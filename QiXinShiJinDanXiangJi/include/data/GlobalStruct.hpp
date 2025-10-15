#pragma once

#include <qobject.h>

#include "DetachDefectThread.h"
#include "dsl_Heap.hpp"
#include "QiXinShiJinDanXiangJi.h"
#include "QiXinShiJinDanXiangJi.hpp"
#include "SetConfig.hpp"
#include "ScoreConfig.hpp"

#include "oso_StorageContext.hpp"
#include "rqw_CameraObjectThread.hpp"
#include "rqw_ImageSaveEngine.h"
#include "ImageProcessorModule.hpp"
#include "CameraAndCardStateThread.h"

enum class RunningState
{
	Debug,
	OpenRemoveFunc,
	Stop
};

class GlobalThread
	:public QObject
{
	Q_OBJECT
public:
	static GlobalThread& getInstance();

	GlobalThread(const GlobalThread&) = delete;
	GlobalThread& operator=(const GlobalThread&) = delete;
private:
	GlobalThread();
	~GlobalThread() override;
public:
	std::unique_ptr<rw::rqw::CameraPassiveThread> camera1{ nullptr };
	QString cameraIp1{ "1" };
	void buildCamera();
	bool buildCamera1();

	void destroyCamera();
	void destroyCamera1();
public:
	// 图像处理模块
	std::unique_ptr<ImageProcessingModuleDuckTongue> modelCamera1 = nullptr;

	void buildImageProcessorModules(const QString& path);
	void destroyImageProcessingModule();
public:
	std::unique_ptr<rw::dsl::ThreadSafeHeap<float>> priorityQueue;

	void build_PriorityQueue();
	void destroy_PriorityQueue();
public:
	DetachDefectThreadDuckTongue* detachDefectThreadDuckTongue;

	void build_DetachDefectThreadDuckTongue();
	void destroy_DetachDefectThreadDuckTongue();
public:
	CameraAndCardStateThreadDuckTongue* cameraAndCardStateThreadDuckTongue;
public:
	void build_CameraAndCardStateThreadDuckTongue();
	void destory_CameraAndCardStateThreadDuckTongue();
signals:
	// 更新UI
	void emit_updateUiLabels(int index, bool isConnected);
public slots:
	// 相机重连
	void rebuild_Camera1();
	void destroy_Camera1();
public:
	bool isTargetCamera(const QString& cameraIndex, const QString& targetName);
	rw::rqw::CameraMetaData cameraMetaDataCheck(const QString& cameraIndex, const QVector<rw::rqw::CameraMetaData>& cameraInfo);

};

class GlobalFuncObject
	:public QObject
{
	Q_OBJECT
public:
	static GlobalFuncObject& getInstance();

	GlobalFuncObject(const GlobalFuncObject&) = delete;
	GlobalFuncObject& operator=(const GlobalFuncObject&) = delete;
private:
	GlobalFuncObject();
	~GlobalFuncObject() override;
public:
	std::unique_ptr<rw::oso::StorageContext> storeContext{ nullptr };
	void buildConfigManager(rw::oso::StorageType type);
	void saveQiXinShiJinDanXiangJiConfig();
	void saveScoreConfig();
	void saveSetConfig();
};

class GlobalData
	:public QObject
{
	Q_OBJECT
public:
	static GlobalData& getInstance()
	{
		static GlobalData instance;
		return instance;
	}

	GlobalData(const GlobalData&) = delete;
	GlobalData& operator=(const GlobalData&) = delete;
private:
	GlobalData();
	~GlobalData() override;
public:
	std::atomic<RunningState> runningState{ RunningState::OpenRemoveFunc };
public:
	// 统计信息
	struct StatisticalInfo
	{
		std::atomic_uint64_t produceCount{ 0 };
		std::atomic_uint64_t wasteCount{ 0 };
		std::atomic_uint64_t bagLength{ 0 };
		std::atomic_uint64_t bagWidth{ 0 };
	} statisticalInfo;

public:
	cdm::QiXinShiJinDanXiangJiConfig qiXinShiJinDanXiangJiConfig;
	cdm::ScoreConfig scoreConfig;
	cdm::SetConfig setConfig;
};