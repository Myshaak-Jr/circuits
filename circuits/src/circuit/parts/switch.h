#pragma once

#include "../circuit_matrix.h"
#include "../n_pin_part.h"
#include "../part.h"
#include "../pin.h"
#include "../scalar.h"
#include <queue>
#include <string>
#include <vector>


class Switch : public NPinPart<2> {
private:
	const scalar off_resistance = 10_M;
	size_t branch_id;
	scalar last_i;

	bool on;

	enum class EventType {
		ON,
		OFF
	};

	struct Event {
		size_t step;
		EventType type;
	};

	struct Comp {
		bool operator()(const Event &a, const Event &b) const noexcept {
			if (a.step != b.step) return a.step > b.step;
			return a.type == EventType::OFF && b.type == EventType::ON;
		}
	};

	std::priority_queue<Event, std::vector<Event>, Comp> events;

public:
	Switch(const std::string &name, bool on = false);
	~Switch() noexcept = default;

	void pre_stamp(CircuitMatrix &matrix, const StampParams &params) override;
	void stamp(CircuitMatrix &matrix, const StampParams &params) const override;
	void post_stamp(const CircuitMatrix &matrix, const StampParams &params) override;

	scalar get_current_between(const ConstPin &a, const ConstPin &b) const override;

	void switch_on() { on = false; }
	void switch_off() { on = true; }

	void schedule_on(size_t step);
	void schedule_off(size_t step);

	constexpr bool requires_matrix_row() const override { return true; }
};