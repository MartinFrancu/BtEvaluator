#ifndef ECHOCOMMAND_H
#define ECHOCOMMAND_H

#include "SpringCommand.h"

namespace BT {
	class EchoCommand : public SpringCommand {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	private:
		std::string message;
	public:
		explicit EchoCommand(const std::string& id, springai::OOAICallback* callback, const std::string& message)
			: SpringCommand(id, callback), message(message) {}
		~EchoCommand() {}

		std::string name() override { return "EchoCommand"; }
		EvaluationResult execute(const EvaluationContext& context) override;

		class Factory : public SpringCommand::Factory {
		private:
			typedef BehaviourTree::ParameterDefinition ParameterDefinition;
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}

			std::string typeName() const override { return "echo"; }

			std::vector<ParameterDefinition> parameters() const override;
		protected:
			std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;
		};
	};
}

#endif