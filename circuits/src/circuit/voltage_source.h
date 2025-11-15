#pragma once


#include "n_pin_part.h"


class VoltageSource : public NPinPart<1> {
private:
	real_t voltage;
	size_t branch_id;

public:
	explicit VoltageSource(real_t voltage);
	~VoltageSource() noexcept;

	void stamp(CircuitMatrix& matrix) const override;
	void reserve_additional_rows(CircuitMatrix& matrix) override;
};


class VoltageSource2Pin : public NPinPart<2> {
private:
	real_t voltage;
	size_t branch_id;

public:
	explicit VoltageSource2Pin(real_t voltage);
	~VoltageSource2Pin() noexcept;

	void stamp(CircuitMatrix& matrix) const override;
	void reserve_additional_rows(CircuitMatrix& matrix) override;
};