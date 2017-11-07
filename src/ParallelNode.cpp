#include "ParallelNode.h"

using namespace BT;

EvaluationResult ParallelNode::tick(EvaluationContext& context)
{
	if (childFinished_.size() < count())
		childFinished_.resize(count());

	if (stoppedAt() <= 0)
		runningCount_ = 0;
	for (int childIndex = std::max(0u, stoppedAt()); childIndex < count(); ++childIndex)
	{
		if (!childFinished_[childIndex])
		{
			Node* currentChild = child(childIndex);
			EvaluationResult childResult = context.tickNode(currentChild);
			if (childResult == btBreakpoint)
				return stopAt(childIndex);

			if (childResult == btRunning)
				++runningCount_;
			else
			{
				childFinished_[childIndex] = true;
				if (childResult != waitFor_) {
					reset(context);
					return childResult;
				}
			}
		}
	}

	if (runningCount_ == 0)
	{
		reset(context);
		return waitFor_;
	}
	else {
		return notStopped(btRunning);
	}
}

void ParallelNode::reset(const EvaluationContext& context)
{
	Node::reset(context);
	childFinished_.clear();
}


std::vector<BehaviourTree::ParameterDefinition> ParallelNode::Factory::parameters() const
{
	return{
		BehaviourTree::ParameterDefinition{
		"mode",
		"all-success,any-success,any-result", //enum(all-success, one-success, any-result)",
		"comboBox",
		"all-success"
	}
	};
}

std::unique_ptr<BehaviourTree::GenericNode> ParallelNode::Factory::createNode(
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
		new ParallelNode(id, waitFor)
		);
}

std::string ParallelNode::Factory::tooltip() const {
	return
		"ParallelNode can have arbitrary children. "
		"All the children are executed in parallel and the result state of the ParallelNode is determined by its 'mode'. ";
}