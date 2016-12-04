/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef _BTEVALUATOR_H
#define _BTEVALUATOR_H

// generated by the C++ Wrapper scripts
#include "OOAICallback.h"
#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"
#include <json.hpp>

namespace BT {
	class SpringCommand;


	/**
	 * This is the main C++ entry point of this AI.
	 *
	 * @author	Robin Vobruba <hoijui.quaero@gmail.com>
	 */
	class BtEvaluator {
		typedef std::map<std::string, BT::SpringCommand> CommandMap;
		typedef std::map<std::string, std::pair<BehaviourTree, BehaviourTree::EvaluationContext>> TreeMap;
	private:
		springai::OOAICallback* callback;
		springai::Game* game;
		springai::Lua* lua;

		int skirmishAIId;

		int nodeIdCounter;

		std::map<std::string, std::unique_ptr<const BehaviourTree::Node::Factory>> nodeFactories;


		TreeMap treeMap;

		//BehaviourTree behaviourTree;
		//BehaviourTree::EvaluationContext context;

		void update(int frame);

		//SpringCommand* resolveCommand(const std::string&) const;

		void sendLuaMessage(const std::string& messageType) const;
		void sendLuaMessage(const std::string& messageType, const nlohmann::json& data) const;
		void receiveLuaMessage(const std::string&);

		void broadcastNodeDefinitions() const;
		std::unique_ptr<BehaviourTree::Node> createTreeFromJSON(const nlohmann::json& tree);
	public:
		explicit BtEvaluator(springai::OOAICallback* callback);

		~BtEvaluator() {}

		void Initialize();
		int HandleEvent(int topic, const void* data);

	};
}

#endif // _BTEVALUATOR_H

