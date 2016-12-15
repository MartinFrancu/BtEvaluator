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
    reset(context);

  if (conditionResult == btRunning)
    return btRunning;
  else
    return branchResult;
}

void ConditionNode::reset(const EvaluationContext& context)
{
	Node::reset(context);
	lastResult_ = btUndefined;
}

std::vector<BT::BehaviourTree::ParameterDefinition> ConditionNode::Factory::parameters() const {
	return{
		BehaviourTree::ParameterDefinition {
			"repeat",
			"bool",
			"checkBox",
			"false"
		}
	};
}

std::unique_ptr<BehaviourTree::TernaryNode> ConditionNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	bool repeatable;

	auto it = parameters.find("repeat");
	if (it != parameters.end()) {
		repeatable = it->second.asBoolean();
	}

	return std::unique_ptr<BehaviourTree::TernaryNode>(
		new ConditionNode(id, false)
	);
}