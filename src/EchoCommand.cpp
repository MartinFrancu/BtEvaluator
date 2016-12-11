#include "EchoCommand.h"
#include "Game.h"

using namespace BT;

EvaluationResult EchoCommand::execute(const std::vector<springai::Unit*> units) {
	callback->GetGame()->SendTextMessage(("BT-Echo: " + message).c_str(), 0);
  return btSuccess;
}


std::vector<BehaviourTree::ParameterDefinition> EchoCommand::Factory::parameters() const
{
	return{
		ParameterDefinition {
			"message",
			"string",
			"editBox",
			"Echo message"
		}
	};
}

std::unique_ptr<BehaviourTree::LeafNode> EchoCommand::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	std::string message = "Echo message";

	auto messageIterator = parameters.find("message");
	if (messageIterator != parameters.end())
		message = messageIterator->second.asString();

	return std::unique_ptr<BehaviourTree::LeafNode>(
		new EchoCommand(id, callback_, message)
		);
}
