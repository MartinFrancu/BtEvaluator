#include "ActiveSelectorNode.h"

BT::EvaluationResult BT::ActiveSelectorNode::tick(EvaluationContext& context) {
	for (int i = 0; i < count(); ++i) {
		Node* currentChild = child(i);
		EvaluationResult childResult = context.tickNode(currentChild);
		if (childResult == btBreakpoint)
			return stopAt(i + 1);
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

std::unique_ptr<BT::BehaviourTree::GenericNode> BT::ActiveSelectorNode::Factory::createNode(const std::string& id, const std::map<std::string, ParameterValuePlaceholder>& parameters) const {
	return std::unique_ptr<ActiveSelectorNode>(new ActiveSelectorNode(id));
}

std::string BT::ActiveSelectorNode::Factory::tooltip() const {
	return
		"Active selector node differs from a normal selector in that it switches to the higher priority children whenever possible. (Even when the lower priority child hasn't finished execution.)";
}
