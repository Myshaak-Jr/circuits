#include "./circuit_matrix.h"


CircuitMatrix::CircuitMatrix() : timestep(), timestep_inv(), matrix(), rhs(), num_rows(0), solution() {}

void CircuitMatrix::reset_row_count() {
	num_rows = 0;
}

size_t CircuitMatrix::reserve_row() {
	return num_rows++;
}

void CircuitMatrix::init(real_t timestep) {
	this->timestep = timestep;
	this->timestep_inv = 1.0 / timestep;
	matrix.assign(num_rows, num_rows);
	rhs.assign(num_rows);
}

void CircuitMatrix::stamp_output_template(const ConstPin& a, const ConstPin& b, size_t branch_id, real_t g_like_value, real_t cross_value, real_t rhs_value) {
	matrix(branch_id, branch_id) += cross_value;
	rhs[branch_id] += rhs_value;

	if (!a.node->is_ground()) {
		const size_t id = a.node->get_node_id();
		matrix(branch_id, id) += g_like_value;
		matrix(id, branch_id) += 1.0;
	}
	if (!b.node->is_ground()) {
		const size_t id = b.node->get_node_id();
		matrix(branch_id, id) -= g_like_value;
		matrix(id, branch_id) -= 1.0;
	}
}

void CircuitMatrix::stamp_output_voltage(const ConstPin& p, size_t branch_id, real_t voltage) {
	rhs[branch_id] += voltage;

	if (!p.node->is_ground()) {
		const size_t id = p.node->get_node_id();
		matrix(branch_id, id) += 1.0;
		matrix(id, branch_id) += 1.0;
	}
}

void CircuitMatrix::stamp_conductance(const ConstPin& a, const ConstPin& b, real_t conductance) {
	const bool a_ground = a.node->is_ground();
	const bool b_ground = b.node->is_ground();

	if (!a_ground && !b_ground) {
		const size_t row_1 = a.node->get_node_id();
		const size_t row_2 = b.node->get_node_id();
		matrix(row_1, row_1) += conductance;
		matrix(row_1, row_2) -= conductance;
		matrix(row_2, row_1) -= conductance;
		matrix(row_2, row_2) += conductance;
	}
	else if (a_ground && !b_ground) {
		const size_t row = b.node->get_node_id();
		matrix(row, row) += conductance;
	}
	else if (!a_ground && b_ground) {
		const size_t row = a.node->get_node_id();
		matrix(row, row) += conductance;
	}
}

void CircuitMatrix::stamp_current(const ConstPin& a, const ConstPin& b, real_t current) {
	if (!a.node->is_ground())
		rhs[a.node->get_node_id()] -= current;
	if (!b.node->is_ground())
		rhs[b.node->get_node_id()] -= current;
}


void CircuitMatrix::solve() {
	lingebra::solve_gaussian_elimination(matrix, rhs);
	solution = rhs;
}