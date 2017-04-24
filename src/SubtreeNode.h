#ifndef _SUBTREENODE_H
#define _SUBTREENODE_H

#include "BehaviourTree.h"
#include "BehaviourTreeFactory.h"

namespace BT {
	class SubtreeNode : public BehaviourTree::UnaryNode {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	public:
		SubtreeNode(const std::string& id, springai::OOAICallback* callback, nlohmann::json parameters = {});
		virtual ~SubtreeNode() {}

		std::string name() override { return "SubtreeNode"; }
		EvaluationResult tick(EvaluationContext& context) override;
		void reset(const EvaluationContext& context) override;

		class Factory : public UnaryNode::Factory {
		private:
			typedef BehaviourTree::ChildDefinition ChildDefinition;
			springai::OOAICallback* callback_;
		public:
			Factory(springai::OOAICallback* callback) : UnaryNode::Factory(), callback_(callback) {}

			std::string typeName() const override { return "subtree"; }
			std::vector<BehaviourTree::ParameterDefinition> parameters() const override;
		protected:
			virtual ChildDefinition child() const override { return ChildDefinition(); }

			std::unique_ptr<UnaryNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;
		};

		class ReferenceFactory : public UnaryNode::Factory {
		private:
			typedef BehaviourTree::ChildDefinition ChildDefinition;
			springai::OOAICallback* callback_;
		public:
			ReferenceFactory(springai::OOAICallback* callback) : UnaryNode::Factory(), callback_(callback) {}

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

#endif // _SUBTREENODE_H