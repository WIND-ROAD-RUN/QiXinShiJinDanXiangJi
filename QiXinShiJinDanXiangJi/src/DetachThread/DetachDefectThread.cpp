#include "DetachDefectThread.h"
#include "GlobalStruct.hpp"
#include <chrono>

DetachDefectThreadDuckTongue::DetachDefectThreadDuckTongue(QObject* parent)
{

}

DetachDefectThreadDuckTongue::~DetachDefectThreadDuckTongue()
{
	stopThread();
	wait(); // 等待线程安全退出
}

void DetachDefectThreadDuckTongue::startThread()
{
	running = true;
	if (!isRunning()) {
		start(); // 启动线程
	}
}

void DetachDefectThreadDuckTongue::stopThread()
{
	running = false; // 停止线程
}

void DetachDefectThreadDuckTongue::processQueue(std::unique_ptr<rw::dsl::ThreadSafeHeap<float>>& queue)
{
#ifdef BUILD_WITHOUT_HARDWARE
	// 记录开始时间
	auto start = std::chrono::high_resolution_clock::now();
#endif

	auto& globalData = GlobalData::getInstance();
	auto& globalThread = GlobalThread::getInstance();
	try
	{

	}
	catch (const std::runtime_error&)
	{
		#ifdef BUILD_WITHOUT_HARDWARE
		// 记录结束时间并打印耗时（毫秒）
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "[processQueue] WasteTime: " << duration << " mm" << std::endl;
		#endif

		return;
	}
	#ifdef BUILD_WITHOUT_HARDWARE
	// 记录结束时间并打印耗时（毫秒）
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "[processQueue] WasteTime: " << duration << " mm" << std::endl;
	#endif
}

void DetachDefectThreadDuckTongue::run()
{
	auto& globalThread = GlobalThread::getInstance();
	auto& priorityQueue = globalThread.priorityQueue;

	while (running) {
		QThread::msleep(1);

		processQueue(priorityQueue);
	}
}
