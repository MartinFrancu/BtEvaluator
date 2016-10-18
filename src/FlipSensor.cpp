#include "FlipSensor.h"
#include "Game.h"

using namespace BT;

EvaluationResult FlipSensor::execute(const std::vector<springai::Unit*> units) {
  if (currentState_ == btSuccess)
    currentState_ = btFailure;
  else
    currentState_ = btSuccess;

	callback->GetGame()->SendTextMessage((std::string("BT-Flip: currently returning ") + (currentState_ == btSuccess ? "success" : "failure")).c_str(), 0);
  return currentState_;
}
