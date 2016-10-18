#include "EchoCommand.h"
#include "Game.h"

using namespace BT;

EvaluationResult EchoCommand::execute(const std::vector<springai::Unit*> units) {
	callback->GetGame()->SendTextMessage(("BT-Echo: " + message).c_str(), 0);
  return btSuccess;
}
