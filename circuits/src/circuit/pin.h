#pragma once

#include "node.h"


class Part;

struct Pin {
	size_t pin_id;
	Node* node;
	Part* owner;

	constexpr Pin(size_t pin_id, Node* node, Part* owner) : pin_id(pin_id), node(node), owner(owner) {}
};

struct ConstPin {
	size_t pin_id;
	const Node* node;
	const Part* owner;

	constexpr ConstPin(size_t pin_id, const Node* node, const Part* owner) : pin_id(pin_id), node(node), owner(owner) {}
	constexpr ConstPin(const Pin& pin) : pin_id(pin.pin_id), node(pin.node), owner(pin.owner) {}
};
