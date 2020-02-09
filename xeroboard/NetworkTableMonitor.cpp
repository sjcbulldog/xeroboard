#include "NetworkTableMonitor.h"
#include "NTEntryTracker.h"
#include <QDebug>
#include <optional>
#include <functional>
#include <sstream>
#include <iomanip>

const std::string NetworkTableMonitor::SmartDashboardTableName("/SmartDashboard");
const std::string NetworkTableMonitor::AllDashboardTableName("/");

NetworkTableMonitor::NetworkTableMonitor(const std::string &ipaddr)
{
	monitoring_ = AllDashboardTableName;

	ipaddr_ = ipaddr;
	inst_ = nt::NetworkTableInstance::GetDefault();
	inst_.StartClient(ipaddr_.c_str());

	unsigned int flags = NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_DELETE | NT_NOTIFY_UPDATE | NT_NOTIFY_FLAGS;
	std::function<void(const nt::EntryNotification & ev)> cb = std::bind(&NetworkTableMonitor::tableChangedEvent, this, std::placeholders::_1);
	inst_.AddEntryListener(monitoring_, cb, flags);

	std::string name = monitoring_;
	if (name.length() > 0 && name[0] == '/')
		name = name.substr(0, name.length() - 1);

	top_ = std::make_shared<NTEntryTracker>(name);

	dirty_ = false;
}

NetworkTableMonitor::NetworkTableMonitor() : NetworkTableMonitor("127.0.0.1")
{
}

NetworkTableMonitor::~NetworkTableMonitor()
{
}

std::string NetworkTableMonitor::toString(const nt::Value& v)
{
	std::string ret = "UnknownType";

	switch (v.type())
	{
	case NT_UNASSIGNED:
		ret = "NT_UNASSIGNED";
		break;
	case NT_BOOLEAN:
		ret = v.GetBoolean() ? "TRUE" : "FALSE";
		break;
	case NT_DOUBLE:
		ret = std::to_string(v.GetDouble());
		break;
	case NT_STRING:
		if (v.GetString().size() > 30)
			ret = std::string("'") + v.GetString().substr(0, 30).str() + "...'";
		else
			ret = "'" + v.GetString().str() + "'";
		break;
	case NT_RAW:
		ret = "NT_RAW";
		break;
	case NT_BOOLEAN_ARRAY:
		ret = std::string("[");
		for (size_t i = 0; i < v.GetBooleanArray().size(); i++)
		{
			if (ret.length() > 32)
			{
				ret += "...";
				break;
			}
			else
			{
				if (i != 0)
					ret += " ";
				ret += (v.GetBooleanArray()[i] ? "TRUE" : "FALSE");
			}
		}
		ret += "]";
		break;
	case NT_DOUBLE_ARRAY:
		ret = std::string("[");
		for (size_t i = 0; i < v.GetDoubleArray().size(); i++)
		{
			if (ret.length() > 32)
			{
				ret += "...";
				break;
			}
			else
			{
				if (i != 0)
					ret += " ";
				ret += std::to_string(v.GetDoubleArray()[i]);
			}
		}
		ret += "]";
		break;
	case NT_STRING_ARRAY:
		ret = std::string("[");
		for (size_t i = 0; i < v.GetStringArray().size(); i++)
		{
			if (ret.length() > 32)
			{
				ret += "...";
				break;
			}
			else
			{
				if (i != 0)
					ret += " ";
				ret += v.GetStringArray()[i];
			}
		}
		ret += "]";
		break;
	case NT_RPC:
		ret = "RPC";
		break;
	}

	return ret;
}

void NetworkTableMonitor::tableChangedEvent(const nt::EntryNotification &ev)
{
	std::lock_guard<std::mutex> guard(db_lock_);
	if (ev.flags & NT_NOTIFY_DELETE)
		deleteEntry(ev.name);
	else
		addEntry(ev.name, ev.value);
}

std::vector<std::string> NetworkTableMonitor::split(const std::string& str)
{
	std::vector<std::string> result;
	std::string word;
	size_t index = 0;

	while (index < str.length())
	{
		if (str[index] == '/')
		{
			if (index != 0)
			{
				result.push_back(word);
				word.clear();
			}
		}
		else
		{
			word += str[index];
		}
		index++;
	}

	if (word.length() > 0)
		result.push_back(word);

	return result;
}

void NetworkTableMonitor::addEntry(const std::string& name, const std::shared_ptr<nt::Value> value)
{
	std::vector<std::string> words = split(name);
	qDebug() << "Adding: " << name.c_str();

	if (words.size() == 0)
		return;

	if (words[words.size() - 1].length() > 0 && words[words.size() - 1][0] == '.')
		return;

	auto key = top_;
	for (size_t i = 0; i < words.size() - 1; i++)
		key = key->addChild(words[i]);

	(void)key->addChild(words[words.size() - 1], value);

	dirty_ = true;
}

void NetworkTableMonitor::deleteEntry(const std::string& name)
{
}

std::shared_ptr<NTEntryTracker> NetworkTableMonitor::getCopy()
{
	std::lock_guard<std::mutex> guard(db_lock_);
	dirty_ = false;
	return top_->copy();
}