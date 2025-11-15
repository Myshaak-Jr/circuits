#pragma once

#include <vector>

#include "types.h"


class Node {
private:
	real_t voltage;
	size_t node_id;
	bool ground;

public:
	constexpr Node() : voltage(0.0), node_id(0), ground(false) {}
	constexpr ~Node() = default;

	constexpr void set_voltage(real_t voltage) { this->voltage = voltage; }
	constexpr real_t get_voltage() const { return voltage; }

	constexpr void set_row_id(size_t node_id) { this->node_id = node_id; }
	constexpr size_t get_node_id() const { return node_id; }

	constexpr void set_ground() { ground = true; }
	constexpr void reset_ground() { ground = false; }
	constexpr bool is_ground() const { return ground; }
};
