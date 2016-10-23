#ifndef _BEHAVIOURTREEFACTORY_H
#define _BEHAVIOURTREEFACTORY_H

#include "BehaviourTree.h"

namespace BT {
	class BehaviourTree::Node::Factory {
	public:
		struct ParameterValuePlaceholder {
			std::string value;

			std::string asString(std::string defaultValue = "") const;
			int asInteger(int defaultValue = 0) const;
			bool asBoolean(bool defaultValue = false) const;
		};

		virtual ~Factory() {}

		virtual std::unique_ptr<Node> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters,
			std::vector<std::unique_ptr<Node>>& children
			) const = 0;

		virtual std::string typeName() const = 0;
		virtual int defaultHeight() const { return 60; };
		virtual int defaultWidth() const { return 110; };
		virtual std::string tooltip() const { return "Node '" + typeName() + "'"; };
		virtual std::vector<ParameterDefinition> parameters() const { return{}; }
		virtual bool unlimitedChildren() const = 0;
		virtual std::vector<ChildDefinition> children() const = 0;
	};

	class BehaviourTree::LeafNode::Factory : public Node::Factory {
	public:
		virtual std::unique_ptr<Node> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters,
			std::vector<std::unique_ptr<Node>>& children
			) const;
		virtual bool unlimitedChildren() const { return false; }
		virtual std::vector<ChildDefinition> children() const { return {}; }

	protected:
		virtual std::unique_ptr<LeafNode> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const = 0;
	};
	class BehaviourTree::UnaryNode::Factory : public Node::Factory {
	public:
		virtual std::unique_ptr<Node> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters,
			std::vector<std::unique_ptr<Node>>& children
			) const;
		virtual bool unlimitedChildren() const { return false; }
		virtual std::vector<ChildDefinition> children() const { return{ child() }; }

	protected:
		virtual ChildDefinition child() const = 0;
		virtual std::unique_ptr<UnaryNode> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const = 0;
	};
	class BehaviourTree::BinaryNode::Factory : public Node::Factory {
	public:
		virtual std::unique_ptr<Node> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters,
			std::vector<std::unique_ptr<Node>>& children
			) const;
		virtual bool unlimitedChildren() const { return false; }
		virtual std::vector<ChildDefinition> children() const { return{ leftChild(), rightChild() }; }

	protected:
		virtual ChildDefinition leftChild() const = 0;
		virtual ChildDefinition rightChild() const = 0;
		virtual std::unique_ptr<BinaryNode> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const = 0;
	};
	class BehaviourTree::TernaryNode::Factory : public Node::Factory {
	public:
		virtual std::unique_ptr<Node> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters,
			std::vector<std::unique_ptr<Node>>& children
			) const;
		virtual bool unlimitedChildren() const { return false; }
		virtual std::vector<ChildDefinition> children() const { return{ firstChild(), secondChild(), thirdChild() }; }

	protected:
		virtual ChildDefinition firstChild() const = 0;
		virtual ChildDefinition secondChild() const = 0;
		virtual ChildDefinition thirdChild() const = 0;
		virtual std::unique_ptr<TernaryNode> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const = 0;
	};
	class BehaviourTree::GenericNode::Factory : public Node::Factory {
	public:
		virtual std::unique_ptr<Node> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters,
			std::vector<std::unique_ptr<Node>>& children
			) const;
		virtual bool unlimitedChildren() const { return true; }
		virtual std::vector<ChildDefinition> children() const { return{}; }

	protected:
		virtual std::unique_ptr<GenericNode> createNode(
			const std::string& id,
			const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const = 0;
	};
}

#endif // _BEHAVIOURTREEFACTORY_H