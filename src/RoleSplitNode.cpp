#include "RoleSplitNode.h"

using namespace BT;

EvaluationResult RoleSplitNode::tick(EvaluationContext& context)
{
	if (context.activeRole() != EvaluationContext::ALL_ROLES)	{
		int childIndex = context.activeRole();
		Node* currentChild = child(childIndex);
		return keep(context.tickNode(currentChild));
	}	else {
		if(childFinished_.size() < count())
			childFinished_.resize(count());

		if(stoppedAt() == 0)
			runningCount_ = 0;
		for (int childIndex = stoppedAt(); childIndex < count(); ++childIndex)
		{
			if (!childFinished_[childIndex])
			{
				Node* currentChild = child(childIndex);
				context.setActiveRole(childIndex);
				EvaluationResult childResult = context.tickNode(currentChild);
				if (childResult == btBreakpoint)
				{
					context.setActiveRole(EvaluationContext::ALL_ROLES);
					return stopAt(childIndex);
				}

				if (childResult == btRunning)
					++runningCount_;
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

		if (runningCount_ == 0)
		{
			reset(context);
			return waitFor_;
		}
		else {
			return notStopped(btRunning);
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

std::string RoleSplitNode::Factory::tooltip() const {
	return 
		"RoleSplitNode can have arbitrary children. "
		"It splits the units according to the roles of the current tree. "
		"It means that the first children will be executed only with units associated to the first role. "
		"The second will gets executed with units associated to the second role, and so on. "
		"All the children are executed in parallel and the result state of the RoleSplitNode is determined by its 'mode'. ";
}