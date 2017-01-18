#include "WaitNode.h"
#include "Game.h"

using namespace BT;

EvaluationResult WaitNode::execute(const EvaluationContext& context) {
  if (++counter_ > tickCount_)
  {
    reset(context);
    return btSuccess;
  }
  else
  {
    //callback->GetGame()->SendTextMessage(("Waiting for " + std::to_string(tickCount_)).c_str(), 0);
    return btRunning;
  }
}

void WaitNode::reset(const EvaluationContext& context)
{
	Node::reset(context);
  counter_ = 0;
}


std::vector<BehaviourTree::ParameterDefinition> WaitNode::Factory::parameters() const
{
	return{
		BehaviourTree::ParameterDefinition {
			"time",
			"number",
			"editBox",
			"10"
		}
	};
}

std::unique_ptr<BehaviourTree::LeafNode> WaitNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	std::size_t time = 5;

	auto timeIterator = parameters.find("time");
	if (timeIterator != parameters.end())
		time = timeIterator->second.asInteger(time);

	return std::unique_ptr<BehaviourTree::LeafNode>(
		new WaitNode(id, callback_, time)
		);
}
