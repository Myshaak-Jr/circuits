#pragma once

#include "./n_pin_part.h"


class Resistor : public NPinPart<2> {
private:
	real_t ohms;
	real_t conductance;

public:
	Resistor(real_t ohms);
	~Resistor() noexcept = default;

	void stamp(CircuitMatrix& matrix) const override;
};