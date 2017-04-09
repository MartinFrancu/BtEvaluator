#pragma once
#include "SpringCommand.h"
#include <json.hpp>

namespace BT {

	class LuaExpression : public SpringCommand {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		LuaExpression(const std::string& id, springai::OOAICallback* callback, nlohmann::json expression = {});
		~LuaExpression() {};

		EvaluationResult execute(const EvaluationContext& context) override;
		void reset(const EvaluationContext& context) override;

		virtual std::string name() override { return "LuaExpression"; }

		class ConditionFactory : public Node::Factory {
		public:
			ConditionFactory(springai::OOAICallback* callback) : callback_(callback) {}

			std::string typeName() const override {
				return "luaCondition";
			}

			virtual std::unique_ptr<Node> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters,
				std::vector<std::unique_ptr<Node>>& children
				) const override;
			virtual bool unlimitedChildren() const { return false; }
			virtual int defaultWidth() const override { return 210; };
			virtual std::vector<BehaviourTree::ChildDefinition> children() const { return{ BehaviourTree::ChildDefinition(), BehaviourTree::ChildDefinition() }; }
			std::vector<BehaviourTree::ParameterDefinition> parameters() const override;
		private:
			springai::OOAICallback* callback_;
		};
	private:
		std::string runLuaScript(nlohmann::json params) const;

		springai::Lua* lua_;
		nlohmann::json expression_;
	};
}

