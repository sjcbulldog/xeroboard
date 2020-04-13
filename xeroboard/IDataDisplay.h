#pragma once

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>
#include <memory>

class IDataDisplay
{
public:
	IDataDisplay()
	{
	}

	virtual ~IDataDisplay()
	{
	}

	virtual void setValue(std::shared_ptr<nt::Value> value) = 0;
};
