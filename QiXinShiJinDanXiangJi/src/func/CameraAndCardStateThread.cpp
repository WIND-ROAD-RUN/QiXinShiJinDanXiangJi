#include "CameraAndCardStateThread.h"

#include "GlobalStruct.hpp"
#include "Modules.hpp"

size_t CameraAndCardStateThreadQiXinShiJin::runtimeCounts = 0;

CameraAndCardStateThreadQiXinShiJin::CameraAndCardStateThreadQiXinShiJin(QObject* parent)
	: QThread(parent), running(false), _dlgProductSet(Modules::getInstance().configManagerModule.setConfig) {
}

CameraAndCardStateThreadQiXinShiJin::~CameraAndCardStateThreadQiXinShiJin()
{
	stopThread();
	wait();
}

void CameraAndCardStateThreadQiXinShiJin::startThread()
{
	running = true;
	if (!isRunning()) {
		start();
	}
}

void CameraAndCardStateThreadQiXinShiJin::stopThread()
{
	running = false;
}

void CameraAndCardStateThreadQiXinShiJin::run()
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

void CameraAndCardStateThreadQiXinShiJin::check_cameraState()
{
	check_cameraState1();
}

void CameraAndCardStateThreadQiXinShiJin::check_cameraState1()
{
	auto& camera1 = Modules::getInstance().cameraModule.camera1;
	static bool isUpdateState = false;


	if (runtimeCounts != 0) {
		return;
	}
	if (camera1) {
		if (camera1->getConnectState()) {
			if (!isUpdateState) {
				emit updateCameraLabelState(1, true);
				isUpdateState = true;
			}
		}
		else {
			emit destroyCamera(1);
			emit updateCameraLabelState(1, false);
		}
	}
	else {
		emit buildCamera(1);
		emit startMonitor(1);
		emit updateCameraLabelState(1, false);
		isUpdateState = false;
	}
}

void CameraAndCardStateThreadQiXinShiJin::check_cardState()
{
	
}
