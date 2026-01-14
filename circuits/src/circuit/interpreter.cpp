#include "interpreter.h"

#include "circuit.h"
#include "parts/capacitor.h"
#include "parts/current_source.h"
#include "parts/inductor.h"
#include "parts/resistor.h"
#include "parts/switch.h"
#include "parts/voltage_source.h"
#include "util.h"
#include <format>
#include <iostream>
#include <sstream>

Interpreter::Interpreter(Circuit &circuit) : circuit(circuit) {}


static bool is_first_word_letter(char x) {
	return (x == '_') || ('a' <= x && x <= 'z') || ('A' <= x && x <= 'Z');
}

static bool is_word_letter(char x) {
	return is_first_word_letter(x) || ('0' <= x && x <= '9');
}

bool Interpreter::check_name(const std::string &name) {
	if (name.empty() || !is_first_word_letter(name[0])) return false;
	for (size_t i = 1; i < name.size(); ++i) {
		if (!is_word_letter(name[i])) return false;
	}
	return true;
}

Part *Interpreter::get_part(const std::string &partname, size_t line_idx) const {
	if (!check_name(partname)) throw ParseError(std::format("Name error on line {}: Invalid part name '{}'.", line_idx, partname));

	auto it = parts.find(partname);
	if (it == parts.end()) throw ParseError(std::format("Name error on line {}: Unknown part name '{}'.", line_idx, partname));

	return it->second;
}

ConstPin Interpreter::get_pin(const std::string &pinname, size_t line_idx) const {
	int dot_pos = -1;
	for (int i = 0; static_cast<int>(i) < pinname.size(); ++i) {
		if (pinname[i] == '.') dot_pos = i;
	}

	if (dot_pos == -1) {
		auto part = get_part(pinname, line_idx);

		if (part->pin_count() != 1) throw ParseError(std::format("Name error on line {}: Invalid pin name '{}'.", line_idx, pinname));

		return part->pin(0);
	}

	if (dot_pos == 0 || dot_pos == pinname.size() - 1) {
		throw ParseError(std::format("Name error on line {}: Invalid pin name '{}'.", line_idx, pinname));
	}

	std::string partname = pinname.substr(0, dot_pos);
	std::string pin = pinname.substr(dot_pos + 1, pinname.size() - (dot_pos + 1));

	if (!check_name(pin)) throw ParseError(std::format("Name error on line {}: Invalid pin name '{}'.", line_idx, pinname));

	auto part = get_part(partname, line_idx);

	return part->pin(pin);
}





void Interpreter::execute_line(std::string_view line, size_t line_idx) {
	std::vector<std::string_view> tokens{};

	size_t token_lo = 0;
	size_t token_hi = 0;

	while (token_hi <= line.size()) {
		if (token_hi == line.size() || line[token_hi] == ' ' || line[token_hi] == '\t') {
			if (token_lo != token_hi) {
				auto token = line.substr(token_lo, token_hi - token_lo);

				if (token == "//") break;

				tokens.push_back(token);
			}
			token_lo = token_hi + 1;
		}

		++token_hi;
	}


	for (size_t i = 0; i < tokens.size(); ++i) {
		auto token = tokens[i];

		if (token == "capacitor") {
			add_basic_part<Capacitor, true>(tokens, i, "capacitor", "F", line_idx);
		}
		else if (token == "current_source") {
			add_basic_part<CurrentSource, true>(tokens, i, "current_source", "Am", line_idx);
		}
		else if (token == "inductor") {
			add_basic_part<Inductor, true>(tokens, i, "inductor", "H", line_idx);
		}
		else if (token == "resistor") {
			add_basic_part<Resistor, true>(tokens, i, "resistor", "Ohm", line_idx);
		}
		else if (token == "switch") {
			add_basic_part<Switch, false>(tokens, i, "switch", "", line_idx);
		}
		else if (token == "voltage_source") {
			add_basic_part<VoltageSource, true>(tokens, i, "voltage_source", "V", line_idx);
		}
		else if (token == "scope") {
			if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected token 'current' or 'voltage' after 'scope', got ''", line_idx));

			auto scope_quantity = tokens[i];

			bool is_current_scope = scope_quantity == "current";
			bool is_voltage_scope = scope_quantity == "voltage";

			if (is_current_scope || is_voltage_scope) {
				if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected token 'of' or 'between' after 'scope {}', got ''", line_idx, scope_quantity));

				auto scope_type = tokens[i];

				if (scope_type != "of" && scope_type != "between") throw ParseError(std::format("Syntax error on line {}: Expected token 'of' or 'between' after 'scope {}', got '{}'", line_idx, scope_quantity, scope_type));

				if (scope_type == "of") {
					if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected part name after 'scope {} of', got ''", line_idx, scope_quantity));
					auto part = get_part(std::string(tokens[i]), line_idx);
					if (part->pin_count() == 2) throw ParseError(std::format("Syntax error on line {}: Expected a 2-pin part after 'scope {} of', got '{}'", line_idx, scope_quantity, tokens[i]));

					if (is_current_scope) circuit.scope_current(part->pin(0), part->pin(1));
					else circuit.scope_voltage(part->pin(0), part->pin(1));
				}
				else if (scope_type == "between") {
					if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected pin name after 'scope {} between', got ''", line_idx, scope_quantity));
					auto pin_0 = get_pin(std::string(tokens[i]), line_idx);
					std::string_view names_and = "";
					if (++i >= tokens.size() || (names_and = tokens[i]) != "and") throw ParseError(std::format("Syntax error on line {}: Expected 'and' after 'scope {} between {}', got '{}'", line_idx, scope_quantity, pin_0.name, names_and));
					if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected pin name after 'scope {} between {} and', got ''", line_idx, scope_quantity, pin_0.name, names_and));
					auto pin_1 = get_pin(std::string(tokens[i]), line_idx);

					if (is_current_scope) circuit.scope_current(pin_0, pin_1);
					else circuit.scope_voltage(pin_0, pin_1);
				}
			}
			else {
				throw ParseError(std::format("Syntax error on line {}: Expected token 'current' or 'voltage' after 'scope', got '{}'", line_idx, scope_quantity));
			}
		}
		else if (token == "turn") {
			// TODO: switch scheduling
		}
		else {
			// TODO: connections
		}
	}
}


void Interpreter::execute(std::istream &in) {
	std::string line;
	size_t i = 0;

	while (std::getline(in, line)) {
		if (line.empty()) { ++i; continue; }
		execute_line(line, i);
		++i;
	}
}

void Interpreter::execute(const std::string &script) {
	std::istringstream iss(script);
	execute(iss);
}
