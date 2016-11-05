#include "LuaCommand.h"


#include "Lua.h"
#include "Unit.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

BT::EvaluationResult BT::LuaCommand::execute(const vector<springai::Unit*> units) {
	vector<int> ids;
	for (auto unit : units) {
		ids.push_back(unit->GetUnitId());
	}
	string result = runLuaScript(json{ { "name", scriptName_ },{ "func", "RUN" }, {"units", ids}, {"parameter", parameter_} });
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

void BT::LuaCommand::reset() {
	runLuaScript(json{ {"name", scriptName_ }, { "func", "RESET"} });
}

unique_ptr<BT::BehaviourTree::LeafNode> BT::LuaCommand::Factory::createNode(const string& id, const map<string, ParameterValuePlaceholder>& parameters) const {
	return unique_ptr<LeafNode>(
		new LuaCommand(id, callback_, parameters.at("scriptName").asString(), parameters.at("parameter").asString()));
}

string BT::LuaCommand::runLuaScript(json params) const {
	return callback->GetLua()->CallUI(("BETS COMMAND " + params.dump()).c_str(), -1);
}
