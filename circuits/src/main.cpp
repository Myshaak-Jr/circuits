#include <iostream>

#include "./circuit/circuit.h"
#include "./circuit/resistor.h"


int main() {
	Circuit circuit{1e-3};

	VoltageSource* ground = circuit.get_ground();
	VoltageSource* source = circuit.add_part<VoltageSource>(5.0f);

	Resistor* r1 = circuit.add_part<Resistor>(10.0f);
	Resistor* r2 = circuit.add_part<Resistor>(10.0f);

	circuit.connect(r1->pin(0), source->pin());
	circuit.connect(r1->pin(1), r2->pin(0));
	circuit.connect(r2->pin(1), ground->pin());
	
	circuit.scope_voltage(r1->pin(1), ground->pin(), "voltage.csv");
	circuit.scope_current(r1, "current.csv");

	circuit.run_for_seconds(10);

	circuit.export_tables();
	
	return 0;
}