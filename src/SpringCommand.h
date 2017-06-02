#ifndef _SPRINGCOMMAND_H
#define _SPRINGCOMMAND_H

#include <vector>
#include <OOAICallback.h>
#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT {

	class SpringCommand : public BehaviourTree::LeafNode {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	protected:
		springai::OOAICallback* callback;
	public:
		SpringCommand(const std::string& id, springai::OOAICallback* callback) : LeafNode(id), callback(callback) {}

		EvaluationResult tick(EvaluationContext& context) override { 
			return notStopped(execute(context));
		}

		virtual EvaluationResult execute(const EvaluationContext& context) = 0;

		class Factory : public LeafNode::Factory {
		protected:
			springai::OOAICallback* callback_;
		public:
			Factory(springai::OOAICallback* callback) : callback_(callback) {}
		};
	};

}

#endif // _SPRINGCOMMAND_H