#include "BehaviourTree.h"

#include <Unit.h>
#include "Game.h"

#include "SequenceNode.h"
#include "ConditionNode.h"
#include "EchoCommand.h"
#include <algorithm>

using namespace BT;
using namespace std;


BehaviourTree::EvaluationContext::EvaluationContext(springai::OOAICallback* callback, const std::string& instanceId)
	: callback_(callback), roleUnits_(), allUnits_(), instanceId_(instanceId), currentUnits_(&allUnits_), currentRole_(ALL_ROLES), breakpoints_() {
}

bool BehaviourTree::EvaluationContext::removeUnits(const std::vector<springai::Unit*>& units) {
	bool isAltered = false;
	auto shouldBeRemoved = [&](springai::Unit* u) {
		bool result = std::find_if(units.begin(), units.end(), [&](springai::Unit* o) { return u->GetUnitId() == o->GetUnitId(); }) != units.end();
		isAltered |= result;
		return result;
	};
	allUnits_.erase(std::remove_if(allUnits_.begin(), allUnits_.end(), shouldBeRemoved), allUnits_.end());
	for (auto& role : roleUnits_) {
		role.erase(std::remove_if(role.begin(), role.end(), shouldBeRemoved), role.end());
	}

	return isAltered;
}
void BehaviourTree::EvaluationContext::setUnits(int roleId, const std::vector<springai::Unit*>& units) {
	if (roleId >= roleUnits_.size()) {
		roleUnits_.resize(roleId + 1);
	}
	auto& role = roleUnits_[roleId];
	removeUnits(role);
	role = units;
	allUnits_.insert(allUnits_.end(), units.begin(), units.end());
}
void BehaviourTree::EvaluationContext::setActiveRole(int roleId) {
	currentRole_ = roleId;
	if (roleId == ALL_ROLES) {
		currentUnits_ = &allUnits_;
	}	else {
		if (roleId >= roleUnits_.size()) {
			roleUnits_.resize(roleId + 1);
		}
		currentUnits_ = &roleUnits_[roleId];
	}
}
void BehaviourTree::EvaluationContext::clear() {
	breakpoints_.clear();
	allUnits_.clear();
	roleUnits_.clear();
	reset();
}
void BehaviourTree::EvaluationContext::reset() {
	for (auto currentIt = currentlyStopped.begin(); currentIt != currentlyStopped.end(); ++currentIt)
		(*currentIt)->reset(*this);
	for (auto currentIt = currentlyRunning.begin(); currentIt != currentlyRunning.end(); ++currentIt)
		(*currentIt)->reset(*this);

	currentlyStopped.clear();
	previouslyRunning.clear();
	currentlyRunning.clear();
	currentlyFinished.clear();
}

void BehaviourTree::EvaluationContext::initialize() {
	std::swap(previouslyRunning, currentlyRunning);
	currentlyRunning.clear();
	currentlyFinished.clear();

	// remove dead units
	auto isUnitDead = [](springai::Unit* u) { return u->GetHealth() <= 0; };
	for (auto& role : roleUnits_) {
		role.erase(std::remove_if(role.begin(), role.end(), isUnitDead), role.end());
	}
	allUnits_.erase(std::remove_if(allUnits_.begin(), allUnits_.end(), isUnitDead), allUnits_.end());

	setActiveRole(ALL_ROLES);
}
void BehaviourTree::EvaluationContext::stoppedInitialize() {
	currentlyStopped.clear();
}

bool BehaviourTree::EvaluationContext::setBreakpoint(const std::string& nodeId) {
	return breakpoints_.insert(nodeId).second;
}
bool BehaviourTree::EvaluationContext::removeBreakpoint(const std::string& nodeId) {
	return breakpoints_.erase(nodeId) > 0;
}

EvaluationResult BehaviourTree::EvaluationContext::tickNode(Node* node) {
	//callback_->GetGame()->SendTextMessage((node == nullptr ? "NULL" : node->name().c_str()), 0);

	if (node == nullptr) // prevent attempts to tick a non-existant node/branch
		return btFailure;

	EvaluationResult result;
	if (node->canBreak() && breakpoints_.find(node->id()) != breakpoints_.end())
		result = node->stopAtEntry();
	else
		result = node->tick(*this);

	switch (result)
	{
	case btRunning:
		currentlyRunning.push_back(node);
		break;
	case btSuccess:
	case btFailure:
		currentlyFinished.push_back(std::make_pair(node, result));
		break;
	case btBreakpoint:
		currentlyStopped.push_back(node);
		break;
	default:
		// unknown result
		break;
	}

	return result;
}

void BehaviourTree::EvaluationContext::finalize() {
	/*
	for (auto finished : currentlyFinished) {
		finished.first->enableBreak();
	}
	*/

	for (auto previousIt = previouslyRunning.begin(); previousIt != previouslyRunning.end(); ++previousIt) {
		bool noLongerRunning = true;
		for (auto currentIt = currentlyRunning.begin(); noLongerRunning && currentIt != currentlyRunning.end(); ++currentIt)
			if (*currentIt == *previousIt)
				noLongerRunning = false;

		if (noLongerRunning) {
			(*previousIt)->reset(*this);
			//(*previousIt)->enableBreak();
		}
	}

	setActiveRole(ALL_ROLES);
}


void BehaviourTree::Node::reset(const EvaluationContext& context) {
	stoppedAt_ = 0;

	for (auto it = children_.begin(); it != children_.end(); ++it) {
		if(*it != nullptr) {
			(*it)->reset(context);
		}
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
	: breakpointReached_(false) {
}

void BehaviourTree::reset(EvaluationContext& context) {
	context.reset();
}
bool BehaviourTree::tick(EvaluationContext& context) {
	if (breakpointReached_) {
		context.stoppedInitialize();
		breakpointReached_ = false;
	}	else {
		context.initialize();
	}

	EvaluationResult finalResult = context.tickNode(root_.get());

	if (finalResult == btBreakpoint) {
		breakpointReached_ = true;
		// we do not finalize, as we are also not going to initialize
		return false;
	}	else {
		context.finalize();
		return true;
	}
}
