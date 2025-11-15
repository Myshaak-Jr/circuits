#pragma once

#include <memory>
#include <vector>
#include <filesystem>

#include "./part_base.h"
#include "./node.h"
#include "./voltage_source.h"
#include "./circuit_matrix.h"
#include "./types.h"


namespace fs = std::filesystem;

class Circuit {
private:
	std::vector<std::unique_ptr<Node>> nodes;
	std::vector<std::unique_ptr<Part>> parts;

	std::vector<VoltageSource*> sources;

	VoltageSource* ground;

	Node* create_new_node();

	CircuitMatrix matrix;

	real_t timestep;

	void update_parts();

public:
	explicit Circuit(real_t timestep);
	~Circuit() noexcept = default;


	template <class TPart, class... TArgs>
	TPart* add_part(TArgs&&... args) {
		static_assert(std::is_base_of_v<Part, TPart>, "TPart must derive from Part.");

		auto part = std::make_unique<TPart>(std::forward<TArgs>(args)...);
		TPart* raw = part.get();
		parts.push_back(std::move(part));

		if constexpr (std::is_same_v<TPart, VoltageSource>) {
			sources.push_back(raw);
		}

		return raw;
	}

	VoltageSource* get_ground() const;

	void connect(const Pin& pin_a, const Pin& pin_b);

	inline void set_timestep(real_t dt) { timestep = dt; }
	inline real_t get_timestep() const { return timestep; }

	void scope_voltage(const ConstPin& a, const ConstPin& b, const fs::path& csv_table_dst);
	// Pin a and b must be of the same part
	void scope_current(const ConstPin& a, const ConstPin& b, const fs::path& csv_table_dst);
	void scope_current(NPinPart<2>* part, const fs::path& csv_table_dst);

	void export_tables() const;

	void run_for_steps(size_t num_steps);
	void run_for_seconds(real_t secs);
};
