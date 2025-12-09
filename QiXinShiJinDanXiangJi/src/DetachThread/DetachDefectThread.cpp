#include "DetachDefectThread.h"
#include "GlobalStruct.hpp"
#include <chrono>

#include "Modules.hpp"

DetachDefectThreadQiXinShiJin::DetachDefectThreadQiXinShiJin(QObject* parent)
{

}

DetachDefectThreadQiXinShiJin::~DetachDefectThreadQiXinShiJin()
{
	stopThread();
	wait(); // 等待线程安全退出
}

void DetachDefectThreadQiXinShiJin::startThread()
{
	running = true;
	if (!isRunning()) {
		start(); // 启动线程
	}
}

void DetachDefectThreadQiXinShiJin::stopThread()
{
	running = false; // 停止线程
}

void DetachDefectThreadQiXinShiJin::processQueue(std::unique_ptr<rw::dsl::ThreadSafeHeap<bool>>& queue)
{
#ifdef BUILD_WITHOUT_HARDWARE
	// 记录开始时间
	auto start = std::chrono::high_resolution_clock::now();
#endif

	try
	{
		auto isBad = false;
		queue->tryGetTop(isBad);

		if (isBad)
		{
			auto& setConfig = GlobalData::getInstance().setConfig;
			auto& camera = Modules::getInstance().cameraModule.camera1;
			queue->tryPopTop(isBad);
			QThread::msleep(setConfig.tifeiyanshi);
			// 剔废动作
			rw::rqw::OutTriggerConfig outTriggerConfig;
			outTriggerConfig.lineSelector = 2;
			outTriggerConfig.lineMode = 8;
			outTriggerConfig.lineSource = 5;
			outTriggerConfig.durationValue = setConfig.tifeiyanshi * 1000;
			outTriggerConfig.strobeEnable = true;
			camera->setOutTriggerConfig(outTriggerConfig);
			camera->outTrigger();
		}
	}
	catch (const std::runtime_error&)
	{
		
	}
}

void DetachDefectThreadQiXinShiJin::run()
{
	auto& globalThread = GlobalThread::getInstance();
	auto& priorityQueue = globalThread.priorityQueue;

	while (running) {
		QThread::msleep(1);

		processQueue(priorityQueue);
	}
}
