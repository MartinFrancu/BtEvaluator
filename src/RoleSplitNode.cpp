#include "RoleSplitNode.h"

using namespace BT;

EvaluationResult RoleSplitNode::tick(EvaluationContext& context)
{
	if (context.activeRole() != EvaluationContext::ALL_ROLES)	{
		int childIndex = context.activeRole();
		Node* currentChild = child(childIndex);
		return context.tickNode(currentChild);
	}	else {
		if(childFinished_.size() < count())
			childFinished_.resize(count());

		std::size_t runningCount = 0;
		for (int childIndex = 0; childIndex < count(); ++childIndex)
		{
			if (!childFinished_[childIndex])
			{
				Node* currentChild = child(childIndex);
				context.setActiveRole(childIndex);
				EvaluationResult childResult = context.tickNode(currentChild);
				if (childResult == btRunning)
					++runningCount;
				else
				{
					childFinished_[childIndex] = true;
					if (childResult != waitFor_) {
						reset(context);
						context.setActiveRole(EvaluationContext::ALL_ROLES);
						return childResult;
					}
				}
			}
		}
		context.setActiveRole(EvaluationContext::ALL_ROLES);

		if (runningCount == 0)
		{
			reset(context);
			return waitFor_;
		}
		else {
			return btRunning;
		}
	}
}

void RoleSplitNode::reset(const EvaluationContext& context)
{
	Node::reset(context);
	childFinished_.clear();
}


std::vector<BehaviourTree::ParameterDefinition> RoleSplitNode::Factory::parameters() const
{
	return{
		BehaviourTree::ParameterDefinition {
			"mode",
			"all-success,any-success,any-result", //enum(all-success, one-success, any-result)",
			"comboBox", 
			"all-success"
		}
	};
}

std::unique_ptr<BehaviourTree::GenericNode> RoleSplitNode::Factory::createNode(
	const std::string& id,
	const std::map<std::string, ParameterValuePlaceholder>& parameters
	) const {
	EvaluationResult waitFor = btFailure;

	auto it = parameters.find("mode");
	if (it != parameters.end()) {
		std::string s = it->second.asString();
		if (s == "all-success")
			waitFor = btSuccess;
		else if (s == "any-success")
			waitFor = btFailure;
		else if (s == "any-result")
			waitFor = btUndefined;
	}

	return std::unique_ptr<BehaviourTree::GenericNode>(
		new RoleSplitNode(id, waitFor)
	);
}
