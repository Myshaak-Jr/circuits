#include "./resistor.h"


Resistor::Resistor(const std::string& name, real_t ohms) : NPinPart<2>(name), ohms(ohms) {
	conductance = 1.0f / ohms;
}

void Resistor::stamp(CircuitMatrix& matrix) const {
	matrix.stamp_conductance(pin(0), pin(1), conductance);
}

real_t Resistor::get_current_between(const ConstPin& a, const ConstPin& b) const {
	if (a.owner != this || b.owner != this) {
		throw std::runtime_error("Pins a and b must belong to this part.");
	}
	return conductance * (a.node->get_voltage() - b.node->get_voltage());
}
