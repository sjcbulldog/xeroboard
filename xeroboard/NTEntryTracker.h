#pragma once
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>
#include <string>
#include <memory>
#include <map>

class NTEntryTracker
{
public:
	NTEntryTracker(const std::string& name, const std::shared_ptr<nt::Value> value) : value_(value) {
		name_ = name;
		parent_ = nullptr;
		subtable_ = false;
	}

	NTEntryTracker(const std::string& name) {
		name_ = name;
		parent_ = nullptr;
		subtable_ = true;
	}

	const std::string& getName() const {
		return name_;
	}

	std::shared_ptr<nt::Value> getValue() {
		return value_;
	}

	bool isSubTable() const {
		return subtable_;
	}

	std::shared_ptr<NTEntryTracker> addChild(const std::string& name) {
		auto child = getChild(name);
		if (child == nullptr)
		{
			child = std::make_shared<NTEntryTracker>(name);
			child->parent_ = this;
			children_[name] = child;
		}

		return child;
	}

	std::shared_ptr<NTEntryTracker> addChild(const std::string& name, const std::shared_ptr<nt::Value> value) {
		auto child = getChild(name);
		if (child == nullptr)
		{
			child = std::make_shared<NTEntryTracker>(name, value);
			child->parent_ = this;
			children_[name] = child;
		}
		else
		{
			// Just in case the type changed since it was entered
			child->value_ = value;
		}

		return child;
	}

	std::shared_ptr<NTEntryTracker> getChild(const std::string& name) {
		auto it = children_.find(name);
		if (it == children_.end())
			return nullptr;

		return it->second;
	}

	std::shared_ptr<NTEntryTracker> copy() const {
		std::shared_ptr<NTEntryTracker> ret;

		if (isSubTable())
		{
			ret = std::make_shared<NTEntryTracker>(name_);
			for (auto entry : children_)
				ret->children_[entry.second->getName()] = entry.second->copy();
		}
		else
		{
			ret = std::make_shared<NTEntryTracker>(name_, value_);
		}

		return ret;
	}

	std::map<std::string, std::shared_ptr<NTEntryTracker>>& getChildMap() {
		return children_;
	}

private:
	NTEntryTracker* parent_;
	std::string name_;
	std::map<std::string, std::shared_ptr<NTEntryTracker>> children_;
	bool subtable_;
	std::shared_ptr<nt::Value> value_;
};

