#pragma once

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>
#include <QObject>
#include <QString>

class SingleDataSource : public QObject
{
	Q_OBJECT;

public:
	SingleDataSource(const std::string& name);
	virtual ~SingleDataSource();

	std::shared_ptr<nt::Value> getValue() {
		return value_;
	}

signals:
	void valueChanged();

private:
	void emitValueChanged();
	void tableChangedEvent(const nt::EntryNotification& ev);
	void updateValue();

private:
	QString name_;
	NT_EntryListener listener_;
	std::shared_ptr<nt::Value> value_;
};

