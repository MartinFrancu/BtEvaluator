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
#include "ConstantNode.h"
#include "RepeatNode.h"
#include "RoleSplitNode.h"
#include "SubtreeNode.h"
#include "SpringCommand.h"
#include "EchoCommand.h"
#include "WaitNode.h"
#include "FlipSensor.h"
#include "GroupReporter.h"
#include "LuaCommand.h"
#include "LuaExpression.h"

#include <memory>
#include <string>
#include <sstream>
#include <chrono>

#include <json.hpp>
#include "SelectorNode.h"
#include "ActiveSelectorNode.h"
using json = nlohmann::json;
using namespace std;

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
	treeMap(),
	reportingContext(nullptr),
	nodeFactories() {

	for (auto factory : std::initializer_list<BehaviourTree::Node::Factory*>{
		new SequenceNode::Factory(),
		new SelectorNode::Factory(),
		//new ActiveSelectorNode::Factory(),
		new ConditionNode::Factory(),
		new ConstantNode::Factory(btSuccess, "Immediately returns 'Success'. "),
		new ConstantNode::Factory(btFailure, "Immediately returns 'Failure'. "),
		new RepeatNode::Factory(),
		new RoleSplitNode::Factory(),
		//new SubtreeNode::Factory(callback),
		new SubtreeNode::ReferenceFactory(callback),
		//new FlipSensor::Factory(callback),
		new WaitNode::Factory(callback),
		//new GroupReporter::Factory(callback),
		new LuaCommand::Factory(callback),
		new LuaExpression::ConditionFactory(callback)
	}) {
		nodeFactories[factory->typeName()] = std::unique_ptr<const BehaviourTree::Node::Factory>(factory);
	}
}

void BtEvaluator::Initialize() {
	treeMap.clear();
	reportingContext = nullptr;

	sendLuaMessage("INITIALIZED");
}

void BtEvaluator::tickTree(Tree& tree) {
	auto& behaviourTree(tree.first);
	auto& context(tree.second);
	behaviourTree.tick(context);

	if (&tree.second == reportingContext) {
		// UPDATE_STATES message
		json states = json::object();
		for (auto& finished : reportingContext->finished()) {
			states[finished.first->id()] = nameOfEvaluationResult(finished.second);
		}
		for (auto& running : reportingContext->running()) {
			states[running->id()] = nameOfEvaluationResult(btRunning);
		}
		for (auto& stopped : reportingContext->stopped()) {
			states[stopped->id()] = nameOfEvaluationResult(btBreakpoint);
		}

		sendLuaMessage("UPDATE_STATES", json{
			{ "id", reportingContext->treeInstanceId() },
			{ "states", states }
		});
	}
}

void BtEvaluator::update(int frame) {
	// tick the tree only once a "game second" == 30 ticks
	if (frame % 30 == 0) {
		auto t1 = chrono::high_resolution_clock::now();
		for (auto it(treeMap.begin()); it != treeMap.end(); ++it) {
			tickTree(it->second);
		}

		auto t2 = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
		lua->CallUI(("BETS LOG Tick duration in ms: " + to_string(duration)).c_str(), -1);
	}
}

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
	} else {
		try {
			json data = json::parse(sstream);

			if (messageCode == "RESET_TREES") {
				for (auto& instIdData : data) {
					auto iterator = treeMap.find(instIdData.get<string>());
					if (iterator != treeMap.end()) {
						iterator->second.second.reset();
					}
				}
				return;
			} else if (messageCode == "TICK_TREES") {
				for (auto& instIdData : data) {
					auto iterator = treeMap.find(instIdData.get<string>());
					if (iterator != treeMap.end()) {
						tickTree(iterator->second);
					}
				}
				return;
			} else if (messageCode == "EXECUTE") {
				system(data.get<string>().c_str());
				return;
			}

			auto instanceId = data["instanceId"].get<string>();

			// messages with data
			if (messageCode == "CREATE_TREE") {
				if (treeMap.find(instanceId) == treeMap.end()) {
					treeMap.emplace(instanceId, make_pair(BehaviourTree(), BehaviourTree::EvaluationContext(callback, instanceId)));
				}
				auto& treeContextPair(treeMap.at(instanceId));
				
				try {
					treeContextPair.first.setRoot(createTreeFromJSON(data["root"]).release());
					treeContextPair.second.clear();
					sendSuccessResponse();
				}catch(const std::exception& e) {
					sendFailureResponse("Failed to create tree: " + string(e.what()));
					return;
				}
			} else if (messageCode == "REPORT_TREE") {
				auto treeIterator = treeMap.find(instanceId);
				if (treeIterator == treeMap.end()) {
					reportingContext = nullptr;
					sendFailureResponse("Failed to report tree " + instanceId);
					return;
				}
				reportingContext = &treeIterator->second.second;
				sendSuccessResponse();
			} else if (messageCode == "ASSIGN_UNITS") {
				auto iterator = treeMap.find(instanceId);

				if (iterator != treeMap.end()) {
					int roleId = data["role"];
					std::vector<springai::Unit*> units = callback->GetSelectedUnits(); // we unfortunately don't have a way to translate units ids to springai:Unit*, so we cannot take units as a parameter
					for (auto& treeMapPair : treeMap) {
						if (treeMapPair.second.second.removeUnits(units)) { // remove the units we are assigning from all trees (including the one where we will assign them next)
							treeMapPair.second.second.reset();
						}
					}
					iterator->second.second.reset();
					iterator->second.second.setUnits(roleId, units);
					sendSuccessResponse();
				} else {
					sendFailureResponse("Failed to assign units to tree " + instanceId);
					return;
				}
			}	else if (messageCode == "REMOVE_TREE") {
				auto iterator = treeMap.find(instanceId);
				if (iterator != treeMap.end()) {
					if (reportingContext == &iterator->second.second) {
						reportingContext = nullptr;
					}
					treeMap.erase(iterator);
					sendSuccessResponse();
				} else {
					sendFailureResponse("Failed to remove tree: " + instanceId);
					return;
				}
			}	else if (messageCode == "SET_BREAKPOINT") {
				auto iterator = treeMap.find(instanceId);
				if (iterator != treeMap.end()) {
					std::string nodeId = data["nodeId"];
					iterator->second.second.setBreakpoint(nodeId);
				}
			}	else if (messageCode == "REMOVE_BREAKPOINT") {
				auto iterator = treeMap.find(instanceId);
				if (iterator != treeMap.end()) {
					std::string nodeId = data["nodeId"];
					iterator->second.second.removeBreakpoint(nodeId);
				}
			}
		} catch (std::logic_error err) {
			// FIXME: logic_error can be raised by other things than the json library
			game->SendTextMessage(("JSON error: " + std::string(err.what())).c_str(), 0);
		}
	}
}

void BtEvaluator::sendSuccessResponse() const {
	sendLuaMessage("RESPONSE", json{ { "result", true } });
}

void BtEvaluator::sendFailureResponse(const std::string& error) const {
	sendLuaMessage("RESPONSE", json{ { "result", false }, { "error", error.c_str() } });
}

void BtEvaluator::broadcastNodeDefinitions() const {
	json definitions;

	for (auto& pair : nodeFactories) {
		auto& name = pair.first;
		auto& factory = pair.second;
		if (!factory->isAvailableInNodepool()) {
			continue;
		}

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
		throw std::invalid_argument("Factory for node type " + type + " doesn't exist");
	}
	auto& factory = factoryIterator->second;

	std::map<std::string, ParameterValuePlaceholder> parameters;
	if (tree.find("parameters") != tree.end()) {
		for (auto& parameter : tree["parameters"]) {
			auto& value = parameter["value"];
			parameters[parameter["name"]] = ParameterValuePlaceholder{ value.is_string() ? value.get<std::string>() : value.dump(), value };
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
			//game->SendTextMessage(("AI received message from Lua: " + message).c_str(), 0);
			receiveLuaMessage(message);

			//auto units = callback->GetSelectedUnits();
			//context.setUnits(units);
		} break;
		default: {
			//game->SendTextMessage("Default Event ", 0);
		} break;
	}

	// signal: everything went OK
	return 0;
}
