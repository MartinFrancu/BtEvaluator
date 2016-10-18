#include "BehaviourTree.h"

#include "Game.h"

#include "json.hpp"
#include "SequenceNode.h"
#include "ConditionNode.h"
#include "EchoCommand.h"

using json = nlohmann::json;

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
json BehaviourTree::Node::getNodeDefinition() {
	auto childDefinitions(getChildDefinitions());

	vector<json> childDefsJson;

	for (const auto& childDef : childDefinitions)
	{
		childDefsJson.push_back(childDef.getJson());
	}

	vector<string> serializedParams;
	for (auto& param : parameters) {
		serializedParams.push_back(param.serialize());
	}

	return json({
		{ "name", name() },
		{ "children", childDefsJson },
		{ "tooltip", tooltip() },
		{ "parameters", serializedParams },
		{ "defaultWidth",defaultWidth()},
		{ "defaultHeight", defaultHeight() }
	});
}

void BehaviourTree::Node::connectTo(Node* node, std::unique_ptr<Node>& link) {
	node->parent_ = this;
	link.reset(node);
}


void BehaviourTree::GenericNode::add(Node* node) {
	children_.emplace_back(nullptr);
	connectTo(node, children_.back());
}


BehaviourTree::BehaviourTree()
{
	// TODO add all node types
	allNodeTypes.push_back(unique_ptr<SequenceNode>(new SequenceNode()));
	allNodeTypes.push_back(unique_ptr<ConditionNode>(new ConditionNode(false)));
}

void BehaviourTree::tick(EvaluationContext& context) {
	context.initialize();
	if (root_ != nullptr)
		context.tickNode(root_.get());
	context.finalize();
}

std::string BehaviourTree::getAllNodeDefinitions() const
{
	vector<json> nodesJsons;
	for (auto& node : allNodeTypes)
	{
		nodesJsons.push_back(node->getNodeDefinition());
	}
	return json(nodesJsons).dump();
}

string BehaviourTree::NodeParameter::serialize() const {
	return json({ {"name", name}, { "variableType", variableType }, {"defaultValue", defaultValue }, { "componentType", componentType }
	}).dump();
}
