#ifndef _BEHAVIOURTREE_H
#define _BEHAVIOURTREE_H

#include <OOAICallback.h>

#include <vector>
#include <memory>

namespace BT {

	enum EvaluationResult {
		btUndefined = 0,

		// final results
		btSuccess = 1,
		btFailure = 2,

		// intermediary result
		btRunning = 4
	};
	inline const char* nameOfEvaluationResult(EvaluationResult result) {
		switch (result) {
		case btUndefined: return "undefined";
		case btSuccess: return "success";
		case btFailure: return "failure";
		case btRunning: return "running";
		default: return "unknown";
		}
	}


	class BehaviourTree {
	public:
		class Node;

    class EvaluationContext {
    public:
      springai::OOAICallback* callback_;
      EvaluationContext(springai::OOAICallback* callback)
        : callback_(callback), units_(callback->GetSelectedUnits())
      {
      }

      const std::vector<springai::Unit*> units() const { return units_; }
      void setUnits(const std::vector<springai::Unit*> units) { units_ = units; }

			const std::vector<std::pair<Node*, EvaluationResult>>& finished() { return currentlyFinished; }
			const std::vector<Node*>& running() { return currentlyRunning; }

      void initialize();
      EvaluationResult tickNode(Node* node);
      void finalize();
    private:
      std::vector<springai::Unit*> units_;
      std::vector<std::pair<Node*, EvaluationResult>> currentlyFinished;
			std::vector<Node*> currentlyRunning;
			std::vector<Node*> previouslyRunning;
    };

		struct ParameterDefinition {
			std::string name;
			std::string variableType;
			std::string componentType;
			std::string defaultValue;
    };
		struct ChildDefinition { };

		class Node {
		public:
      class Factory;

			Node(const std::string& id, int children = 0) : id_(id), children_(children), parent_(nullptr) {}
			virtual ~Node() {}

			const std::string& id() const { return id_; }

			virtual std::string name() = 0;
			virtual EvaluationResult tick(EvaluationContext& context) = 0;
			virtual void reset() {} // reset can quite often do nothing
		protected:
			void connectTo(Node* node, std::unique_ptr<Node>& link);
			std::vector<std::unique_ptr<Node>> children_ = std::vector<std::unique_ptr<Node>>();
		private:
			std::string id_;
			Node* parent_; // weak reference

		};

		class LeafNode : public Node {
		public:
			LeafNode(const std::string& id) : Node(id, 0) {}

			class Factory;
		};

		class UnaryNode : public Node {
		public:
			UnaryNode(const std::string& id) : Node(id, 1) {}
			virtual ~UnaryNode() {}

			class Factory;

			void setChild(Node* node) { connectTo(node, children_[0]); }

			Node* child() { return children_[0].get(); }
		};

		class BinaryNode : public Node {
		public:
			BinaryNode(const std::string& id) : Node(id, 2) {}
			virtual ~BinaryNode() {}

			class Factory;

			void setChildren(Node* left, Node* right) { setLeftChild(left); setRightChild(right); }
			void setLeftChild(Node* node) { connectTo(node, children_[0]); }
			void setRightChild(Node* node) { connectTo(node, children_[1]); }

			Node* leftChild() { return children_[0].get(); }
			Node* rightChild() { return children_[1].get(); }
		};

		class TernaryNode : public Node {
		public:
			TernaryNode(const std::string& id) : Node(id, 3) {}
			virtual ~TernaryNode() {}

			class Factory;

			void setChildren(Node* first, Node* second, Node* third) { setFirstChild(first); setSecondChild(second); setThirdChild(third); }
			void setFirstChild(Node* node) { connectTo(node, children_[0]); }
			void setSecondChild(Node* node) { connectTo(node, children_[1]); }
			void setThirdChild(Node* node) { connectTo(node, children_[2]); }

			Node* firstChild() { return children_[0].get(); }
			Node* secondChild() { return children_[1].get(); }
			Node* thirdChild() { return children_[2].get(); }
		};

		class GenericNode : public Node {
		public:
			GenericNode(const std::string& id) : Node(id) {}
			virtual ~GenericNode() {}

			class Factory;

			void add(Node* node);
			std::size_t count() const {
				return children_.size();
			}
			Node* child(std::size_t i) { return children_[i].get(); }
		};

		BehaviourTree();

		void tick(EvaluationContext& context);

		void setRoot(Node* node) { root_.reset(node); }

	private:
		std::unique_ptr<Node> root_;
	};
}

#endif // _BEHAVIOURTREE_H

