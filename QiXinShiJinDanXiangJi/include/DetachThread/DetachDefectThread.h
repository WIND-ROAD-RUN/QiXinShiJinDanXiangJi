#pragma once

#include <atomic>
#include<QThread>

#include"rqw_LabelWarning.h"
#include"dsl_Heap.hpp"

#include"Utilty.hpp"


class DetachDefectThreadDuckTongue : public QThread
{
	Q_OBJECT
public:
	std::atomic_bool isProcessing{ false };
	std::atomic_bool isProcessFinish{ false };
public:
	explicit DetachDefectThreadDuckTongue(QObject* parent = nullptr);

	~DetachDefectThreadDuckTongue() override;

	void startThread();

	void stopThread();

	void processQueue(std::unique_ptr<rw::dsl::ThreadSafeHeap<float>>& queue);

protected:
	void run() override;
private:
	std::atomic<bool> running; // 使用原子变量保证线程安全
};
