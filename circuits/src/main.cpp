#include <iostream>

#include "settings.h"

#include "circuit/circuit.h"
#include "circuit/parts/capacitor.h"
#include "circuit/parts/resistor.h"
#include "circuit/parts/voltage_source.h"
#include "circuit/scalar.h"



int main(int argc, char *argv[]) {
	// handle arguments
	Settings settings = handle_args(argc, argv);
	if (settings.exit) return settings.exit_code;

	// Main code
	Circuit circuit(1e-3, settings.tables_path);

	VoltageSource *ground = circuit.get_ground();
	VoltageSource *source = circuit.add_part<VoltageSource>("V1", 5);

	Resistor *R1 = circuit.add_part<Resistor>("R1", 1_k);
	Capacitor *C1 = circuit.add_part<Capacitor>("C1", 10_u);

	circuit.connect(R1->pin(0), source->pin());
	circuit.connect(R1->pin(1), C1->pin(0));
	circuit.connect(C1->pin(1), ground->pin());

	circuit.scope_voltage(C1->pin(0), C1->pin(1));
	circuit.scope_current(C1);

	std::cout << "Node Count: " << circuit.get_nodes().size() << "\n";
	std::cout << "Part Count: " << circuit.get_parts().size() << "\n";

	circuit.run_for_seconds(10_m);

	circuit.export_tables();

	return 0;
}