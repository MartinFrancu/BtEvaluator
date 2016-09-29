#ifndef _SEQUENCENODE_H
#define _SEQUENCENODE_H

#include "BehaviourTree.h"

namespace BT
{
  class SequenceNode : public BehaviourTree::GenericNode
  {
    typedef BehaviourTree::EvaluationContext EvaluationContext;
  public:
    SequenceNode() :
      nextChildIndex_(0)
    { }
    virtual ~SequenceNode() {}

    virtual std::string name() { return "SequenceNode"; }
    virtual EvaluationResult tick(EvaluationContext& context) override;
    virtual void reset() override;
  private:
    std::size_t nextChildIndex_;
  };
}

#endif // _SEQUENCENODE_H