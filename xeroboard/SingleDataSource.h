#pragma once

#include "DataSource.h"
#include <QString>
#include <cassert>

class SingleDataSource : public DataSource
{
	Q_OBJECT;

public:
	SingleDataSource(const std::string& name);
	virtual ~SingleDataSource();

	size_t count() const override;
	QString getString(size_t index) const override;
	bool getBoolean(size_t index) const override;
	double getDouble(size_t index) const override;
	DataSource::DataType getType(size_t index) const override;
	const QString& getName(size_t index) const override;
	void updateValue() override;

private:
	void tableChangedEvent(const nt::EntryNotification& ev);
	void valueChanged();

private:
	QString name_;
	NT_EntryListener listener_;
	NTValueWrapper value_;
};
