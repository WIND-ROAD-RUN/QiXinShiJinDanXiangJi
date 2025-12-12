#include "EliminateModule.hpp"

bool EliminateModule::build()
{
	detachDefectThread = std::make_unique<DetachDefectThreadQiXinShiJin>(this);
	productPriorityQueue1 = std::make_unique<rw::dsl::ThreadSafeHeap<bool, rw::dsl::HeapType::MinHeap>>(50);
	return true;
}

void EliminateModule::destroy()
{
	if (detachDefectThread)
	{
		detachDefectThread.reset();
	}
	if (productPriorityQueue1)
	{
		productPriorityQueue1.reset();
	}
}

void EliminateModule::start()
{
	if (detachDefectThread)
	{
		detachDefectThread->startThread();
	}
}

void EliminateModule::stop()
{
	if (detachDefectThread)
	{
		detachDefectThread->stopThread();
		detachDefectThread->wait();
	}
}
