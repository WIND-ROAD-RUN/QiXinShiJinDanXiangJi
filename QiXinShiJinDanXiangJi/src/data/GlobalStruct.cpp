#include "GlobalStruct.hpp"

#include "QiXinShiJinDanXiangJi.h"
#include "Utilty.hpp"
#include <QRegularExpression>

GlobalThread& GlobalThread::getInstance()
{
	static GlobalThread instance;
	return instance;
}

GlobalThread::GlobalThread()
{
}

GlobalThread::~GlobalThread()
{
}

void GlobalThread::buildImageProcessorModules(const QString& path)
{
	modelCamera1 = std::make_unique<ImageProcessingModule>(1);

	modelCamera1->modelEnginePath = path;

	modelCamera1->index = 1;

	modelCamera1->BuildModule();
}

void GlobalThread::destroyImageProcessingModule()
{
	modelCamera1.reset();
}

void GlobalThread::build_PriorityQueue()
{
	auto compareNodeEqual = [](const float& a, const float& b) {
		return a == b;
		};
	auto compareNodePriority = [](const float& a, const float& b) {
		return a < b;
		};

	priorityQueue = std::make_unique<rw::dsl::ThreadSafeHeap<bool>>();
}

void GlobalThread::destroy_PriorityQueue()
{
	priorityQueue.reset();
}

void GlobalThread::build_DetachDefectThread()
{
	detachDefectThreadQiXinShiJin = new DetachDefectThreadQiXinShiJin(this);
}

void GlobalThread::destroy_DetachDefectThread()
{
	if (detachDefectThreadQiXinShiJin)
	{
		delete detachDefectThreadQiXinShiJin;
		detachDefectThreadQiXinShiJin = nullptr;
	}
}

bool GlobalThread::isTargetCamera(const QString& cameraIndex, const QString& targetName)
{
	QRegularExpression regex(R"((\d+)\.(\d+)\.(\d+)\.(\d+))");
	QRegularExpressionMatch match = regex.match(targetName);

	if (match.hasMatch()) {
		auto matchString = match.captured(3);

		return cameraIndex == matchString;
	}

	return false;
}

rw::rqw::CameraMetaData GlobalThread::cameraMetaDataCheck(const QString& cameraIndex,
	const QVector<rw::rqw::CameraMetaData>& cameraInfo)
{
	for (const auto& cameraMetaData : cameraInfo) {
		if (isTargetCamera(cameraIndex, cameraMetaData.ip)) {
			return cameraMetaData;
		}
	}
	rw::rqw::CameraMetaData error;
	error.ip = "0";
	return error;
}
