#pragma once

#include <vector>

#include "types.h"
#include "../lingebra/lingebra.h"
#include "pin.h"


class CircuitMatrix {
private:
	using matrix_t = lingebra::Matrix<real_t>;
	using vector_t = lingebra::Vector<real_t>;

	matrix_t matrix;
	vector_t rhs;

	vector_t solution;

	size_t num_rows;

	real_t timestep;
	real_t timestep_inv;

	void stamp_output_template(const ConstPin& a, const ConstPin& b, size_t branch_id, real_t g_like_value, real_t cross_value, real_t rhs_value);

public:
	CircuitMatrix();

	void reset_row_count();
	size_t reserve_row();

	// resize & clear
	void init(real_t timestep);
	
	// stamps, the current always flows a -> b

	void stamp_conductance(const ConstPin& a, const ConstPin& b, real_t conductance);
	inline void stamp_output_conductance(const ConstPin& a, const ConstPin& b, size_t branch_id, real_t conductance) {
		stamp_output_template(a, b, branch_id, conductance, -1.0, 0.0);
	}

	void stamp_current(const ConstPin& a, const ConstPin& b, real_t current);
	inline void stamp_output_current(const ConstPin& a, const ConstPin& b, size_t branch_id, real_t current) {
		stamp_output_template(a, b, branch_id, 0.0, 1.0, current);
	}

	inline void stamp_capacitance(const ConstPin& a, const ConstPin& b, real_t capacitance, real_t prev_voltage) {
		const real_t g = capacitance * timestep_inv;
		stamp_conductance(a, b, g);
		stamp_current(a, b, -g * prev_voltage);
	}
	inline void stamp_output_capacitance(const ConstPin& a, const ConstPin& b, size_t branch_id, real_t capacitance, real_t prev_voltage) {
		const real_t g = capacitance * timestep_inv;
		stamp_output_template(a, b, branch_id, g, -1.0, g * prev_voltage);
	}

	inline void stamp_output_inductance(const ConstPin& a, const ConstPin& b, size_t branch_id, real_t inductance, real_t prev_current) {
		const real_t temp = inductance * timestep_inv;
		stamp_output_template(a, b, branch_id, 1.0, -temp, -temp * prev_current);
	}

	inline void stamp_output_voltage(const ConstPin& a, const ConstPin& b, size_t branch_id, real_t voltage) {
		stamp_output_template(a, b, branch_id, 1.0, 0.0, voltage);
	}

	void stamp_output_voltage(const ConstPin& p, size_t branch_id, real_t voltage);

	void solve();
	inline real_t get_solution_value(size_t id) const { return solution[id]; }
};

