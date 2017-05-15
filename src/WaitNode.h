#ifndef WAITNODE_H
#define WAITNODE_H

#include "SpringCommand.h"

namespace BT {
	class WaitNode : public SpringCommand {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	private:
		std::size_t counter_;
		std::size_t tickCount_;
	public:
		explicit WaitNode(const std::string& id, springai::OOAICallback* callback, std::size_t tickCount)
			: SpringCommand(id, callback), counter_(0), tickCount_(tickCount) {}
		~WaitNode() {}

		std::string name() override { return "WaitNode"; }
		EvaluationResult execute(const EvaluationContext& context) override;
		void reset(const EvaluationContext& context) override;

		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}
			std::string typeName() const override { return "wait"; }
			std::vector<BehaviourTree::ParameterDefinition> parameters() const override;
		protected:
			std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;

            std::string tooltip() const override;
		};
	};
}

#endif // WAITNODE_H