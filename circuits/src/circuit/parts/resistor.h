#pragma once

#include "../circuit_matrix.h"
#include "../n_pin_part.h"
#include "../part.h"
#include "../pin.h"
#include "../scalar.h"
#include <string>


class Resistor : public NPinPart<2> {
private:
	scalar ohms;
	scalar conductance;

public:
	Resistor(const std::string &name, scalar ohms);
	~Resistor() noexcept = default;

	void stamp(CircuitMatrix &matrix, const StampParams &params) const override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;

	constexpr bool requires_matrix_row() const override { return false; }
};