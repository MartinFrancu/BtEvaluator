#ifndef ECHOCOMMAND_H
#define ECHOCOMMAND_H

#include "SpringCommand.h"

namespace BT {
	class EchoCommand : public SpringCommand {
  private:
		std::string message;
	public:
		explicit EchoCommand(springai::OOAICallback* callback, std::string message) : SpringCommand(callback), message(message) {}
		~EchoCommand() {}

    virtual std::string name() { return "EchoCommand"; }
    virtual EvaluationResult execute(const std::vector<springai::Unit*> units) override;
	};
}

#endif