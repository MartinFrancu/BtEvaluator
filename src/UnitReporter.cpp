#include "UnitReporter.h"
#include "Game.h"
#include "Unit.h"

#include <string>
#include <sstream>

using namespace BT;

EvaluationResult UnitReporter::execute(const std::vector<springai::Unit*> units) {
  std::string messageComposer("Current units: ");

  int limit = maxUnitReported_;
  for (auto it = units.begin(); it != units.end() && --limit >= 0; ++it)
  {
    auto unit(*it);
    if (limit != 4)
      messageComposer += ", ";
    messageComposer += std::to_string(unit->GetUnitId());
  }

  callback->GetGame()->SendTextMessage(messageComposer.c_str(), 0);

  return btSuccess;
}
