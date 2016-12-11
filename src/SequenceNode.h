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

	std::string name() override { return "SequenceNode"; }
    EvaluationResult tick(EvaluationContext& context) override;
    void reset() override;
	
		class Factory : public GenericNode::Factory {
		public:
			Factory() : GenericNode::Factory() {}

			std::string typeName() const override { return "sequence"; }
		protected:
			std::unique_ptr<GenericNode> createNode(
				const std::string& id, 
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const override;
		};
	private:
    std::size_t nextChildIndex_;
  };
}

#endif // _SEQUENCENODE_H