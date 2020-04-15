#include "NTValueWrapper.h"

NTValueWrapper::NTValueWrapper()
{
	value_ = nullptr;
}

NTValueWrapper::NTValueWrapper(std::shared_ptr<nt::Value> v)
{
	value_ = v;
}

NTValueWrapper::~NTValueWrapper()
{
}

NTValueWrapper& NTValueWrapper::operator=(const NTValueWrapper &v)
{
	value_ = v.value_;
	return *this;
}

NTValueWrapper& NTValueWrapper::operator=(std::shared_ptr<nt::Value> v)
{
	value_ = v;
	return *this;
}

QString NTValueWrapper::toString() const
{
	QString txt;

	switch (value_->type())
	{
	case NT_UNASSIGNED:
		break;
	case NT_BOOLEAN:
		txt = value_->GetBoolean() ? "TRUE" : "FALSE";
		break;
	case NT_DOUBLE:
		txt = QString::number(value_->GetDouble());
		break;
	case NT_STRING:
		txt = value_->GetString().str().c_str();
		break;
	case NT_RAW:
		break;
	case NT_BOOLEAN_ARRAY:
		txt = "[]";
		break;
	case NT_DOUBLE_ARRAY:
		txt = "[]";
		break;
	case NT_STRING_ARRAY:
		txt = "[]";
		break;
	case NT_RPC:
		break;
	}

	return txt;
}
