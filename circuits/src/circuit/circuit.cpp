#include "./circuit.h"


Circuit::Circuit(real_t timestep) : timestep(timestep), matrix() {
	ground = add_part<VoltageSource>(0.0f);
}


VoltageSource* Circuit::get_ground() const {
	return ground;
}

Node* Circuit::create_new_node() {
	auto node = std::make_unique<Node>();
	Node* raw = node.get();
	nodes.push_back(std::move(node));
	return raw;
}

void Circuit::connect(const Pin& pin_a, const Pin& pin_b) {
	if (pin_a.node == pin_b.node) {
		if (pin_a.node == nullptr) {
			Node* node = create_new_node();
			pin_a.owner->set_node(pin_a.pin_id, node);
			pin_b.owner->set_node(pin_b.pin_id, node);
		}
	}
	else if (pin_a.node == nullptr) {
		pin_a.owner->set_node(pin_a.pin_id, pin_b.node);
	}
	else {
		pin_b.owner->set_node(pin_b.pin_id, pin_a.node);
	}
}

void Circuit::update_parts() {
	// prepare the matrix and count the number of rows needed
	matrix.reset_row_count();
	for (auto& node : nodes) {
		node->set_row_id(matrix.reserve_row());
	}
	for (auto& part : parts) {
		part->reserve_additional_rows(matrix);
	}
	matrix.init(timestep);

	// fill the matrix
	for (const auto& part : parts) {
		part->stamp(matrix);
	}

	matrix.solve();

	for (auto& node : nodes) {
		const size_t id = node->get_node_id();
		node->set_voltage(matrix.get_solution_value(id));
	}

	for (auto& part : parts) {
		part->update(matrix);
	}
}

void Circuit::run_for_steps(size_t num_steps) {
	for (size_t frame = 0; frame < num_steps; ++frame) {
		update_parts();

		// TODO: record the data in the scopes etc.
	}
}

void Circuit::run_for_seconds(real_t secs) {
	run_for_steps(static_cast<size_t>(secs/timestep));
}

// scopes
void Circuit::scope_voltage(const ConstPin& a, const ConstPin& b, const fs::path& csv_table_dst) {
	
}

void Circuit::scope_current(const ConstPin& a, const ConstPin& b, const fs::path& csv_table_dst) {

}

void Circuit::scope_current(NPinPart<2>* part, const fs::path& csv_table_dst) {

}

void Circuit::export_tables() const {

}
