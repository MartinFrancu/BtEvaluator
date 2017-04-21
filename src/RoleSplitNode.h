#ifndef _SWITCHNODE_H
#define _SWITCHNODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT
{
  class RoleSplitNode : public BehaviourTree::GenericNode
  {
    typedef BehaviourTree::EvaluationContext EvaluationContext;
  public:
    RoleSplitNode(const std::string& id, EvaluationResult waitFor) :
      GenericNode(id), waitFor_(waitFor)
    { }
    virtual ~RoleSplitNode() {}

    std::string name() override { return "RoleSplitNode"; }
    EvaluationResult tick(EvaluationContext& context) override;
    void reset(const EvaluationContext& context) override;
	
		class Factory : public GenericNode::Factory {
		public:
			Factory() : GenericNode::Factory() {}

			std::string typeName() const override { return "roleSplit"; }
			std::vector<BehaviourTree::ParameterDefinition> parameters() const override;
		protected:
			std::unique_ptr<GenericNode> createNode(
				const std::string& id, 
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const override;
			std::string tooltip() const override;
		};
	private:
		EvaluationResult waitFor_;
		std::size_t runningCount_;
		std::vector<bool> childFinished_;
  };
}

#endif // _SWITCHNODE_H