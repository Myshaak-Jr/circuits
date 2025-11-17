#include "voltage_source.h"



VoltageSource::VoltageSource(const std::string& name, real_t voltage) : NPinPart<1>(name), voltage(voltage), branch_id(0), current(0) {}

VoltageSource::~VoltageSource() {}

void VoltageSource::stamp(CircuitMatrix& matrix) const {
	matrix.stamp_output_voltage(pin(), branch_id, voltage);
}

void VoltageSource::reserve_additional_rows(CircuitMatrix& matrix) {
	branch_id = matrix.reserve_row();
}

void VoltageSource::update(const CircuitMatrix& matrix) {
	current = matrix.get_solution_value(branch_id);
}

real_t VoltageSource::get_current_between(const ConstPin& a, const ConstPin& b) const {
	if (a.owner != this) {
		throw std::runtime_error("Pin a must belong to this part.");
	}
	else if (!b.node->is_ground()) {
		throw std::runtime_error("Pin b must be a ground pin.");
	}

	return current;
}


VoltageSource2Pin::VoltageSource2Pin(const std::string& name, real_t voltage) : NPinPart<2>(name), voltage(voltage), branch_id(0), current(0) {}

VoltageSource2Pin::~VoltageSource2Pin() {}

void VoltageSource2Pin::stamp(CircuitMatrix& matrix) const {
	matrix.stamp_output_voltage(pin(0), pin(1), branch_id, voltage);
}

void VoltageSource2Pin::reserve_additional_rows(CircuitMatrix& matrix) {
	branch_id = matrix.reserve_row();
}

void VoltageSource2Pin::update(const CircuitMatrix& matrix) {
	current = matrix.get_solution_value(branch_id);
}

real_t VoltageSource2Pin::get_current_between(const ConstPin& a, const ConstPin& b) const {
	if (a.owner != this || b.owner != this) {
		throw std::runtime_error("Pins a and b must belong to this part.");
	}
	return current;
}