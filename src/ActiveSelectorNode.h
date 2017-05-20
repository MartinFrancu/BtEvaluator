#ifndef _ACTIVESELECTORNODE_H
#define _ACTIVESELECTORNODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT {
	class ActiveSelectorNode : public BehaviourTree::GenericNode {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		ActiveSelectorNode(const std::string& id) : GenericNode(id) {}
		virtual ~ActiveSelectorNode() {}
		std::string name() override { return "ActiveSelectorNode"; }
		EvaluationResult tick(EvaluationContext& context) override;

		class Factory : public GenericNode::Factory {
		public:
			Factory() : GenericNode::Factory() {}

			std::string typeName() const override { return "active selector"; }
		protected:
			std::unique_ptr<GenericNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;

			std::string tooltip() const override;
		};
	};

}
#endif
