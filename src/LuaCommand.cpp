#include "LuaCommand.h"


#include "Lua.h"
#include "Unit.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
using namespace BT;

EvaluationResult LuaCommand::execute(const EvaluationContext& context) {
	vector<int> ids;
	for (auto unit : context.units()) {
		ids.push_back(unit->GetUnitId());
	}
	string result = runLuaScript(json{ {"func", "RUN"},{"units", ids}, { "parameter", parameter_ }, { "treeId", context.treeInstanceId() } });
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
	runLuaScript(json{ {"func", "RESET"}, { "treeId", context.treeInstanceId() } });
}

std::vector<BehaviourTree::ParameterDefinition> BT::LuaCommand::Factory::parameters() const {
	return{
		BehaviourTree::ParameterDefinition{
			"scriptName",
			"string",
			"editBox",
			"move.lua"
		},
		BehaviourTree::ParameterDefinition{
			"x",
			"number",
			"editBox",
			"0"
		}, BehaviourTree::ParameterDefinition{
			"y",
			"number",
			"editBox",
			"0"
		} };
}

unique_ptr<BT::BehaviourTree::LeafNode> BT::LuaCommand::Factory::createNode(const string& id, const map<string, ParameterValuePlaceholder>& parameters) const {
	json paramJson;

	for (auto it = parameters.begin(); it != parameters.end(); ++it) {
		if (it->first == "scriptName") {
			continue;
		}

		paramJson[it->first] = it->second.asString();
	}
    
    int x, y;

    auto it = parameters.find("x");
	if (it != parameters.end())
		x = it->second.asInteger();
    paramJson[it->first] = it->second.asInteger();

    it = parameters.find("y");
	if (it != parameters.end())
		y = it->second.asInteger();
    paramJson[it->first] = it->second.asInteger();

	return unique_ptr<LeafNode>(
		new LuaCommand(id, callback_, parameters.at("scriptName").asString(), paramJson));
}

string BT::LuaCommand::runLuaScript(json params) const {
	params["name"] = scriptName_;
	params["id"] = id();
	return callback->GetLua()->CallUI(("BETS COMMAND " + params.dump()).c_str(), -1);
}
