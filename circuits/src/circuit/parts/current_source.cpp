#include "../n_pin_part.h"
#include "../part.h"
#include "../pin.h"
#include "../scalar.h"
#include "current_source.h"
#include <string>


CurrentSource::CurrentSource(const std::string &name, scalar current) :
	NPinPart<2>(name),
	current(current) {
}

void CurrentSource::stamp_rhs_entries(std::vector<scalar> &rhs, const StampParams &params) {
	rhs[pin(0).node->node_id] = -current;
	rhs[pin(1).node->node_id] = current;
}

scalar CurrentSource::get_current_between(const ConstPin &a, const ConstPin &b) const {
	return current;
}

