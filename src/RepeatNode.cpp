#include "RepeatNode.h"

using namespace BT;

EvaluationResult RepeatNode::tick(EvaluationContext& context)
{
	if (context.tickNode(child()) == btBreakpoint)
		return stopAt(1);
	return notStopped(btRunning);
}

std::unique_ptr<BehaviourTree::UnaryNode> RepeatNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	return std::unique_ptr<BehaviourTree::UnaryNode>(
		new RepeatNode(id)
	);
}

 std::string RepeatNode::Factory::tooltip() const { 
    return "Repeat node is unary node, which always evaluates its only child and returns 'Running' state. "; 
 }