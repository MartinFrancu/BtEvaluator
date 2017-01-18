#include "RepeatNode.h"

using namespace BT;

EvaluationResult RepeatNode::tick(EvaluationContext& context)
{
	context.tickNode(child());
	return btRunning;
}

std::unique_ptr<BehaviourTree::UnaryNode> RepeatNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	return std::unique_ptr<BehaviourTree::UnaryNode>(
		new RepeatNode(id)
	);
}