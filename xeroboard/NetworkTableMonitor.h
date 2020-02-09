#pragma once

#include "NTEntryTracker.h"
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>
#include <thread>
#include <string>
#include <mutex>
#include <vector>

class NetworkTableMonitor
{
public:
	NetworkTableMonitor(const std::string &ipaddr);
	NetworkTableMonitor();
	virtual ~NetworkTableMonitor();

	std::shared_ptr<NTEntryTracker> getCopy() ;
	bool isDirty() { return dirty_; }

	static std::string toString(const nt::Value &v);

private:
	void tableChangedEvent(const nt::EntryNotification& ev);
	void addEntry(const std::string& name, const std::shared_ptr<nt::Value> value);
	void deleteEntry(const std::string& name);

	std::vector<std::string> split(const std::string& name);

	static const std::string SmartDashboardTableName;
	static const std::string AllDashboardTableName;

private:
	std::string monitoring_;
	std::thread thread_;
	std::string ipaddr_;
	nt::NetworkTableInstance inst_;
	std::mutex db_lock_;
	std::shared_ptr<NTEntryTracker> top_;
	bool dirty_;
};
