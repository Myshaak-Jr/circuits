#pragma once

#include <filesystem>


constexpr int APP_VERSION_MAJOR = 0;
constexpr int APP_VERSION_MINOR = 1;
constexpr int APP_VERSION_PATCH = 0;

namespace fs = std::filesystem;

struct Settings {
	bool exit = false;
	int exit_code = 0;
	fs::path tables_path = fs::path("./tables/");
};

Settings handle_args(int argc, char *argv[]);