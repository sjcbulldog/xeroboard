#include "SingleDataSource.h"

SingleDataSource::SingleDataSource(const std::string& name)
{
	name_ = name.c_str();
	updateValue();

	unsigned int flags = NT_NOTIFY_DELETE | NT_NOTIFY_UPDATE | NT_NOTIFY_FLAGS;
	nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
	std::function<void(const nt::EntryNotification & ev)> cb = std::bind(&SingleDataSource::tableChangedEvent, this, std::placeholders::_1);
	listener_ = inst.AddEntryListener(name, cb, flags);
}

SingleDataSource::~SingleDataSource()
{
	nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
	inst.RemoveEntryListener(listener_);
}

size_t SingleDataSource::count() const {
	size_t ret;

	if (value_.value()->IsBooleanArray()) {
		ret = value_.value()->GetBooleanArray().size();
	}
	else if (value_.value()->IsStringArray()) {
		ret = value_.value()->GetStringArray().size();
	}
	else if (value_.value()->IsDoubleArray()) {
		ret = value_.value()->GetDoubleArray().size();
	}
	else {
		ret = 1;
	}

	return ret;
}

QString SingleDataSource::getString(size_t index) const {
	QString ret;

	if (value_.value()->IsString()) {
		ret = value_.value()->GetString().data();
	}
	else if (value_.value()->IsStringArray()) {
		auto a = value_.value()->GetStringArray();
		if (index >= a.size()) {
			std::runtime_error err("invalid index for SingleDataSource value");
			throw err;
		}

		ret = a[index].data();
	}
	else {
		std::runtime_error err("request for value of wrong type");
		throw err;
	}

	return ret;
}

bool SingleDataSource::getBoolean(size_t index) const {
	bool ret;

	if (value_.value()->IsBoolean()) {
		ret = value_.value()->GetBoolean();
	}
	else if (value_.value()->IsBooleanArray()) {
		auto a = value_.value()->GetBooleanArray();
		if (index >= a.size()) {
			std::runtime_error err("invalid index for SingleDataSource value");
			throw err;
		}

		ret = a[index];
	}
	else {
		std::runtime_error err("request for value of wrong type");
		throw err;
	}

	return ret;
}

double SingleDataSource::getDouble(size_t index) const {
	double ret;

	if (value_.value()->IsDouble()) {
		ret = value_.value()->GetDouble();
	}
	else if (value_.value()->IsDoubleArray()) {
		auto a = value_.value()->GetDoubleArray();
		if (index >= a.size()) {
			std::runtime_error err("invalid index for SingleDataSource value");
			throw err;
		}

		ret = a[index];
	}
	else {
		std::runtime_error err("request for value of wrong type");
		throw err;
	}

	return ret;
}

const QString& SingleDataSource::getName(size_t index) const {
	assert(index == 0);
	return name_;
}

DataSource::DataType SingleDataSource::getType(size_t index) const {
	DataSource::DataType ret = DataSource::DataType::Unknown;

	if (value_.value()->IsBoolean() || value_.value()->IsBooleanArray()) {
		ret = DataSource::DataType::Boolean;
	}
	else if (value_.value()->IsString() || value_.value()->IsStringArray()) {
		ret = DataSource::DataType::String;
	}
	else if (value_.value()->IsDouble() || value_.value()->IsDoubleArray()) {
		ret = DataSource::DataType::Double;
	}

	return ret;
}
void SingleDataSource::updateValue()
{
	nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
	value_ = inst.GetEntry(name_.toStdString()).GetValue();
}

void SingleDataSource::tableChangedEvent(const nt::EntryNotification& ev)
{
	value_ = ev.value;
	emitValueChanged();
}
