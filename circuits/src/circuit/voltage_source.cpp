#include "voltage_source.h"



VoltageSource::VoltageSource(real_t voltage) : voltage(voltage), branch_id(0) {}

VoltageSource::~VoltageSource() {}

void VoltageSource::stamp(CircuitMatrix& matrix) const {
	matrix.stamp_output_voltage(pin(), branch_id, voltage);
}

void VoltageSource::reserve_additional_rows(CircuitMatrix& matrix) {
	branch_id = matrix.reserve_row();
}


VoltageSource2Pin::VoltageSource2Pin(real_t voltage) : voltage(voltage), branch_id(0) {}

VoltageSource2Pin::~VoltageSource2Pin() {}

void VoltageSource2Pin::stamp(CircuitMatrix& matrix) const {
	matrix.stamp_output_voltage(pin(0), pin(1), branch_id, voltage);
}

void VoltageSource2Pin::reserve_additional_rows(CircuitMatrix& matrix) {
	branch_id = matrix.reserve_row();
}