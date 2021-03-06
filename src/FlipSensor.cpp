#include "FlipSensor.h"
#include "Game.h"

using namespace BT;

EvaluationResult FlipSensor::execute(const EvaluationContext& context) {
  if (currentState_ == btSuccess)
    currentState_ = btFailure;
  else
    currentState_ = btSuccess;

	callback->GetGame()->SendTextMessage((std::string("BT-Flip: currently returning ") + (currentState_ == btSuccess ? "success" : "failure")).c_str(), 0);
  return currentState_;
}


std::unique_ptr<BehaviourTree::LeafNode> FlipSensor::Factory::createNode(
	const std::string& id, 
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	return std::unique_ptr<BehaviourTree::LeafNode>(
		new FlipSensor(id, callback_)
	);
}

std::string FlipSensor::Factory::tooltip() const {
	return "FlipSensor first returns 'Success'. Every consecutive tick returns the negation of last returned state. ";
}