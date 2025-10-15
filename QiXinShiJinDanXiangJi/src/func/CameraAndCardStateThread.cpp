#include "CameraAndCardStateThread.h"

#include "GlobalStruct.hpp"

size_t CameraAndCardStateThreadDuckTongue::runtimeCounts=0;

CameraAndCardStateThreadDuckTongue::CameraAndCardStateThreadDuckTongue(QObject* parent)
	: QThread(parent), running(false), _dlgProductSet(GlobalData::getInstance().setConfig){
}

CameraAndCardStateThreadDuckTongue::~CameraAndCardStateThreadDuckTongue()
{
	stopThread();
	wait();
}

void CameraAndCardStateThreadDuckTongue::startThread()
{
	running = true;
	if (!isRunning()) {
		start();
	}
}

void CameraAndCardStateThreadDuckTongue::stopThread()
{
	running = false;
}

void CameraAndCardStateThreadDuckTongue::run()
{
	while (running) {
		QThread::msleep(2000);
		if (_dlgProductSet.qiyongxiangjichonglian)
		{
			check_cameraState();
		}

		runtimeCounts++;
		if (runtimeCounts == 4) {
			runtimeCounts = 0;
		}
	}
}

void CameraAndCardStateThreadDuckTongue::check_cameraState()
{
	check_cameraState1();
	if (_dlgProductSet.qiyongerxiangji)
	{
		check_cameraState2();
	}
}

void CameraAndCardStateThreadDuckTongue::check_cameraState1()
{
	static bool isUpdateState = false;

	auto& globalThread = GlobalThread::getInstance();

	if (runtimeCounts != 0) {
		return;
	}
	if (globalThread.camera1) {
		if (globalThread.camera1->getConnectState()) {
			if (!isUpdateState) {
				emit updateCameraLabelState(1, true);
				isUpdateState = true;
			}
		}
		else {
			emit destroyCamera1();
			emit updateCameraLabelState(1, false);
		}
	}
	else {
		emit buildCamera1();
		emit updateCameraLabelState(1, false);
		isUpdateState = false;
	}
}

void CameraAndCardStateThreadDuckTongue::check_cameraState2()
{
	static bool isUpdateSate = false;

	auto& globalThread = GlobalThread::getInstance();

	if (runtimeCounts != 1) {
		return;
	}

	if (globalThread.camera2) {
		if (globalThread.camera2->getConnectState()) {
			if (!isUpdateSate) {
				emit updateCameraLabelState(2, true);
				isUpdateSate = true;
			}
		}
		else {
			emit destroyCamera2();
			emit updateCameraLabelState(2, false);
		}
	}
	else {
		emit buildCamera2();
		emit updateCameraLabelState(2, false);
		isUpdateSate = false;
	}
}