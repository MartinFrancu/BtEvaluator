#pragma once
#include "SpringCommand.h"
#include <json.hpp>

namespace BT {

	/**
	 * Command node representing any lua based command.
	 * When ticked, sends the name of the the script, along with node parameters, to the Lua side, where the script itself is run.
	 */
	class LuaCommand : public SpringCommand {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		LuaCommand(const std::string& id, springai::OOAICallback* callback, std::string scriptName, nlohmann::json parameter = {});
		~LuaCommand() {};

		EvaluationResult execute(const EvaluationContext& context) override;
		void reset(const EvaluationContext& context) override;

		virtual std::string name() override { return "LuaCommand"; }


		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}

			std::string typeName() const override {
				return "luaCommand";
			}

			std::vector<BehaviourTree::ParameterDefinition> parameters() const override;

			// Command nodes are created separately in BtCreator
			bool isAvailableInNodepool() const override { return false; }
		protected:
			std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;
		};
	private:
		std::string runLuaScript(nlohmann::json params) const;

		springai::Lua* lua_;
		std::string scriptName_;
		nlohmann::json parameter_;
	};
}

