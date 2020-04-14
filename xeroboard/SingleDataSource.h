#pragma once

#include "NTValueWrapper.h"
#include <QObject>
#include <QString>

class SingleDataSource : public QObject
{
	Q_OBJECT;

public:
	SingleDataSource(const std::string& name);
	virtual ~SingleDataSource();

	const NTValueWrapper & value() {
		return value_;
	}

	const QString& name() {
		return name_;
	}

	bool isSubtable() {
		return value_.isEmpty();
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
	NTValueWrapper value_;
};

