#pragma once

#include <qobject.h>

#include "DetachDefectThread.h"
#include "dsl_Heap.hpp"
#include "oso_StorageContext.hpp"
#include "rqw_ImageSaveEngine.h"
#include "ImageProcessorModule.hpp"

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
	// 图像处理模块
	std::unique_ptr<ImageProcessingModule> modelCamera1 = nullptr;

	void buildImageProcessorModules(const QString& path);
	void destroyImageProcessingModule();
public:
	std::unique_ptr<rw::dsl::ThreadSafeHeap<bool>> priorityQueue;

	void build_PriorityQueue();
	void destroy_PriorityQueue();
public:
	DetachDefectThreadQiXinShiJin* detachDefectThreadQiXinShiJin;

	void build_DetachDefectThread();
	void destroy_DetachDefectThread();
signals:
	// 更新UI
	void emit_updateUiLabels(int index, bool isConnected);
public:
	bool isTargetCamera(const QString& cameraIndex, const QString& targetName);
	rw::rqw::CameraMetaData cameraMetaDataCheck(const QString& cameraIndex, const QVector<rw::rqw::CameraMetaData>& cameraInfo);
};