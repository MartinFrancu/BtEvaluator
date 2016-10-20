#include "BehaviourTreeFactory.h"

#include <sstream>

using namespace BT;

std::string BehaviourTree::Node::Factory::ParameterValuePlaceholder::asString(std::string defaultValue) const {
	return value;
}
int BehaviourTree::Node::Factory::ParameterValuePlaceholder::asInteger(int defaultValue) const {
	int result;
	std::stringstream	ss(value);
	if (ss >> result)
		return result;

	return defaultValue;
}
bool BehaviourTree::Node::Factory::ParameterValuePlaceholder::asBoolean(bool defaultValue) const {
	if (value == "true" || value == "on" || value == "1")
		return true;
	else if (value == "false" || value == "off" || value == "0")
		return false;
	else
		return defaultValue;
}

std::unique_ptr<BehaviourTree::Node> BehaviourTree::LeafNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters,
	std::vector<std::unique_ptr<Node>>& children
	) const {
	if (children.size() > 0)
		throw "Invalid number of children.";

	auto node = createNode(id, parameters);
	return std::move(node);
}

std::unique_ptr<BehaviourTree::Node> BehaviourTree::UnaryNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters,
	std::vector<std::unique_ptr<Node>>& children
	) const {
	if (children.size() > 1)
		throw "Invalid number of children.";

	auto node = createNode(id, parameters);
	node->setChild(children[0].release());
	return std::move(node);
}

std::unique_ptr<BehaviourTree::Node> BehaviourTree::BinaryNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters,
	std::vector<std::unique_ptr<Node>>& children
	) const {
	if (children.size() > 2)
		throw "Invalid number of children.";

	auto node = createNode(id, parameters);
	node->setChildren(children[0].release(), children[1].release());
	return std::move(node);
}

std::unique_ptr<BehaviourTree::Node> BehaviourTree::TernaryNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters,
	std::vector<std::unique_ptr<Node>>& children
	) const {
	if (children.size() > 3)
		throw "Invalid number of children.";

	auto node = createNode(id, parameters);
	node->setChildren(children[0].release(), children[1].release(), children[2].release());
	return std::move(node);
}

std::unique_ptr<BehaviourTree::Node> BehaviourTree::GenericNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters,
	std::vector<std::unique_ptr<Node>>& children
	) const {
	auto node = createNode(id, parameters);
	for (auto& child : children)
		node->add(child.release());
	return std::move(node);
}
