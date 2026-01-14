#include "settings.h"

#include <filesystem>
#include <iostream>
#include <string>



static void print_help() {
	std::cout
		<< "SimLogue: An Analogue Circiut Simulator\n"
		<< "Version: " << APP_VERSION_MAJOR << "."
		<< APP_VERSION_MINOR << "."
		<< APP_VERSION_PATCH << "\n\n"

		<< "Usage:\n"
		<< "  simlogue [options] circuit_file\n\n"

		<< "Options:\n"
		<< "  -t, --tables     <path>   Path to generated CSV tables\n"
		<< "                            (default: ./tables/)\n"
		<< "  -v, --version             Show version information\n"
		<< "  -h, --help			    Show this help message\n"
		<< "  -r, --samplerate <freq>   Sets the samplerate in Hz\n"
		<< "                            (default: 44100)\n";
}

static void print_version() {
	std::cout << "Version: " << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR << "." << APP_VERSION_PATCH << "\n";
}

Settings handle_args(int argc, char *argv[]) {
	Settings settings;

	bool do_print_help = false;
	bool do_print_version = false;

	bool accept_options = true;

	for (int i = 1; i < argc; ++i) {
		std::string option(argv[i]);

		if (accept_options && (option == "-h" || option == "--help")) {
			do_print_help = true;
		}
		else if (accept_options && (option == "-v" || option == "--version")) {
			do_print_version = true;
		}
		else if (accept_options && (option == "-t" || option == "--tables")) {
			if (++i >= argc) {
				std::cout << "Option " << option << " requires <path> argument.\nSee help:\n\n";
				print_help();
				return Settings{ .exit = true, .exit_code = 2 };
			}
			std::string argument = argv[i];
			settings.tables_path = fs::path(argument);
		}
		else if (accept_options && (option == "-r" || option == "--samplerate")) {
			if (++i >= argc) {
				std::cout << "Option " << option << " requires <freq> argument.\nSee help:\n\n";
				print_help();
				return Settings{ .exit = true, .exit_code = 2 };
			}
			try {
				settings.samplerate = std::stof(argv[i]);
			}
			catch (const std::exception &) {
				std::cout << "Argument <freq> must be a floating point number in valid range.\nSee help:\n\n";
				print_help();
				return Settings{ .exit = true, .exit_code = 2 };
			}
			if (settings.samplerate <= 0.0) {
				std::cout << "Argument <freq> must be positive.\n";
				print_help();
				return Settings{ .exit = true, .exit_code = 2 };
			}
		}
		else {
			if (settings.circuit_path == "") {
				settings.circuit_path = option;
			}
			else {
				std::cout << "SimLogue accepts just one positional argument.\nSee help:\n\n";
				print_help();
				return Settings{ .exit = true, .exit_code = 2 };
			}
		}
	}

	if (settings.circuit_path == "") {
		std::cout << "SimLogue requires the circuit file path.\nSee help:\n\n";
		print_help();
		return Settings{ .exit = true, .exit_code = 2 };
	}

	if (do_print_help) {
		print_help();
		return Settings{ .exit = true, .exit_code = 0 };
	}
	if (do_print_version) {
		print_version();
		return Settings{ .exit = true, .exit_code = 0 };
	}

	return settings;
}