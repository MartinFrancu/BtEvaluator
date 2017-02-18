#ifndef _REFERENCENODE_H
#define _REFERENCENODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT {
	class ReferenceNode : public BehaviourTree::UnaryNode {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		ReferenceNode(const std::string& id, springai::OOAICallback* callback, nlohmann::json parameters = {});
		virtual ~ReferenceNode() {}

		std::string name() override { return "ReferenceNode"; }
		EvaluationResult tick(EvaluationContext& context) override;
		void reset(const EvaluationContext& context) override;

		class Factory : public UnaryNode::Factory {
		private:
			typedef BehaviourTree::ChildDefinition ChildDefinition;
			springai::OOAICallback* callback_;
		public:
			Factory(springai::OOAICallback* callback) : UnaryNode::Factory(), callback_(callback) {}

			std::string typeName() const override { return "reference"; }
			virtual bool isAvailableInNodepool() const override { return false; }
		protected:
			virtual ChildDefinition child() const override { return ChildDefinition(); }

			std::unique_ptr<UnaryNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;
		};
	private:
		bool subtreeStarted_;
		springai::Lua* lua_;
		nlohmann::json parameters_;
	};
}

#endif // _REFERENCENODE_H