#ifndef _SWITCHNODE_H
#define _SWITCHNODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT
{
  class SwitchNode : public BehaviourTree::GenericNode
  {
    typedef BehaviourTree::EvaluationContext EvaluationContext;
  public:
    SwitchNode(const std::string& id, EvaluationResult waitFor) :
      GenericNode(id), waitFor_(waitFor)
    { }
    virtual ~SwitchNode() {}

    std::string name() override { return "SwitchNode"; }
    EvaluationResult tick(EvaluationContext& context) override;
    void reset(const EvaluationContext& context) override;
	
		class Factory : public GenericNode::Factory {
		public:
			Factory() : GenericNode::Factory() {}

			std::string typeName() const override { return "switch"; }
			std::vector<BehaviourTree::ParameterDefinition> parameters() const override;
		protected:
			std::unique_ptr<GenericNode> createNode(
				const std::string& id, 
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const override;
		};
	private:
		EvaluationResult waitFor_;
		std::vector<bool> childFinished_;
  };
}

#endif // _SWITCHNODE_H