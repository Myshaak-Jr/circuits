#pragma once

#include <cstddef>
#include <functional>
#include <memory>

#include "circuit_matrix.h"
#include "node.h"
#include "pin.h"


class Part {
public:
	Part() = default;
	virtual ~Part() = default;

	virtual std::size_t pin_count() const noexcept = 0;

	virtual void set_node(std::size_t i, Node* node) = 0;
	virtual Pin pin(std::size_t i) = 0;
	virtual ConstPin pin(std::size_t i) const = 0;

	virtual void stamp(CircuitMatrix& matrix) const = 0;
	virtual void reserve_additional_rows(CircuitMatrix& matrix) {}
	virtual void update(const CircuitMatrix& matrix) {};
};
