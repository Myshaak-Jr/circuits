#pragma once

#include <vector>
#include <filesystem>

#include "types.h"
#include "pin.h"


namespace fs = std::filesystem;


class VoltageScope {
private:
	struct DataEntry {
		real_t time;
		real_t voltage;
	};

	std::vector<DataEntry> data;

	fs::path export_path;

	ConstPin a;
	ConstPin b;

public:
	VoltageScope(const ConstPin& a, const ConstPin& b, const fs::path& export_path);

	void record_voltage(real_t time);

	void export_data() const;
};

class CurrentScope {
private:
	struct DataEntry {
		real_t time;
		real_t current;
	};

	std::vector<DataEntry> data;

	fs::path export_path;

	ConstPin a;
	ConstPin b;

public:
	CurrentScope(const ConstPin& a, const ConstPin& b, const fs::path& export_path);

	void record_current(real_t time);

	void export_data() const;
};