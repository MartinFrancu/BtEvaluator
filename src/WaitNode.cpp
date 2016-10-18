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
