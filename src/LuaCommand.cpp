#include "LuaCommand.h"


#include "Lua.h"
#include "Unit.h"
#include "json.hpp"
#include "Game.h"

using json = nlohmann::json;
using namespace std;
using namespace BT;

LuaCommand::LuaCommand(const std::string& id, springai::OOAICallback* callback, std::string scriptName, nlohmann::json parameter)
	: SpringCommand(id, callback), scriptName_(scriptName), parameter_(parameter), lua_(callback->GetLua())
{
	lua_->CallUI(("BETS CREATE_COMMAND " + (json{ { "name", scriptName }, { "id", id }, { "parameter", parameter } }).dump()).c_str(), -1);
}

EvaluationResult LuaCommand::execute(const EvaluationContext& context) {
	vector<int> ids;
	for (auto unit : context.units()) {
		ids.push_back(unit->GetUnitId());
	}
	//context.callback_->GetGame()->SendTextMessage(json{ { "func", "RUN" },{ "units", ids },{ "parameter", parameter_ },{ "treeId", context.treeInstanceId() } }.dump().c_str(), 0);
	string result = runLuaScript(json{ {"func", "RUN"}/*,{"units", ids}*/, { "parameter", parameter_ }, { "treeId", context.treeInstanceId() }, { "roleId", context.activeRole() } });
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

void LuaCommand::reset(const EvaluationContext& context) {
	Node::reset(context);
	runLuaScript(json{ {"func", "RESET"}, { "treeId", context.treeInstanceId() },{ "roleId", context.activeRole() } });
}

std::vector<BehaviourTree::ParameterDefinition> BT::LuaCommand::Factory::parameters() const {
	return{};
}

unique_ptr<BT::BehaviourTree::LeafNode> BT::LuaCommand::Factory::createNode(const string& id, const map<string, ParameterValuePlaceholder>& parameters) const {
	json paramJson = json::object();

	for (auto it = parameters.begin(); it != parameters.end(); ++it) {
		if (it->first == "scriptName") {
			continue;
		}
		paramJson[it->first] = it->second.jsonValue;
	}
    
	return unique_ptr<LeafNode>(
		new LuaCommand(id, callback_, parameters.at("scriptName").asString(), paramJson));
}

string BT::LuaCommand::runLuaScript(json params) const {
	params["name"] = scriptName_;
	params["id"] = id();
	return lua_->CallUI(("BETS COMMAND " + params.dump()).c_str(), -1);
}
