#pragma once

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>
#include <QString>

class NTValueWrapper
{
public:
	NTValueWrapper();
	NTValueWrapper(std::shared_ptr<nt::Value> v);
	virtual ~NTValueWrapper();

	NTValueWrapper& operator=(const NTValueWrapper& v);
	NTValueWrapper& operator=(std::shared_ptr<nt::Value> v);

	bool isEmpty() const {
		return value_ == nullptr;
	}

	std::shared_ptr<nt::Value> value() const {
		return value_;
	}

	QString toString() const;

private:
	std::shared_ptr<nt::Value> value_;
};

