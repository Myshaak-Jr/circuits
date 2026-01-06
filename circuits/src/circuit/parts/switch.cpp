#include "switch.h"


Switch::Switch(const std::string &name, bool on) : NPinPart<2>(name), branch_id(0), last_i(0.0), on(on) {}

void Switch::pre_stamp(CircuitMatrix &matrix, const StampParams &params) {
	while (!events.empty() && events.top().step <= params.step) {
		on = events.top().type == EventType::ON;
		events.pop();
	}

	branch_id = matrix.reserve_row();
}

void Switch::stamp(CircuitMatrix &matrix, const StampParams &params) const {
	matrix.stamp_template_I_out_LHS(pin(0), pin(1), branch_id, 1.0, on ? 0 : off_resistance);
}

void Switch::post_stamp(const CircuitMatrix &matrix, const StampParams &params) {
	last_i = matrix.get_solution_value(branch_id);
}

scalar Switch::get_current_between(const ConstPin &a, const ConstPin &b) const {
	return last_i;
}

void Switch::schedule_on(size_t step) {
	events.push({ .step = step, .type = EventType::ON });
}

void Switch::schedule_off(size_t step) {
	events.push({ .step = step, .type = EventType::OFF });
}
