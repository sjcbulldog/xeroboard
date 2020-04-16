#include "AggregrateDataSource.h"

AggregrateDataSource::AggregrateDataSource()
{
}

AggregrateDataSource::~AggregrateDataSource()
{
}


size_t AggregrateDataSource::count() const
{
	size_t cnt = 0;
	for (auto v : sources_)
		cnt += v->count();

	return cnt;
}

QString AggregrateDataSource::getString(size_t index) const
{
	size_t which = 0;

	while (true) {
		if (index < sources_[which]->count())
			break;

		index -= sources_[which++]->count();
	}

	return sources_[which]->getString(index);
}

bool AggregrateDataSource::getBoolean(size_t index) const
{
	size_t which = 0;

	while (true) {
		if (index < sources_[which]->count())
			break;

		index -= sources_[which++]->count();
	}

	return sources_[which]->getBoolean(index);
}

double AggregrateDataSource::getDouble(size_t index) const
{
	size_t which = 0;

	while (true) {
		if (index < sources_[which]->count())
			break;

		index -= sources_[which++]->count();
	}

	return sources_[which]->getDouble(index);
}

DataSource::DataType AggregrateDataSource::getType(size_t index) const
{
	size_t which = 0;

	while (true) {
		if (index < sources_[which]->count())
			break;

		index -= sources_[which++]->count();
	}

	return sources_[which]->getType(index);
}

const QString& AggregrateDataSource::getName(size_t index) const
{
	size_t which = 0;

	while (true) {
		if (index < sources_[which]->count())
			break;

		index -= sources_[which++]->count();
	}

	return sources_[which]->getName(index);
}

void AggregrateDataSource::updateValue()
{
	for (auto v : sources_)
		v->updateValue();
}
