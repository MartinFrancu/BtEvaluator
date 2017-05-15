#include "SubtreeNode.h"

#include "Lua.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace BT;

SubtreeNode::SubtreeNode(const std::string& id, springai::OOAICallback* callback, std::string projectSwitch, nlohmann::json parameters)
	: BehaviourTree::UnaryNode(id), parameters_(parameters), lua_(callback->GetLua()), subtreeStarted_(false), projectSwitch_(projectSwitch)
{
	lua_->CallUI(("BETS CREATE_SUBTREE " + (json{ { "id", id },{ "parameter", parameters },{ "project", projectSwitch } }).dump()).c_str(), -1);
}

EvaluationResult SubtreeNode::tick(EvaluationContext& context)
{
	if (subtreeStarted_)
	{
		if (lua_->CallUI(("BETS ENTER_SUBTREE " + (json{ { "id", id() },{ "treeId", context.treeInstanceId() },{ "project", projectSwitch_ } }).dump()).c_str(), -1) == "F")
			return btFailure;
	}
	else
	{
		if (lua_->CallUI(("BETS START_SUBTREE " + (json{ { "id", id() },{ "treeId", context.treeInstanceId() },{ "parameter", parameters_ },{ "project", projectSwitch_ }, { "roleId", context.activeRole() } }).dump()).c_str(), -1) == "F")
			return btFailure;
		subtreeStarted_ = true;
	}

	EvaluationResult result = context.tickNode(child());
	if (result == btFailure || result == btSuccess)
		subtreeStarted_ = false;

	if (lua_->CallUI(("BETS EXIT_SUBTREE " + (json{ { "id", id() },{ "treeId", context.treeInstanceId() } }).dump()).c_str(), -1) == "F")
		return btFailure;

	return keep(result);
}

void SubtreeNode::reset(const EvaluationContext& context)
{
	Node::reset(context);
	subtreeStarted_ = false;
}


std::vector<BehaviourTree::ParameterDefinition> SubtreeNode::Factory::parameters() const {
	return{
		BehaviourTree::ParameterDefinition{
		"var",
		"string",
		"editBox",
		"x"
	},
		BehaviourTree::ParameterDefinition{
		"value",
		"longString",
		"editBox",
		"1"
	}
	};
}
std::unique_ptr<BehaviourTree::UnaryNode> SubtreeNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	json paramJson = json::object();

	std::string variableName = "x";
	auto variableIterator = parameters.find("var");
	if (variableIterator != parameters.end())
		variableName = variableIterator->second.asString();

	auto valueIterator = parameters.find("value");
	if (valueIterator != parameters.end())
		paramJson[variableName] = valueIterator->second.jsonValue;

	return std::unique_ptr<BehaviourTree::UnaryNode>(
		new SubtreeNode(id, callback_, "", paramJson)
	);
}

std::unique_ptr<BehaviourTree::UnaryNode> SubtreeNode::ReferenceFactory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
) const {
	std::string projectSwitch;
	json paramJson = json::object();

	for (auto it = parameters.begin(); it != parameters.end(); ++it) {
		if (it->first == "project")
			projectSwitch = it->second.asString();
		else if(it->first.substr(0, 4) == "ref_")
			paramJson[it->first.substr(4)] = it->second.jsonValue;
		// else // ignore all other parameters
	}

	return std::unique_ptr<BehaviourTree::UnaryNode>(
		new SubtreeNode(id, callback_, projectSwitch, paramJson)
		);
}