#include "WaitNode.h"
#include "Game.h"

using namespace BT;

EvaluationResult WaitNode::execute(const std::vector<springai::Unit*> units) {
  if (++counter_ > tickCount_)
  {
    reset();
    return btSuccess;
  }
  else
  {
    callback->GetGame()->SendTextMessage(("Waiting for " + std::to_string(tickCount_)).c_str(), 0);
    return btRunning;
  }
}

void WaitNode::reset()
{
  counter_ = 0;
}


std::unique_ptr<BehaviourTree::LeafNode> WaitNode::Factory::createNode(
	const std::string& id,
	const std::string& treeId,
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
