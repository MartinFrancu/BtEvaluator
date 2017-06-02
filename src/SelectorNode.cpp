#include "SelectorNode.h"
#include "ActiveSelectorNode.h"

using namespace BT;

BT::EvaluationResult BT::SelectorNode::tick(EvaluationContext& context) {
	for (; nextChildIndex_ < count(); ++nextChildIndex_) {
		Node* currentChild = child(nextChildIndex_);
		EvaluationResult childResult = context.tickNode(currentChild);
		if (childResult == btBreakpoint)
			return stopAt(nextChildIndex_ + 1);

		if (childResult == btRunning)
			return notStopped(btRunning);
		else if (childResult == btSuccess) {
			reset(context);
			return btSuccess;
		}
	}
	reset(context);
	return btFailure;
}

void BT::SelectorNode::reset(const EvaluationContext& context) {
	nextChildIndex_ = 0;
	Node::reset(context);
}

std::vector<SelectorNode::Factory::ParameterDefinition> SelectorNode::Factory::parameters() const {
	return{
		ParameterDefinition{
			"active",
			"expression",
			"checkBox",
			"true"
		}
	};
}

std::unique_ptr<BehaviourTree::GenericNode> SelectorNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
) const {
	bool active = false;
	auto activeIt = parameters.find("active");
	if (activeIt != parameters.end())
		active = activeIt->second.asBoolean();
	if (active) {
		return std::unique_ptr<ActiveSelectorNode>(new ActiveSelectorNode(id));
	} else {
		return std::unique_ptr<SelectorNode>(new SelectorNode(id));
	}
}

std::string SelectorNode::Factory::tooltip() const {
	return
		"Selector node executes all the children sequentially from top to bottom. "
		"It will stop executing its children when one of the children succeeds. In that case, the Selector succeeds. "
		"If all the Selector's children fail, then the Selector fails. "
		"If active is selected, it switches to the higher priority children whenever possible. " 
		"Otherwise the selector keeps running the selected child until it finishes execution.";
}