#ifndef COMMAND_H
#define COMMAND_H
#include <vector>
#include <OOAICallback.h>
#include "BehaviourTree.h"

namespace BT {

	class SpringCommand : public BehaviourTree::LeafNode {
    typedef BehaviourTree::EvaluationContext EvaluationContext;
  protected:
		springai::OOAICallback* callback;
	public:
    SpringCommand(springai::OOAICallback* callback) : callback(callback) {}

    virtual EvaluationResult tick(EvaluationContext& context) override { return execute(); }

		virtual EvaluationResult execute() = 0;
	};
}
#endif