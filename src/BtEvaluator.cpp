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

#include <string>
#include <sstream>

//#include "json.hpp"
//using json = nlohmann::json;

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

void BtEvaluator::loadTree() {
  auto root = new ConditionNode(false);
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
    root->setChildren(condition, branchTrue, branchFalse);
  }
  behaviourTree.setRoot(root);
}

SpringCommand* BtEvaluator::resolveCommand(const char* message) const {
	std::stringstream sstream(message);
	std::string commandCode;
	sstream >> commandCode;

	if (commandCode == "Move") {
		return new MoveCommand(callback);
	} else if (commandCode == "Msg") {
		std::string msg;
		sstream >> msg;
		return new EchoCommand(callback, msg);
	} else {
		throw std::invalid_argument("Couldn't resolve command: " + commandCode);
	}
}

int BtEvaluator::HandleEvent(int event, const void* data) {
	std::string initMsg("");/* json({
    { "test", 1 },
    { "pole", {
      1,
      2,
	  3
    } },
    { "struktura", json({
      { "polozka", "test" }
    }) }
  }).dump());
  */
  switch (event) {
	case EVENT_UPDATE:
	{ // every frame UPDATE_EVENT is called
		//game->SendTextMessage("Update Event ", 0);
		const SUpdateEvent* updateData = static_cast<const SUpdateEvent*>(data);
		if(updateData->frame % 30 == 0) // tick the tree only once a "game second" == 30 ticks
		    behaviourTree.tick(context);
		break;
	}
	case EVENT_LUA_MESSAGE:
	{
		game->SendTextMessage(initMsg.c_str(), 0);
		game->SendTextMessage("Lua Message Event. ", 0);

		game->SendTextMessage(("BT-JSON: " + behaviourTree.getAllNodeDefinitions()).c_str(), 0);
		const char* message = static_cast<const SLuaMessageEvent*>(data)->inData;
		resolveCommand(message)->execute();
		break;
	}
	default:
		//game->SendTextMessage("Default Event ", 0);
		break;
	}

	// signal: everything went OK
	return 0;
}
