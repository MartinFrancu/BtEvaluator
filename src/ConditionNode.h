#ifndef _CONDITIONNODE_H
#define _CONDITIONNODE_H

#include "BehaviourTree.h"

namespace BT
{
  class ConditionNode : public BehaviourTree::TernaryNode
  {
    typedef BehaviourTree::EvaluationContext EvaluationContext;
  public:
    ConditionNode(bool conditionRepeatable) :
      conditionRepeatable_(conditionRepeatable),
      lastResult_(btUndefined)
    {}
    virtual ~ConditionNode() {}

    virtual std::string name() { return "ConditionNode"; }
    virtual EvaluationResult tick(EvaluationContext& context) override;
    virtual void reset() override;
  private:
    bool conditionRepeatable_;

    // evaluation variable
    EvaluationResult lastResult_;
  };
}

#endif // _CONDITIONNODE_H