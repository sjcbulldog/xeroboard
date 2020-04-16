#pragma once
#include "DataSource.h"
class AggregrateDataSource : public DataSource
{
public:
	AggregrateDataSource();
	virtual ~AggregrateDataSource();

	void add(std::shared_ptr<DataSource> src) {
		sources_.push_back(src);
		emitValueChanged();
	}

	size_t count() const override;
	QString getString(size_t index) const override;
	bool getBoolean(size_t index) const override;
	double getDouble(size_t index) const override;
	DataType getType(size_t index) const override;
	const QString& getName(size_t index) const override;
	void updateValue() override;

private:
	std::vector<std::shared_ptr<DataSource>> sources_;
};

