#ifndef _SELECTORNODE_H
#define _SELECTORNODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT {
	class SelectorNode : public BehaviourTree::GenericNode {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		SelectorNode(const std::string& id) : GenericNode(id), nextChildIndex_(0) {}
		virtual ~SelectorNode() {}
		std::string name() override { return "SelectorNode"; }
		EvaluationResult tick(EvaluationContext& context) override;
		void reset(const EvaluationContext& context) override;


		class Factory : public GenericNode::Factory {
			typedef BehaviourTree::ParameterDefinition ParameterDefinition;
		public:
			Factory() : GenericNode::Factory() {}

			std::string typeName() const override { return "selector"; }

			std::vector<ParameterDefinition> parameters() const override;
		protected:
			std::unique_ptr<GenericNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;

			std::string tooltip() const override;
		};
	private:
		std::size_t nextChildIndex_;
	};

}

#endif
