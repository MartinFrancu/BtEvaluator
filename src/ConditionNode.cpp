#include "ConditionNode.h"

using namespace BT;

EvaluationResult ConditionNode::tick(EvaluationContext& context)
{
  EvaluationResult conditionResult = lastResult_;
  if (conditionRepeatable_ || lastResult_ == btUndefined)
  {
    conditionResult = context.tickNode(firstChild());
    if (conditionResult != btRunning)
      lastResult_ = conditionResult;
  }

  EvaluationResult branchResult = btRunning;
  if (lastResult_ == btSuccess)
    branchResult = context.tickNode(secondChild());
  else if (lastResult_ == btFailure)
    branchResult = context.tickNode(thirdChild());

  if (branchResult != btRunning)
    reset();

  if (conditionResult == btRunning)
    return btRunning;
  else
    return branchResult;
}

void ConditionNode::reset()
{
	Node::reset();
	lastResult_ = btUndefined;
}


std::unique_ptr<BehaviourTree::TernaryNode> ConditionNode::Factory::createNode(
	const std::string& id,
	const std::string& treeId,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	return std::unique_ptr<BehaviourTree::TernaryNode>(
		new ConditionNode(id, false)
		);
}