#ifndef _CONDITIONNODE_H
#define _CONDITIONNODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT {

	/**
	 * Ternary node where the result of the first child determines if second or third child will be ticked.
	 */
	class ConditionNode : public BehaviourTree::TernaryNode {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		ConditionNode(const std::string& id, bool conditionRepeatable) :
			TernaryNode(id),
			conditionRepeatable_(conditionRepeatable), // reevaluate 1st on each tick
			lastResult_(btUndefined) {}
		virtual ~ConditionNode() {}

		std::string name() override { return "ConditionNode"; }
		EvaluationResult tick(EvaluationContext& context) override;
		void reset(const EvaluationContext& context) override;

		class Factory : public TernaryNode::Factory {
		private:
			typedef BehaviourTree::ChildDefinition ChildDefinition;
		public:
			Factory() : TernaryNode::Factory() {}

			std::string typeName() const override { return "condition"; }
			std::vector<BehaviourTree::ParameterDefinition> parameters() const override;
		protected:
			virtual ChildDefinition firstChild() const { return ChildDefinition(); }
			virtual ChildDefinition secondChild() const { return ChildDefinition(); }
			virtual ChildDefinition thirdChild() const { return ChildDefinition(); }

			std::unique_ptr<TernaryNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;
            virtual std::string tooltip() const override;
		};
	private:
		bool conditionRepeatable_;

		// evaluation variable
		EvaluationResult lastResult_;
	};
}

#endif // _CONDITIONNODE_H