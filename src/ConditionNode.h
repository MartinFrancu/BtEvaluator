#ifndef _CONDITIONNODE_H
#define _CONDITIONNODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT
{
  class ConditionNode : public BehaviourTree::TernaryNode
  {
    typedef BehaviourTree::EvaluationContext EvaluationContext;
  public:
    ConditionNode(const std::string& id, bool conditionRepeatable) :
			TernaryNode(id),
      conditionRepeatable_(conditionRepeatable),
      lastResult_(btUndefined)
    {}
    virtual ~ConditionNode() {}

    virtual std::string name() { return "ConditionNode"; }
    virtual EvaluationResult tick(EvaluationContext& context) override;
    virtual void reset() override;

		class Factory : public TernaryNode::Factory {
		private:
			typedef BehaviourTree::ChildDefinition ChildDefinition;
		public:
			Factory() : TernaryNode::Factory() {}
			virtual std::string typeName() const { return "condition"; }
		protected:
			virtual ChildDefinition firstChild() const { return ChildDefinition(); }
			virtual ChildDefinition secondChild() const { return ChildDefinition(); }
			virtual ChildDefinition thirdChild() const { return ChildDefinition(); }
			virtual std::unique_ptr<TernaryNode> createNode(
				const std::string& id, const std::string& treeInstanceId,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const;
		};
	private:
    bool conditionRepeatable_;

    // evaluation variable
    EvaluationResult lastResult_;
  };
}

#endif // _CONDITIONNODE_H