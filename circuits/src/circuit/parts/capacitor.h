#pragma once

#include "../circuit_matrix.h"
#include "../n_pin_part.h"
#include "../part.h"
#include "../pin.h"
#include "../scalar.h"
#include <string>


class Capacitor : public NPinPart<2> {
private:
	scalar capacitance;
	scalar last_v;
	scalar last_i;
	scalar admittance;

public:
	Capacitor(const std::string &name, scalar capacitance);
	~Capacitor() noexcept = default;

	void pre_stamp(CircuitMatrix &matrix, const StampParams &params) override;
	void stamp(CircuitMatrix &matrix, const StampParams &params) const override;
	void post_stamp(const CircuitMatrix &matrix, const StampParams &params) override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;

	constexpr bool requires_matrix_row() const override { return false; }
};