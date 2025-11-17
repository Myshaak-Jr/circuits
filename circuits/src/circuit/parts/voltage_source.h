#pragma once


#include "../n_pin_part.h"


class VoltageSource : public NPinPart<1> {
private:
	real_t voltage;
	size_t branch_id;

	real_t current;

public:
	explicit VoltageSource(const std::string& name, real_t voltage);
	~VoltageSource() noexcept;

	void stamp(CircuitMatrix& matrix) const override;
	void reserve_additional_rows(CircuitMatrix& matrix) override;
	void update(const CircuitMatrix& matrix) override;

	real_t get_current_between(const ConstPin& a, const ConstPin& b) const override;
};


class VoltageSource2Pin : public NPinPart<2> {
private:
	real_t voltage;
	size_t branch_id;

	real_t current;

public:
	explicit VoltageSource2Pin(const std::string& name, real_t voltage);
	~VoltageSource2Pin() noexcept;

	void stamp(CircuitMatrix& matrix) const override;
	void reserve_additional_rows(CircuitMatrix& matrix) override;
	void update(const CircuitMatrix& matrix) override;

	real_t get_current_between(const ConstPin& a, const ConstPin& b) const override;
};