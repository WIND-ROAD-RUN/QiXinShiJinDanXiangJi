#include "MotionControllerModule.hpp"
#include "Utilty.hpp"

MotionControllerModule::MotionControllerModule()
{
}

MotionControllerModule::~MotionControllerModule()
{
}

bool MotionControllerModule::build()
{
	zmotion = std::make_shared<rw::rqw::ZMotion>(Utility::zmotionIp);
	zmotion->setIp(Utility::zmotionIp);
	bool isConnected = zmotion->connect();
	return isConnected;
}

void MotionControllerModule::destroy()
{
	if (zmotion)
	{
		bool isDisconnect = zmotion->disConnect();
		zmotion.reset();
	}
}

void MotionControllerModule::start()
{

}

void MotionControllerModule::stop()
{

}

bool MotionControllerModule::onBuildZMotion()
{
	zmotion = std::make_shared<rw::rqw::ZMotion>(Utility::zmotionIp);
	zmotion->setIp(Utility::zmotionIp);
	bool isConnected = zmotion->connect();
	return isConnected;
}

void MotionControllerModule::onDestroyZMotion()
{
	if (zmotion)
	{
		bool isDisconnect = zmotion->disConnect();
		zmotion.reset();
	}
}
