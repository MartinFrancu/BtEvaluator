#pragma once
#include "SpringCommand.h"
#include <json.hpp>

namespace BT {

	class LuaCommand : public SpringCommand {
	public:
		LuaCommand(const std::string& id, springai::OOAICallback* callback, std::string scriptName) : SpringCommand(id, callback), scriptName_(scriptName) {}
		~LuaCommand() {};

		EvaluationResult execute(const std::vector<springai::Unit*> units) override;
		void reset() override;

		virtual std::string name() override { return "LuaCommand"; }

		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}
			virtual std::string typeName() const { return "luaCommand"; }
		protected:
			virtual std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const;
		};
	private:
		void runLuaScript(nlohmann::json params) const;

		std::string scriptName_;
	};
}

