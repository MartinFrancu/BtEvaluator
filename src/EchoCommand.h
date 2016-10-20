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
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}
			virtual std::string typeName() const { return "echo"; }
		protected:
			virtual std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const;
		};
	};
}

#endif