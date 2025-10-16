#include "CameraAndCardStateThread.h"

#include "GlobalStruct.hpp"

size_t CameraAndCardStateThreadDuckTongue::runtimeCounts = 0;

CameraAndCardStateThreadDuckTongue::CameraAndCardStateThreadDuckTongue(QObject* parent)
	: QThread(parent), running(false), _dlgProductSet(GlobalData::getInstance().setConfig) {
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

		check_cameraState();
		check_cardState();

		runtimeCounts++;
		if (runtimeCounts == 4) {
			runtimeCounts = 0;
		}
	}
}

void CameraAndCardStateThreadDuckTongue::check_cameraState()
{
	check_cameraState1();
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

void CameraAndCardStateThreadDuckTongue::check_cardState()
{
	static bool isUpdateState = false;

	auto& globalThread = GlobalThread::getInstance();

	if (runtimeCounts != 0) {
		return;
	}

	if (globalThread.zmotion) {
		if (globalThread.zmotion->getConnectState()) {
			if (!isUpdateState) {
				emit updateCameraLabelState(0, true);
				isUpdateState = true;
			}
		}
		else {
			emit destroyZMotion();
			emit updateCameraLabelState(0, false);
		}
	}
	else {
		emit buildZMotion();
		emit updateCameraLabelState(0, false);
		isUpdateState = false;
	}
}
