#include "SingleDataSource.h"

SingleDataSource::SingleDataSource(const std::string& name)
{
	name_ = name.c_str();

	unsigned int flags = NT_NOTIFY_DELETE | NT_NOTIFY_UPDATE | NT_NOTIFY_FLAGS;
	nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
	std::function<void(const nt::EntryNotification & ev)> cb = std::bind(&SingleDataSource::tableChangedEvent, this, std::placeholders::_1);
	listener_ = inst.AddEntryListener(name, cb, flags);
	updateValue();
}

SingleDataSource::~SingleDataSource()
{
	nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
	inst.RemoveEntryListener(listener_);
}

void SingleDataSource::emitValueChanged()
{
	emit valueChanged();
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