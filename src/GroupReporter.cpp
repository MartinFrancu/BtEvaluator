#include "GroupReporter.h"
#include "Game.h"
#include "Unit.h"

#include <string>
#include <sstream>

using namespace BT;

EvaluationResult GroupReporter::execute(const EvaluationContext& context) {
  std::string message("Current units: ");

  int limit = maxUnitReported_;
  for (auto it = context.units().begin(); it != context.units().end() && --limit >= 0; ++it)
  {
    auto unit(*it);
    if (limit + 1 != maxUnitReported_)
      message += ", ";
    message += std::to_string(unit->GetUnitId());
  }
  if (limit < 0)
    message += "...";

	message += "                                                                                                                  ";
	if (context.activeRole() != EvaluationContext::ALL_ROLES) {
		message += "Role: " + std::to_string(context.activeRole()) + "   ";
	}
	message += "Instance: " + context.treeInstanceId();

  callback->GetGame()->SendTextMessage(message.c_str(), 0);

  return btSuccess;
}


std::unique_ptr<BehaviourTree::LeafNode> GroupReporter::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	int reportCount = 5;

	auto reportIterator = parameters.find("reportCount");
	if (reportIterator != parameters.end())
		reportCount = reportIterator->second.asInteger(reportCount);

	return std::unique_ptr<BehaviourTree::LeafNode>(
		new GroupReporter(id, callback_, reportCount)
	);
}

std::string GroupReporter::Factory::tooltip() const {
	return "GroupReporter node echoes the unit IDs and also the current role name if any. Then returns 'Success'. ";
}