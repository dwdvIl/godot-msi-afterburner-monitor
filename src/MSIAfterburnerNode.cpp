
#include "MSIAfterburnerNode.h"
#include <godot_cpp/core/class_db.hpp>

#include "MAHMSharedMemoryImpl.h"

using namespace godot;

void MSIAfterburnerNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("poll"), &MSIAfterburnerNode::poll);
	ClassDB::bind_method(D_METHOD("get_data"), &MSIAfterburnerNode::get_data);
	// Export variable
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "data"), "", "get_data");
}

void MSIAfterburnerNode::_ready() {
	MAHM_Connect();
}

bool MSIAfterburnerNode::poll() {
	return MAHM_UpdateMonitor(&data);
}

Dictionary MSIAfterburnerNode::get_data() {
	return data;
}