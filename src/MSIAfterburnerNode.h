
#ifndef MSIAFTERBURNERNODE_H
#define MSIAFTERBURNERNODE_H

#include "godot_cpp/classes/node.hpp"
using namespace godot;

class MSIAfterburnerNode : public Node {
    GDCLASS(MSIAfterburnerNode, Node);

	public:
	MSIAfterburnerNode() = default;
	~MSIAfterburnerNode() = default;

	void _ready() override;
	bool poll();

	Dictionary data;

	private:
	Dictionary get_data();

	protected:
    static void _bind_methods();
};


#endif