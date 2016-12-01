/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "BtEvaluator.h"

// spring_source_dir/rts
#include "ExternalAI/Interface/AISEvents.h"
#include "ExternalAI/Interface/AISCommands.h"

// generated by the C++ Wrapper scripts
#include "OOAICallback.h"
#include "Unit.h"
#include "UnitDef.h"
#include "Game.h"
#include "Map.h"
#include "Lua.h"
#include "Log.h"

#include "SequenceNode.h"
#include "ConditionNode.h"
#include "SpringCommand.h"
#include "EchoCommand.h"
#include "WaitNode.h"
#include "FlipSensor.h"
#include "MoveCommand.h"
#include "GroupReporter.h"

#include <memory>
#include <string>
#include <sstream>

#include <json.hpp>
#include "LuaCommand.h"
using json = nlohmann::json;

// Every event has its own event struct defined on AISEvents.h, which is passed as const void* data to HandleEvenet. 
/*
enum EventTopic {
	EVENT_NULL                         =  0,
	EVENT_INIT                         =  1,
	EVENT_RELEASE                      =  2,
	EVENT_UPDATE                       =  3,
	EVENT_MESSAGE                      =  4,
	EVENT_UNIT_CREATED                 =  5,
	EVENT_UNIT_FINISHED                =  6,
	EVENT_UNIT_IDLE                    =  7,
	EVENT_UNIT_MOVE_FAILED             =  8,
	EVENT_UNIT_DAMAGED                 =  9,
	EVENT_UNIT_DESTROYED               = 10,
	EVENT_UNIT_GIVEN                   = 11,
	EVENT_UNIT_CAPTURED                = 12,
	EVENT_ENEMY_ENTER_LOS              = 13,
	EVENT_ENEMY_LEAVE_LOS              = 14,
	EVENT_ENEMY_ENTER_RADAR            = 15,
	EVENT_ENEMY_LEAVE_RADAR            = 16,
	EVENT_ENEMY_DAMAGED                = 17,
	EVENT_ENEMY_DESTROYED              = 18,
	EVENT_WEAPON_FIRED                 = 19,
	EVENT_PLAYER_COMMAND               = 20,
	EVENT_SEISMIC_PING                 = 21,
	EVENT_COMMAND_FINISHED             = 22,
	EVENT_LOAD                         = 23,
	EVENT_SAVE                         = 24,
	EVENT_ENEMY_CREATED                = 25,
	EVENT_ENEMY_FINISHED               = 26,
	EVENT_LUA_MESSAGE                  = 27,
};*/

using namespace BT;

const BehaviourTree::Node::Factory* factory(const BehaviourTree::Node::Factory* f) { return f; }

BtEvaluator::BtEvaluator(springai::OOAICallback* callback) :
	callback(callback),
	game(callback->GetGame()),
	lua(callback->GetLua()),
	skirmishAIId(callback != nullptr ? callback->GetSkirmishAIId() : -1),
	nodeIdCounter(0),
	behaviourTree(),
	context(callback),
	nodeFactories() {

	for (auto factory : std::initializer_list<BehaviourTree::Node::Factory*>{
		new SequenceNode::Factory(),
		new ConditionNode::Factory(),
		new EchoCommand::Factory(callback),
		new MoveCommand::Factory(callback),
		new FlipSensor::Factory(callback),
		new WaitNode::Factory(callback),
		new GroupReporter::Factory(callback),
		new LuaCommand::Factory(callback)
	}) {
		nodeFactories[factory->typeName()] = std::unique_ptr<const BehaviourTree::Node::Factory>(factory);
	}
}

void BtEvaluator::Initialize() {
	try {
		/*json tree = R"({
					  "type": "sequence",
						"children": [
							{ "type": "groupReporter", "parameters": [ { "name": "reportCount", "value": 2 } ] },
							{ "type": "wait", "parameters": [ { "name": "time", "value": 10 } ] },
							{ "type": "echo", "parameters": [ { "name": "message", "value": "My custom message" } ] },
							{ "type": "condition", "children": [
								{ "type": "flipSensor" },
								{ "type": "wait", "parameters": [ { "name": "time", "value": 5 } ] },
								{ "type": "wait", "parameters": [ { "name": "time", "value": 2 } ] }
							] }
						]
					})"_json;*/

		json tree = R"({
					  "type": "wait",
						"parameters": [ { "name": "time", "value": 30 } ]
					})"_json;

		context = BehaviourTree::EvaluationContext(callback);
		behaviourTree.setRoot(createTreeFromJSON(tree).release());
	} catch (std::logic_error err) {
		game->SendTextMessage(err.what(), 0);
	}

	sendLuaMessage("INITIALIZED");
}

void BtEvaluator::update(int frame) {
	// tick the tree only once a "game second" == 30 ticks
	if (frame % 10 == 0) {
		behaviourTree.tick(context);

		// UPDATE_STATES message
		json update;
		for (auto& finished : context.finished()) {
			update[finished.first->id()] = nameOfEvaluationResult(finished.second);
		}
		for (auto& running : context.running()) {
			update[running->id()] = nameOfEvaluationResult(btRunning);
		}
		sendLuaMessage("UPDATE_STATES", update);
	}
}

/*
void BtEvaluator::loadTree() {
	auto mainSequence = new SequenceNode();
	{
		mainSequence->add(new UnitReporter(callback));
		/*
		auto brancher = new ConditionNode(false);
		{
		  auto condition = new FlipSensor(callback);
		  auto branchTrue = new SequenceNode();
		  {
			auto firstEcho = new EchoCommand(callback, "true branch");
			branchTrue->add(firstEcho);
			auto waiting = new WaitNode(callback, 6);
			branchTrue->add(waiting);
			auto lastEcho = new EchoCommand(callback, "true branch ends");
			branchTrue->add(lastEcho);
		  }
		  auto branchFalse = new SequenceNode();
		  {
			auto firstEcho = new EchoCommand(callback, "false branch");
			branchFalse->add(firstEcho);
			auto waiting = new WaitNode(callback, 3);
			branchFalse->add(waiting);
			auto lastEcho = new EchoCommand(callback, "false branch ends");
			branchFalse->add(lastEcho);
		  }
		  brancher->setChildren(condition, branchTrue, branchFalse);
		}
		mainSequence->add(brancher);
		*//*
	}
	behaviourTree.setRoot(mainSequence);
}
*/

void BtEvaluator::sendLuaMessage(const std::string& messageType) const {
	std::string message = "BETS " + messageType;
	//game->SendTextMessage(message.c_str(), -1);
	lua->CallUI(message.c_str(), -1);
}
void BtEvaluator::sendLuaMessage(const std::string& messageType, const nlohmann::json& data) const {
	std::string message = "BETS " + messageType + " " + data.dump();
	//game->SendTextMessage(message.c_str(), -1);
	lua->CallUI(message.c_str(), -1);
}

void BtEvaluator::receiveLuaMessage(const std::string& message) {
	std::stringstream sstream(message);
	std::string betsString;
	sstream >> betsString;
	if (betsString != "BETS") {
		return; // not a message for us
	}

	std::string messageCode;
	sstream >> messageCode;

	// messages without data
	if (messageCode == "REQUEST_NODE_DEFINITIONS") {
		broadcastNodeDefinitions();
	} else if(messageCode == "REINITIALIZE") {
		Initialize();
	} else if (messageCode == "ASSIGN_UNITS") {
		context = BehaviourTree::EvaluationContext(callback);
	} else {
		try {
			json data = json::parse(sstream);

			// messages with data
			if (messageCode == "CREATE_TREE") {
				context = BehaviourTree::EvaluationContext(callback);
				behaviourTree.setRoot(createTreeFromJSON(data).release());
			}
		} catch (std::logic_error err) {
			// FIXME: logic_error can be raised by other things than the json library
			game->SendTextMessage(("JSON error: " + std::string(err.what())).c_str(), 0);
		}
	}
}

void BtEvaluator::broadcastNodeDefinitions() const {
	json definitions;

	for (auto& pair : nodeFactories) {
		auto& name = pair.first;
		auto& factory = pair.second;
		json children(json::array()), parameters(json::array());

		for (auto& parameter : factory->parameters()) {
			parameters.push_back({
				{ "name", parameter.name },
				{ "variableType", parameter.variableType },
				{ "defaultValue", parameter.defaultValue },
				{ "componentType", parameter.componentType },
			});
		}

		if (factory->unlimitedChildren()) {
			children = nullptr;
		} else {
			int i = 0;
			for (auto& child : factory->children()) {
				children.push_back({
					{ "name", ++i }
				});
			}
		}

		definitions.push_back({
			{ "name", name },
			{ "children", children },
			{ "tooltip", factory->tooltip() },
			{ "parameters", parameters },
			{ "defaultWidth", factory->defaultWidth() },
			{ "defaultHeight", factory->defaultHeight() }
		});
	}

	sendLuaMessage("NODE_DEFINITIONS", definitions);
}

std::unique_ptr<BehaviourTree::Node> BtEvaluator::createTreeFromJSON(const nlohmann::json& tree) {
	typedef BehaviourTree::Node::Factory::ParameterValuePlaceholder ParameterValuePlaceholder;

	std::string type = tree.find("type") != tree.end() ? tree["type"] : tree["nodeType"];
	if (type.size() > 0) // TODO: temporary workaround as node types from BtCreator have uppercase first character
		type[0] = std::tolower(type[0]);

	auto factoryIterator = nodeFactories.find(type);
	if (factoryIterator == nodeFactories.end()) {
		return nullptr;
	}
	auto& factory = factoryIterator->second;

	std::map<std::string, ParameterValuePlaceholder> parameters;
	if (tree.find("parameters") != tree.end()) {
		for (auto& parameter : tree["parameters"]) {
			auto& value = parameter["value"];
			parameters[parameter["name"]] = ParameterValuePlaceholder{ value.is_string() ? value.get<std::string>() : value.dump() };
		}
	}

	std::vector<std::unique_ptr<BehaviourTree::Node>> children;
	if (tree.find("children") != tree.end()) {
		for (auto& child : tree["children"]) {
			children.push_back(createTreeFromJSON(child));
		}
	}

	return factory->createNode(tree.find("id") != tree.end() ? tree["id"].get<std::string>() : std::to_string(++nodeIdCounter), parameters, children);
}

int BtEvaluator::HandleEvent(int event, const void* data) {

	switch (event) {
		case EVENT_UPDATE: {
			// every frame UPDATE_EVENT is called
			const SUpdateEvent* updateData = static_cast<const SUpdateEvent*>(data);
			update(updateData->frame);
		} break;
		case EVENT_LUA_MESSAGE: {
			std::string message = static_cast<const SLuaMessageEvent*>(data)->inData;
			game->SendTextMessage(("AI received message from Lua: " + message).c_str(), 0);
			receiveLuaMessage(message);

			auto units = callback->GetSelectedUnits();
			context.setUnits(units);
		} break;
		default: {
			//game->SendTextMessage("Default Event ", 0);
		} break;
	}

	// signal: everything went OK
	return 0;
}
