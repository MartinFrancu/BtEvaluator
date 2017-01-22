#ifndef _CONSTANTNODE_H
#define _CONSTANTNODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT {
	class ConstantNode : public BehaviourTree::LeafNode {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		ConstantNode(const std::string& id, EvaluationResult resultValue) :
			LeafNode(id), _resultValue(resultValue) {}
		virtual ~ConstantNode() {}

		std::string name() override { return "ConstantNode"; }
		EvaluationResult tick(EvaluationContext& context) override;

		class Factory : public LeafNode::Factory {
		public:
			Factory(EvaluationResult resultValue)
				: LeafNode::Factory(), _resultValue(resultValue) {}

			int defaultHeight() const override { return 30; };
			int defaultWidth() const override { return 80; };
			std::string typeName() const override { return nameOfEvaluationResult(_resultValue); }
		protected:
			std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;
		private:
			EvaluationResult _resultValue;
		};
	private:
		EvaluationResult _resultValue;
	};
}

#endif // _CONSTANTNODE_H