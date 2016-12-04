#pragma once
#include "SpringCommand.h"
#include <json.hpp>

namespace BT {

	class LuaCommand : public SpringCommand {
	public:
		LuaCommand(const std::string& id, const std::string& treeId, springai::OOAICallback* callback, std::string scriptName, nlohmann::json parameter = {}) : SpringCommand(id, callback), scriptName_(scriptName), treeId_(treeId), parameter_(parameter) {}
		~LuaCommand() {};

		EvaluationResult execute(const std::vector<springai::Unit*> units) override;
		void reset() override;

		virtual std::string name() override { return "LuaCommand"; }
		const std::string& treeId() const { return treeId_; }

		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}

			std::string typeName() const override {
				return "luaCommand";
			}

			std::vector<BehaviourTree::ParameterDefinition> parameters() const override;
		protected:
			std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::string& treeInstanceId,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;
		};
	private:
		std::string runLuaScript(nlohmann::json params) const;

		std::string scriptName_;
		std::string treeId_; // TODO move to Node? SpringCommand?
		nlohmann::json parameter_;
	};
}

