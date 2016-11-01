#include "LuaCommand.h"


#include "Lua.h"
#include "Unit.h"
#include "json.hpp"

using json = nlohmann::json;

BT::EvaluationResult BT::LuaCommand::execute(const std::vector<springai::Unit*> units) {
	std::vector<int> ids;
	for (auto unit : units) {
		ids.push_back(unit->GetUnitId());
	}
	runLuaScript(json{ { "name", scriptName_ },{ "func", "RESET" }, {"units", ids} });
}

void BT::LuaCommand::reset() {
	runLuaScript(json{ {"name", scriptName_ }, { "func", "RUN"} });
}

std::unique_ptr<BT::BehaviourTree::LeafNode> BT::LuaCommand::Factory::createNode(const std::string& id, const std::map<std::string, ParameterValuePlaceholder>& parameters) const {
	return std::unique_ptr<BehaviourTree::LeafNode>(
		new LuaCommand(id, callback_, parameters.at("scriptName").asString("luaecho.lua"))); // TODO 
}

void BT::LuaCommand::runLuaScript(json params) const {
	callback->GetLua()->CallUI(("BETS COMMAND " + params.dump()).c_str(), -1);
}
