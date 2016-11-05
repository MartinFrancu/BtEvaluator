#pragma once
#include "SpringCommand.h"
#include <json.hpp>

namespace BT {

	class LuaCommand : public SpringCommand {
	public:
		LuaCommand(const std::string& id, springai::OOAICallback* callback, std::string scriptName, std::string parameter = "") : SpringCommand(id, callback), scriptName_(scriptName), parameter_(parameter) {}
		~LuaCommand() {};

		EvaluationResult execute(const std::vector<springai::Unit*> units) override;
		void reset() override;

		virtual std::string name() override { return "LuaCommand"; }

		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}

			std::string typeName() const override {
				return "luaCommand";
			}
		protected:
			virtual std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const;
		};
	private:
		std::string runLuaScript(nlohmann::json params) const;

		std::string scriptName_;
		std::string parameter_;
	};
}

