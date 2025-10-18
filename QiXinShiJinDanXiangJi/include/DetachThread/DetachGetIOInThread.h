#pragma once

#include <QThread>
#include <QObject>
#include <atomic>

class DetachGetIOInThread : public QThread
{
    Q_OBJECT
public:
    std::atomic_bool isProcessing{ false };
    std::atomic_bool isProcessFinish{ false };
public:
    explicit DetachGetIOInThread(QObject *parent = nullptr);

    ~DetachGetIOInThread() override;

    void startThread();

    void stopThread();

    void process();
protected:
    void run() override;
private:
    std::atomic<bool> running;
};

