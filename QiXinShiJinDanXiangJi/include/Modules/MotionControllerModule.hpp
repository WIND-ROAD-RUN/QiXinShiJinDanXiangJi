#pragma once

#include <QObject>

#include"IModule.hpp"
#include"rqw_ZMotion.hpp"
#include"rqw_MonitorMotionIO.hpp"

class MotionControllerModule
	:public QObject, public IModule<bool>
{
	Q_OBJECT
public:
	MotionControllerModule();
	~MotionControllerModule() override;
public:
	bool build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	std::shared_ptr<rw::rqw::ZMotion> zmotion{ nullptr };
public slots:
	bool onBuildZMotion();
	void onDestroyZMotion();
};