#include "interpreter.h"

#include "circuit.h"
#include "parts/capacitor.h"
#include "parts/current_source.h"
#include "parts/inductor.h"
#include "parts/resistor.h"
#include "parts/switch.h"
#include "parts/voltage_source.h"
#include "util.h"
#include <iostream>
#include <sstream>

Interpreter::Interpreter(Circuit &circuit) : circuit(circuit) {}

void Interpreter::set_ground() {
	parts["GND"] = circuit.get_ground();
}

static bool is_first_word_letter(char x) {
	return (x == '_') || ('a' <= x && x <= 'z') || ('A' <= x && x <= 'Z');
}

static bool is_digit(char x) {
	return ('0' <= x && x <= '9');
}

static bool is_word_letter(char x) {
	return is_first_word_letter(x) || is_digit(x);
}

bool Interpreter::check_name(const std::string &name) {
	if (name.empty() || !is_first_word_letter(name[0])) return false;
	for (size_t i = 1; i < name.size(); ++i) {
		if (!is_word_letter(name[i])) return false;
	}
	return true;
}

Part *Interpreter::parse_part(const std::string &partname, size_t line_idx) const {
	if (!check_name(partname)) throw ParseError(std::format("Name error on line {}: Invalid part name '{}'.", line_idx, partname));

	auto it = parts.find(partname);
	if (it == parts.end()) throw ParseError(std::format("Name error on line {}: Unknown part name '{}'.", line_idx, partname));

	return it->second;
}

Pin Interpreter::parse_pin(const std::string &pinname, size_t line_idx, bool support_twopin, size_t twopin_part_pin_id) const {
	int dot_pos = -1;
	for (int i = 0; static_cast<int>(i) < pinname.size(); ++i) {
		if (pinname[i] == '.') dot_pos = i;
	}

	if (dot_pos == -1) {
		auto part = parse_part(pinname, line_idx);

		if (part->pin_count() == 1) {
			return part->pin(0);
		}
		else if (part->pin_count() == 2 && support_twopin) {
			return part->pin(twopin_part_pin_id);
		}
		else {
			throw ParseError(std::format("Name error on line {}: Invalid pin name '{}'.", line_idx, pinname));
		}
	}

	if (dot_pos == 0 || dot_pos == pinname.size() - 1) {
		throw ParseError(std::format("Name error on line {}: Invalid pin name '{}'.", line_idx, pinname));
	}

	std::string partname = pinname.substr(0, dot_pos);
	std::string pin = pinname.substr(dot_pos + 1, pinname.size() - (dot_pos + 1));

	if (!check_name(pin)) throw ParseError(std::format("Name error on line {}: Invalid pin name '{}'.", line_idx, pinname));

	auto part = parse_part(partname, line_idx);

	return part->pin(pin);
}


scalar Interpreter::parse_value(std::string_view value_string, const std::string &unit_name, size_t line_idx) {
	size_t i = 0;
	for (; i < value_string.size(); ++i) {
		if (!is_digit(value_string[i]) && value_string[i] != '.') break;
	}

	auto number_string = value_string.substr(0, i);

	scalar v;
	auto [ptr, ec] = std::from_chars(value_string.data(), value_string.data() + i, v);
	if (ec != std::errc()) {
		throw ParseError(std::format("Syntax error on line {}: Invalid number '{}'.", line_idx, number_string));
	}

	auto unit = value_string.substr(i);

	if (unit.size() < unit_name.size()) throw ParseError(std::format("Syntax error on line {}: Invalid unit ''.", line_idx));

	for (size_t j = 0; j < unit_name.size(); ++j) {
		if (unit[unit.size() - unit_name.size() + j] != unit_name[j])
			throw ParseError(std::format("Syntax error on line {}: Invalid unit '{}', expected {}.", line_idx, unit, unit_name));
	}

	auto mult = unit.substr(0, unit.size() - unit_name.size());

	if (mult == "E") v *= 1e18;
	else if (mult == "P") v *= 1e15;
	else if (mult == "T") v *= 1e12;
	else if (mult == "G") v *= 1e9;
	else if (mult == "M") v *= 1e6;
	else if (mult == "k") v *= 1e3;
	else if (mult == "");
	else if (mult == "m") v *= 1e-3;
	else if (mult == "u") v *= 1e-6;
	else if (mult == "n") v *= 1e-9;
	else if (mult == "p") v *= 1e-12;
	else if (mult == "f") v *= 1e-15;
	else if (mult == "a") v *= 1e-18;
	else throw ParseError(std::format("Syntax error on line {}: Invalid unit multiplier '{}'.", line_idx, mult));

	return v;
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
		else if (token == "voltage_source_2P") {
			add_basic_part<VoltageSource2Pin, true>(tokens, i, "voltage_source_2P", "V", line_idx);
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
					auto part = parse_part(std::string(tokens[i]), line_idx);
					if (part->pin_count() != 2) throw ParseError(std::format("Syntax error on line {}: Expected a 2-pin part after 'scope {} of', got '{}'", line_idx, scope_quantity, tokens[i]));

					if (is_current_scope) circuit.scope_current(part->pin(0), part->pin(1));
					else circuit.scope_voltage(part->pin(0), part->pin(1));
				}
				else if (scope_type == "between") {
					if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected pin name after 'scope {} between', got ''", line_idx, scope_quantity));
					auto pin_0 = parse_pin(std::string(tokens[i]), line_idx);
					std::string_view names_and_keyword = "";
					if (++i >= tokens.size() || (names_and_keyword = tokens[i]) != "and") throw ParseError(std::format("Syntax error on line {}: Expected 'and' after 'scope {} between {}', got '{}'", line_idx, scope_quantity, pin_0.name, names_and_keyword));
					if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected pin name after 'scope {} between {} and', got ''", line_idx, scope_quantity, pin_0.name, names_and_keyword));
					auto pin_1 = parse_pin(std::string(tokens[i]), line_idx);

					if (is_current_scope) circuit.scope_current(pin_0, pin_1);
					else circuit.scope_voltage(pin_0, pin_1);
				}
			}
			else {
				throw ParseError(std::format("Syntax error on line {}: Expected token 'current' or 'voltage' after 'scope', got '{}'", line_idx, scope_quantity));
			}
		}
		else if (token == "turn") {
			if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected token 'on' or 'off' after 'turn', got ''", line_idx));

			auto turn_to = tokens[i];

			bool is_on = turn_to == "on";
			bool is_off = turn_to == "off";

			if (is_on || is_off) {
				if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected a switch name after 'turn {}', got ''", line_idx, turn_to));

				std::string switch_name(tokens[i]);
				auto switch_part = dynamic_cast<Switch *>(parse_part(switch_name, line_idx));
				if (!switch_part) throw ParseError(std::format("Type error on line {}: {} is not a switch", line_idx, switch_name));

				std::string_view turn_at_keyword = "";
				if (++i >= tokens.size() || (turn_at_keyword = tokens[i]) != "at") throw ParseError(std::format("Syntax error on line {}: Expected token 'at' after 'turn {} {}', got '{}'", line_idx, turn_to, switch_name, turn_at_keyword));

				if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected a time value after 'turn {} {} at', got ''", line_idx, turn_to, switch_name));
				scalar t = parse_value(tokens[i], "s", line_idx);

				if (is_on) {
					switch_part->schedule_on(t / circuit.get_timestep());
				}
				else {
					switch_part->schedule_off(t / circuit.get_timestep());
				}
			}
			else {
				throw ParseError(std::format("Syntax error on line {}: Expected token 'on' or 'off' after 'turn', got '{}'", line_idx, turn_to));
			}
		}
		else {


			parse_connections(tokens, line_idx);
		}
	}
}

void Interpreter::parse_connections(const std::vector<std::string_view> &tokens, size_t line_idx) const {
	for (size_t i = 0; i < tokens.size(); ++i) {
		auto pin_0 = parse_pin(std::string(tokens[i]), line_idx, true, 1);

		if (++i >= tokens.size()) break;

		auto pin_connector = tokens[i];
		if (pin_connector != "-") throw ParseError(std::format("Syntax error on line {}: Expected '-' after '{}', got '{}'", line_idx, tokens[i - 1], pin_connector));

		if (++i >= tokens.size()) throw ParseError(std::format("Syntax error on line {}: Expected a pin name after '{} -', got ''", line_idx, tokens[i - 2]));

		auto pin_1 = parse_pin(std::string(tokens[i]), line_idx, true, 0);

		circuit.connect(pin_0, pin_1);

		--i;
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
