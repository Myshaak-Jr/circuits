#include "../circuit_matrix.h"
#include "../n_pin_part.h"
#include "../part.h"
#include "../pin.h"
#include "../scalar.h"
#include "capacitor.h"
#include <string>



Capacitor::Capacitor(const std::string &name, scalar capacitance) :
	NPinPart<2>(name),
	capacitance(capacitance),
	last_v(0.0),
	last_i(0.0),
	admittance(0.0) {
}


void Capacitor::pre_stamp(CircuitMatrix &matrix, const StampParams &params) {
	admittance = capacitance * params.timestep_inv;
}

void Capacitor::stamp(CircuitMatrix &matrix, const StampParams &params) const {
	matrix.stamp_template_LHS(pin(0), pin(1), admittance);
	matrix.stamp_template_RHS(pin(0), pin(1), -admittance * last_v);
}

void Capacitor::post_stamp(const CircuitMatrix &matrix, const StampParams &params) {
	scalar v_now = pin(0).node->voltage - pin(1).node->voltage;
	last_i = admittance * (v_now - last_v);
	last_v = v_now;
}

scalar Capacitor::get_current_between(const ConstPin &a, const ConstPin &b) const {
	return last_i;
}