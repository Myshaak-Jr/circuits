#include "./resistor.h"


Resistor::Resistor(real_t ohms) : ohms(ohms) {
	conductance = 1.0f / ohms;
}

void Resistor::stamp(CircuitMatrix& matrix) const {
	matrix.stamp_conductance(pin(0), pin(1), conductance);
}