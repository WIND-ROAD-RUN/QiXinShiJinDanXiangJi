#pragma once
#include <QThread>
#include <QDebug>
#include <atomic>
#include "SetConfig.hpp"

class CameraAndCardStateThreadQiXinShiJin : public QThread
{
	Q_OBJECT
private:
	static size_t runtimeCounts;

public:
	explicit CameraAndCardStateThreadQiXinShiJin(QObject* parent = nullptr);

	~CameraAndCardStateThreadQiXinShiJin() override;

	void startThread();

	void stopThread();

	cdm::SetConfig& _dlgProductSet;

protected:
	void run() override;
private:
	void check_cameraState();
	void check_cameraState1();

	void check_cardState();
signals:
	void updateCameraLabelState(int cameraIndex, bool state);
	void updateCardLabelState(bool state);
	void addWarningInfo(QString message, bool updateTimestampIfSame, int redDuration);

signals:
	void buildCamera(int index);

	void destroyCamera(int index);

	void startMonitor(int index);

	void buildZMotion();
	void destroyZMotion();
private:
	std::atomic<bool> running; // 使用原子变量保证线程安全
};