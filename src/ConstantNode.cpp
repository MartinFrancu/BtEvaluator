#include "ConstantNode.h"

using namespace BT;

EvaluationResult ConstantNode::tick(EvaluationContext& context)
{
	return _resultValue;
}

std::unique_ptr<BehaviourTree::LeafNode> ConstantNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	return std::unique_ptr<BehaviourTree::LeafNode>(
		new ConstantNode(id, _resultValue)
	);
}
