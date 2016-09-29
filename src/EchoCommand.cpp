#include "EchoCommand.h"
#include "Game.h"

using namespace BT;

EvaluationResult EchoCommand::execute() {
	callback->GetGame()->SendTextMessage(("BT-Echo: " + message).c_str(), 0);
  return btSuccess;
}
