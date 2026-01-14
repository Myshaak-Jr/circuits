#pragma once

#include "part.h"
#include <istream>
#include <string>
#include <unordered_map>
#include <vector>


class ParseError : public std::runtime_error {
public:
	explicit ParseError(const std::string &message)
		: std::runtime_error(message) {}
};

class Circuit;

class Interpreter {
private:
	Circuit &circuit;

	std::unordered_map<std::string, Part *> parts;

	static bool check_name(const std::string &name);

	static scalar parse_value(std::string_view name, const std::string &unit_name);

	Part *get_part(const std::string &partname, size_t line_idx) const;
	ConstPin get_pin(const std::string &pinname, size_t line_idx) const;

	void execute_line(std::string_view line, size_t line_idx);

	template <class T, bool needs_value>
	void add_basic_part(const std::vector<std::string_view> &tokens, size_t &i, const std::string &part_type_name, const std::string &unit_name, size_t line_idx) {
		if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected part name after '{}', got ''", part_type_name, line_idx));
		std::string partname(tokens[i]);

		if constexpr (needs_value) {
			if (partname.back() != ':') throw ParseError(std::format("Syntax error on line {}: Expected ':' after '{} {}'", part_type_name, line_idx, partname));
			partname.pop_back();
		}

		if (check_name(partname)) throw ParseError(std::format("Name error on line {}: Invalid part name '{}'.", line_idx, partname));
		if (parts.find(partname) != parts.end()) throw ParseError(std::format("Syntax error on line {}: Redefinition of part name '{}'.", line_idx, partname));

		Part *part;

		if constexpr (needs_value) {
			if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected value after '{} {}:', got ''", part_type_name, line_idx, partname));
			scalar value = parse_value(tokens[i], unit_name);

			part = circuit.add_part<T>(partname, value);
		}
		else {
			part = circuit.add_part<T>(partname);
		}

		parts[partname] = part;
	}

public:
	Interpreter(Circuit &circuit);

	void execute(std::istream &in);
	void execute(const std::string &script);
};