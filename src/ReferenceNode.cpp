#include "ReferenceNode.h"

#include "Lua.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace BT;

ReferenceNode::ReferenceNode(const std::string& id, springai::OOAICallback* callback, nlohmann::json parameters)
	: BehaviourTree::UnaryNode(id), parameters_(parameters), lua_(callback->GetLua()), subtreeStarted_(false)
{
	lua_->CallUI(("BETS CREATE_SUBTREE " + (json{ { "id", id },{ "parameter", parameters } }).dump()).c_str(), -1);
}

EvaluationResult ReferenceNode::tick(EvaluationContext& context)
{
	if(subtreeStarted_)
		lua_->CallUI(("BETS ENTER_SUBTREE " + (json{ { "id", id() } }).dump()).c_str(), -1);
	else
	{
		lua_->CallUI(("BETS START_SUBTREE " + (json{ { "id", id() },{ "parameter", parameters_ } }).dump()).c_str(), -1);
		subtreeStarted_ = true;
	}

	EvaluationResult result = context.tickNode(child());

	lua_->CallUI(("BETS EXIT_SUBTREE " + (json{ { "id", id() } }).dump()).c_str(), -1);

	return keep(result);
}

void ReferenceNode::reset(const EvaluationContext& context)
{
	Node::reset(context);
	subtreeStarted_ = false;
}



std::unique_ptr<BehaviourTree::UnaryNode> ReferenceNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	json paramJson;

	for (auto it = parameters.begin(); it != parameters.end(); ++it) {
		paramJson[it->first] = it->second.jsonValue;
	}

	return std::unique_ptr<BehaviourTree::UnaryNode>(
		new ReferenceNode(id, callback_, paramJson)
	);
}