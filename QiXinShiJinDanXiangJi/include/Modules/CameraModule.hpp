#pragma once
#include <memory>
#include <QObject>

#include "ImageProcessorModule.hpp"
#include "rqw_CameraObjectThread.hpp"
#include"IModule.hpp"

enum BuildError
{
	Camera1Error = 1,
	Camera2Error = 2
};

class CameraModule
	: public QObject, public IModule<std::vector<BuildError>>
{
	Q_OBJECT
public:
	CameraModule();
	~CameraModule() override;

};