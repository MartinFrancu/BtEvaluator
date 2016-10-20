#include "BehaviourTree.h"

#include "Game.h"

#include "SequenceNode.h"
#include "ConditionNode.h"
#include "EchoCommand.h"

using namespace BT;
using namespace std;


void BehaviourTree::EvaluationContext::initialize() {
	std::swap(previouslyRunning, currentlyRunning);
	currentlyRunning.clear();
}

EvaluationResult BehaviourTree::EvaluationContext::tickNode(Node* node) {
	//callback_->GetGame()->SendTextMessage((node == nullptr ? "NULL" : node->name().c_str()), 0);

	EvaluationResult result = node->tick(*this);
	if (result == btRunning)
		currentlyRunning.push_back(node);
	return result;
}

void BehaviourTree::EvaluationContext::finalize() {
	for (auto previousIt = previouslyRunning.begin(); previousIt != previouslyRunning.end(); ++previousIt) {
		bool noLongerRunning = true;
		for (auto currentIt = currentlyRunning.begin(); noLongerRunning && currentIt != currentlyRunning.end(); ++currentIt)
			if (*currentIt == *previousIt)
				noLongerRunning = false;

		if (noLongerRunning)
			(*previousIt)->reset();
	}
}


void BehaviourTree::Node::connectTo(Node* node, std::unique_ptr<Node>& link) {
	if(node != 0)
		node->parent_ = this;
	link.reset(node);
}


void BehaviourTree::GenericNode::add(Node* node) {
	children_.emplace_back(nullptr);
	connectTo(node, children_.back());
}


BehaviourTree::BehaviourTree()
{

}

void BehaviourTree::tick(EvaluationContext& context) {
	context.initialize();
	if (root_ != nullptr)
		context.tickNode(root_.get());
	context.finalize();
}
