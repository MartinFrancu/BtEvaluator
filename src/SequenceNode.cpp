#include "SequenceNode.h"

using namespace BT;

EvaluationResult SequenceNode::tick(EvaluationContext& context)
{
  for (; nextChildIndex_ < count(); ++nextChildIndex_)
  {
    Node* currentChild = child(nextChildIndex_);
    EvaluationResult childResult = context.tickNode(currentChild);
		if (childResult == btBreakpoint)
			return stopAt(nextChildIndex_ + 1);

    if (childResult == btRunning)
      return notStopped(btRunning);
    else if (childResult == btFailure)
    {
      reset(context);
      return btFailure;
    }
  }
  reset(context);
  return btSuccess;
}

void SequenceNode::reset(const EvaluationContext& context)
{
	Node::reset(context);
	nextChildIndex_ = 0;
}


std::unique_ptr<BehaviourTree::GenericNode> SequenceNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	return std::unique_ptr<BehaviourTree::GenericNode>(
		new SequenceNode(id)
		);
}
