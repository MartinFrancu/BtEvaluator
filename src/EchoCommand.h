#ifndef ECHOCOMMAND_H
#define ECHOCOMMAND_H

#include "SpringCommand.h"

namespace BT {
	class EchoCommand : public SpringCommand {
  private:
		std::string message;
	public:
		explicit EchoCommand(const std::string& id, springai::OOAICallback* callback, const std::string& message)
			: SpringCommand(id, callback), message(message) {}
		~EchoCommand() {}

    virtual std::string name() { return "EchoCommand"; }
    virtual EvaluationResult execute(const std::vector<springai::Unit*> units) override;

		class Factory : public SpringCommand::Factory {
		private:
			typedef BehaviourTree::ParameterDefinition ParameterDefinition;
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}
			virtual std::string typeName() const { return "echo"; }
			virtual std::vector<ParameterDefinition> parameters() const;
		protected:
			virtual std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::string& treeInstanceId,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const;
		};
	};
}

#endif