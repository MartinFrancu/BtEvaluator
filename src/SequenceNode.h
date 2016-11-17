#ifndef _SEQUENCENODE_H
#define _SEQUENCENODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT
{
  class SequenceNode : public BehaviourTree::GenericNode
  {
    typedef BehaviourTree::EvaluationContext EvaluationContext;
  public:
    SequenceNode(const std::string& id) :
      GenericNode(id), nextChildIndex_(0)
    { }
    virtual ~SequenceNode() {}

    virtual std::string name() { return "SequenceNode"; }
    virtual EvaluationResult tick(EvaluationContext& context) override;
    virtual void reset() override;
	
		class Factory : public GenericNode::Factory {
		public:
			Factory() : GenericNode::Factory() {}
			virtual std::string typeName() const { return "sequence"; }
		protected:
			virtual std::unique_ptr<GenericNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const;
		};
	private:
    std::size_t nextChildIndex_;
  };
}

#endif // _SEQUENCENODE_H