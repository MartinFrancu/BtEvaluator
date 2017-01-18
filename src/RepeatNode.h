#ifndef _REPEATNODE_H
#define _REPEATNODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT {
	class RepeatNode : public BehaviourTree::UnaryNode {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		RepeatNode(const std::string& id) :
			UnaryNode(id) {}
		virtual ~RepeatNode() {}

		std::string name() override { return "RepeatNode"; }
		EvaluationResult tick(EvaluationContext& context) override;

		class Factory : public UnaryNode::Factory {
		private:
			typedef BehaviourTree::ChildDefinition ChildDefinition;
		public:
			Factory() : UnaryNode::Factory() {}

			std::string typeName() const override { return "repeat"; }
		protected:
			virtual ChildDefinition child() const override { return ChildDefinition(); }

			std::unique_ptr<UnaryNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;
		};
	};
}

#endif // _REPEATNODE_H