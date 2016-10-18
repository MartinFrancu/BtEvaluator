#ifndef _BEHAVIOURTREE_H
#define _BEHAVIOURTREE_H

#include <OOAICallback.h>

#include <vector>
#include <memory>
#include <json.hpp>

namespace BT {

	enum EvaluationResult {
		btUndefined = 0,

		// final results
		btSuccess = 1,
		btFailure = 2,

		// intermediary result
		btRunning = 4
	};

	class BehaviourTree {
	public:
		class Node;

		class EvaluationContext {
		public:
			springai::OOAICallback* callback_;
			EvaluationContext(springai::OOAICallback* callback) : callback_(callback) {}

			void initialize();
			EvaluationResult tickNode(Node* node);
			void finalize();
		private:
			std::vector<Node*> currentlyRunning;
			std::vector<Node*> previouslyRunning;
		};

		struct NodeParameter {
			std::string name = "param";
			std::string variableType = "string";
			std::string defaultValue = "testVal";
			std::string componentType = "textBox";
			std::string serialize() const;
		};

		struct ChildDefinition {
			nlohmann::json getJson() const
			{
				return nlohmann::json();
			}
		};

		class Node {
		public:
			Node(int children = 0) : children_(children), parent_(nullptr) {}
			virtual ~Node() {}

			virtual std::string name() = 0;
			virtual EvaluationResult tick(EvaluationContext& context) = 0;
			virtual void reset() {} // reset can quite often do nothing

			nlohmann::json getNodeDefinition();
		protected:
			void connectTo(Node* node, std::unique_ptr<Node>& link);
			std::vector<std::unique_ptr<Node>> children_ = std::vector<std::unique_ptr<Node>>();
			std::vector<NodeParameter> parameters;

			// Node definition
			virtual int defaultHeight() { return 10; }; // TODO replace with real values
			virtual int defaultWidth() { return 20; };
			virtual std::string tooltip() { return "test tooltip"; };
			virtual std::vector<ChildDefinition> getChildDefinitions() const {
				return std::vector<ChildDefinition>(children_.size());
			}
		private:
			Node* parent_; // weak reference

		};

		class LeafNode : public Node {
		public:
			LeafNode() : Node(0) {}
		};

		class UnaryNode : public Node {
		public:
			UnaryNode() : Node(1) {}
			virtual ~UnaryNode() {}

			void setChild(Node* node) { connectTo(node, children_[0]); }

			Node* child() { return children_[0].get(); }
		private:
			//std::unique_ptr<Node> child_;
		};

		class BinaryNode : public Node {
		public:
			BinaryNode() : Node(2) {}
			virtual ~BinaryNode() {}

			void setChildren(Node* left, Node* right) { setLeftChild(left); setRightChild(right); }
			void setLeftChild(Node* node) { connectTo(node, children_[0]); }
			void setRightChild(Node* node) { connectTo(node, children_[1]); }

			Node* leftChild() { return children_[0].get(); }
			Node* rightChild() { return children_[1].get(); }
		private:
			// std::unique_ptr<Node> leftChild_;
			// std::unique_ptr<Node> rightChild_;
		};

		class TernaryNode : public Node {
		public:
			TernaryNode() : Node(3) {}
			virtual ~TernaryNode() {}

			void setChildren(Node* first, Node* second, Node* third) { setFirstChild(first); setSecondChild(second); setThirdChild(third); }
			void setFirstChild(Node* node) { connectTo(node, children_[0]); }
			void setSecondChild(Node* node) { connectTo(node, children_[1]); }
			void setThirdChild(Node* node) { connectTo(node, children_[2]); }

			Node* firstChild() { return children_[0].get(); }
			Node* secondChild() { return children_[1].get(); }
			Node* thirdChild() { return children_[2].get(); }
		private:
			/* std::unique_ptr<Node> firstChild_;
			 std::unique_ptr<Node> secondChild_;
			 std::unique_ptr<Node> thirdChild_;*/
		};

		class GenericNode : public Node {
		public:
			virtual ~GenericNode() {}

			void add(Node* node);
			std::size_t count() const {
				return children_.size();
			}
			Node* child(std::size_t i) { return children_[i].get(); }
		private:
			// std::vector<std::unique_ptr<Node>> children_;
		};

		BehaviourTree();

		void tick(EvaluationContext& context);

		void setRoot(Node* node) { root_.reset(node); }

		std::string getAllNodeDefinitions() const;

	private:
		std::unique_ptr<Node> root_;

		std::vector<std::unique_ptr<Node>> allNodeTypes ;
	};


}

#endif // _BEHAVIOURTREE_H

