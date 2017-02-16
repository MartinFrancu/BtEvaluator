#include "LuaExpression.h"
#include "ConditionNode.h"


#include "Lua.h"
#include "Unit.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
using namespace BT;

LuaExpression::LuaExpression(const std::string& id, springai::OOAICallback* callback, nlohmann::json expression)
	: SpringCommand(id, callback), expression_(expression), lua_(callback->GetLua())
{
	lua_->CallUI(("BETS CREATE_EXPRESSION " + (json{ { "id", id }, { "expression", expression } }).dump()).c_str(), -1);
}

EvaluationResult LuaExpression::execute(const EvaluationContext& context) {
	vector<int> ids;
	for (auto unit : context.units()) {
		ids.push_back(unit->GetUnitId());
	}
	string result = runLuaScript(json{ {"func", "RUN"},/*{"units", ids},*/ { "expression", expression_ }, { "treeId", context.treeInstanceId() }, { "roleId", context.activeRole() } });
	if (result == "R") {
		return btRunning;
	} else if (result == "S") {
		return btSuccess;
	} else if (result == "F") {
		return btFailure;
	} else {
		return btUndefined;
	}
}

void LuaExpression::reset(const EvaluationContext& context) {
	runLuaScript(json{ {"func", "RESET"}, { "treeId", context.treeInstanceId() },{ "roleId", context.activeRole() } });
}

std::vector<BehaviourTree::ParameterDefinition> BT::LuaExpression::Factory::parameters() const {
	return{
		BehaviourTree::ParameterDefinition{
			"expression",
			"expression",
			"editBox",
			"true"
		}
	};
}

std::unique_ptr<BehaviourTree::Node> BT::LuaExpression::Factory::createNode(const string& id, const map<string, ParameterValuePlaceholder>& parameters, std::vector<std::unique_ptr<Node>>& children) const {
	if (children.size() > 2)
		throw "Invalid number of children.";

	auto conditionNode = unique_ptr<BehaviourTree::TernaryNode>(
		new ConditionNode(id, false)
	);
	conditionNode->setChildren(
		new LuaExpression(id + "-condition", callback_, parameters.at("expression").jsonValue),
		children.size() <= 0 ? 0 : children[0].release(),
		children.size() <= 1 ? 0 : children[1].release()
	);
	return std::move(conditionNode);

}

string BT::LuaExpression::runLuaScript(json params) const {
	params["id"] = id();
	return callback->GetLua()->CallUI(("BETS EXPRESSION " + params.dump()).c_str(), -1);
}
