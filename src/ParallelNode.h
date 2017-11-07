#ifndef _PARALLELNODE_H
#define _PARALLELNODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT {

	/**
	* Node that ticks all its children in parallel.
	*/
	class ParallelNode : public BehaviourTree::GenericNode {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		ParallelNode(const std::string& id, EvaluationResult waitFor) :
			GenericNode(id), waitFor_(waitFor) {}
		virtual ~ParallelNode() {}

		std::string name() override { return "ParallelNode"; }
		EvaluationResult tick(EvaluationContext& context) override;
		void reset(const EvaluationContext& context) override;

		class Factory : public GenericNode::Factory {
		public:
			Factory() : GenericNode::Factory() {}

			std::string typeName() const override { return "parallel"; }
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

#endif // _PARALLELNODE_H