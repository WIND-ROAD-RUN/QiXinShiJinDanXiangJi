#include "DetachGetIOInThread.h"
#include "GlobalStruct.hpp"
#include "ImageProcessorModule.hpp"
#include "Modules.hpp"

DetachGetIOInThread::DetachGetIOInThread(QObject *parent) : QThread(parent)
{

}

DetachGetIOInThread::~DetachGetIOInThread()
{
    stopThread();
    wait(); // 等待线程安全退出
}

void DetachGetIOInThread::startThread()
{
    running = true;
    if (!isRunning()) {
        start(); // 启动线程
    }
}

void DetachGetIOInThread::stopThread()
{
    running = false; // 停止线程
}

void DetachGetIOInThread::process()
{
    auto& zmotion = Modules::getInstance().motionControllerModule.zmotion;

    bool status;
    if(zmotion->getConnectState())
    {
       status = zmotion->getIOOut(0);
    }

    if(1 == status)
    {
        auto& isBadVector = ImageProcessor::isBadVector;
        auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
        auto& jiange = setConfig.cipinguangdianjiange;
        {
            QMutexLocker locker(&ImageProcessor::isBadVectorMutex);
            isBadVector[jiange - 1] = true;
        }
    }

    auto result = zmotion->setIOOut(0,false);
}

void DetachGetIOInThread::run()
{
    while (running) {
        QThread::msleep(1);

        process();
    }
}
