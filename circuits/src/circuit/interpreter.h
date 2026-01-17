#pragma once

#include "circuit.h"
#include "part.h"
#include <format>
#include <istream>
#include <string>
#include <unordered_map>
#include <vector>


class ParseError : public std::runtime_error {
public:
	explicit ParseError(const std::string &message)
		: std::runtime_error(message) {}
};

class Interpreter {
private:
	Circuit &circuit;

	std::unordered_map<std::string, Part *> parts;

	static bool check_name(const std::string &name);

	static scalar parse_value(std::string_view value_string, const std::string &unit_name, size_t line_idx);
	Part *parse_part(const std::string &partname, size_t line_idx) const;
	Pin parse_pin(const std::string &pinname, size_t line_idx, bool support_twopin = false, size_t twopin_part_pin_id = 0) const;
	void parse_connections(const std::vector<std::string_view> &tokens, size_t line_idx) const;

	void execute_line(std::string_view line, size_t line_idx);

	template <class T, bool needs_value>
	void add_basic_part(const std::vector<std::string_view> &tokens, size_t &i, const std::string &part_type_name, const std::string &unit_name, size_t line_idx) {
		if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected part name after '{}', got ''", line_idx, part_type_name));
		std::string partname(tokens[i]);

		if constexpr (needs_value) {
			if (partname.back() != ':') throw ParseError(std::format("Syntax error on line {}: Expected ':' after '{} {}'", line_idx, part_type_name, partname));
			partname.pop_back();
		}

		if (!check_name(partname)) throw ParseError(std::format("Name error on line {}: Invalid part name '{}'.", line_idx, partname));
		if (parts.find(partname) != parts.end()) throw ParseError(std::format("Syntax error on line {}: Redefinition of part name '{}'.", line_idx, partname));

		Part *part;

		if constexpr (needs_value) {
			if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected value after '{} {}:', got ''", line_idx, part_type_name, partname));
			scalar value = parse_value(tokens[i], unit_name, line_idx);

			part = circuit.add_part<T>(partname, value);
		}
		else {
			part = circuit.add_part<T>(partname);
		}

		parts[partname] = part;
	}

public:
	Interpreter(Circuit &circuit);

	void set_ground();

	void execute(std::istream &in);
	void execute(const std::string &script);
};