#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <string>

#include "circuit_matrix.h"
#include "node.h"
#include "pin.h"


class Part {
public:
	Part() = default;
	virtual ~Part() = default;

	virtual size_t pin_count() const noexcept = 0;

	virtual void set_node(size_t pin_id, Node* node) = 0;
	virtual Pin pin(size_t pin_id) = 0;
	virtual ConstPin pin(size_t pin_id) const = 0;

	virtual void stamp(CircuitMatrix& matrix) const = 0;
	virtual void reserve_additional_rows(CircuitMatrix& matrix) {}
	virtual void update(const CircuitMatrix& matrix) {};

	virtual const std::string& get_name() const = 0;
	virtual void set_name(const std::string& name) = 0;

	virtual real_t get_current_between(const ConstPin& a, const ConstPin& b) const = 0;
};
