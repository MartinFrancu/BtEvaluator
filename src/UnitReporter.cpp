#include "UnitReporter.h"
#include "Game.h"
#include "Unit.h"

#include <string>
#include <sstream>

using namespace BT;

EvaluationResult UnitReporter::execute(const std::vector<springai::Unit*> units) {
  std::string message("Current units: ");

  int limit = maxUnitReported_;
  for (auto it = units.begin(); it != units.end() && --limit >= 0; ++it)
  {
    auto unit(*it);
    if (limit != 4)
      message += ", ";
    message += std::to_string(unit->GetUnitId());
  }
  if (limit < 0)
    message += "...";

  callback->GetGame()->SendTextMessage(message.c_str(), 0);

  return btSuccess;
}
