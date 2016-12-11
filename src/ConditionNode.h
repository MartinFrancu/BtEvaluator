#ifndef _CONDITIONNODE_H
#define _CONDITIONNODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT {
	class ConditionNode : public BehaviourTree::TernaryNode {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		ConditionNode(const std::string& id, bool conditionRepeatable) :
			TernaryNode(id),
			conditionRepeatable_(conditionRepeatable),
			lastResult_(btUndefined) {}
		virtual ~ConditionNode() {}

		std::string name() override { return "ConditionNode"; }
		EvaluationResult tick(EvaluationContext& context) override;
		virtual void reset() override;

		class Factory : public TernaryNode::Factory {
		private:
			typedef BehaviourTree::ChildDefinition ChildDefinition;
		public:
			Factory() : TernaryNode::Factory() {}

			std::string typeName() const override { return "condition"; }
		protected:
			virtual ChildDefinition firstChild() const { return ChildDefinition(); }
			virtual ChildDefinition secondChild() const { return ChildDefinition(); }
			virtual ChildDefinition thirdChild() const { return ChildDefinition(); }

			std::unique_ptr<TernaryNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;
		};
	private:
		bool conditionRepeatable_;

		// evaluation variable
		EvaluationResult lastResult_;
	};
}

#endif // _CONDITIONNODE_H