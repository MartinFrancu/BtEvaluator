#ifndef _BEHAVIOURTREE_H
#define _BEHAVIOURTREE_H

#include <OOAICallback.h>

#include <vector>
#include <set>
#include <memory>

namespace BT {

	enum EvaluationResult {
		btUndefined = 0,

		// final results
		btSuccess = 1,
		btFailure = 2,

		// intermediary result
		btRunning = 4,

		// breakpoint flag, meant to be immediately returned while storing the current state so that the node can resume
		btBreakpoint = 8
	};
	inline const char* nameOfEvaluationResult(EvaluationResult result) {
		switch (result) {
		case btUndefined: return "undefined";
		case btSuccess: return "success";
		case btFailure: return "failure";
		case btRunning: return "running";
		case btBreakpoint: return "stopped";
		default: return "unknown";
		}
	}

	/**
	 * Containes a pointer to the root of the tree. Handles the execution of the tree and its nodes.
	 * Also implements the breakpoint functionality
	 */
	class BehaviourTree {
	public:
		class Node;

		/**
		 * Contains data only relevant to the execution of the tree, not to the structure of the tree itself. (units, breakpoints,...)
		 */
		class EvaluationContext {
		public:
			static const int ALL_ROLES = -1;

			springai::OOAICallback* callback_;
			EvaluationContext(springai::OOAICallback* callback, const std::string& instanceId);

			const std::vector<springai::Unit*>& units() const { return *currentUnits_; }

			// Remove units from the tree (from any role)
			bool removeUnits(const std::vector<springai::Unit*>& units);

			// Assign units to the specified role in the tree
			void setUnits(int roleId, const std::vector<springai::Unit*>& units);

			// gets the index of the role the nodes will be executed on
			int activeRole() const { return currentRole_; }

			// sets the index of the role the nodes will be executed on
			void setActiveRole(int roleId);

			void setEvaluationPeriod(int period);
			int evaluationPeriod() const { return evaluationPeriod_; }
			int nextEvaluationFrame() const { return nextEvaluationFrame_; }

			// methods returning vector of nodes in a certain state

			const std::vector<std::pair<Node*, EvaluationResult>>& finished() const { return currentlyFinished; }
			const std::vector<Node*>& running() const { return currentlyRunning; }
			const std::vector<Node*>& stopped() const { return currentlyStopped; }

			const std::string& treeInstanceId() const { return instanceId_; }

			// removes all breakpoints and units from the tree and resets it
			void clear();

			// clears the internal state of execution of the tree and its nodes
			void reset();

			bool setBreakpoint(const std::string& nodeId);
			bool removeBreakpoint(const std::string& nodeId);

			// prepare the tree to be ticked
			void initialize();
			void stoppedInitialize();
			EvaluationResult tickNode(Node* node);
			void finalize();
		private:
			int currentRole_;
			std::set<std::string> breakpoints_;
			std::vector<springai::Unit*>* currentUnits_;
			std::vector<springai::Unit*> allUnits_;
			std::vector<std::vector<springai::Unit*>> roleUnits_;
			std::vector<std::pair<Node*, EvaluationResult>> currentlyFinished;
			std::vector<Node*> currentlyStopped;
			std::vector<Node*> currentlyRunning;
			std::vector<Node*> previouslyRunning;

			int evaluationPeriod_;
			int nextEvaluationFrame_;

			std::string instanceId_;
		};

		/**
		 * A representation of the node parameter
		 */
		struct ParameterDefinition {
			std::string name;
			std::string variableType;
			std::string componentType;
			std::string defaultValue;
		};

		struct ChildDefinition {};
		
		/**
		 * Abstract class representing any node in a tree. Contains pointers to its children. Derived class need to implement their logic in tick() method.
		 */
		class Node {
		public:
			class Factory;

			Node(const std::string& id, int children = 0) : id_(id), children_(children), parent_(nullptr), stoppedAt_(0)/*, canBreak_(true)*/ {}
			virtual ~Node() {}

			// unique id
			const std::string& id() const { return id_; }

			virtual std::string name() = 0;

			// called during a tree tick. Returns a current state of the node.
			virtual EvaluationResult tick(EvaluationContext& context) = 0;

			// clears the internal state of the node
			virtual void reset(const EvaluationContext& context); // base implementation only resets children

			bool canBreak() { return stoppedAt() == 0; }
			EvaluationResult stopAtEntry() { return stopAt(-1); }
			// // the following were available to provide breakpoint functionality, where a breakpoint would stop on a node when first reached and would not fire again until the node got reset or finished running
			// // if this is ever to be reactivated, the canBreak_ has be to properly resetted during Context:reset
			// void enableBreak() { canBreak_ = true; }
			// void disableBreak() { canBreak_ = false; }
		protected:
			unsigned int& stoppedAt() { return stoppedAt_; }
			EvaluationResult stopAt(unsigned int index) { stoppedAt_ = index; return btBreakpoint; }
			EvaluationResult notStopped(EvaluationResult result) { stoppedAt_ = 0; return result; }
			EvaluationResult keep(EvaluationResult result) { stoppedAt_ = (result == btBreakpoint ? 1 : 0); return result; }

			void connectTo(Node* node, std::unique_ptr<Node>& link);
			std::vector<std::unique_ptr<Node>> children_ = std::vector<std::unique_ptr<Node>>();
		private:
			std::string id_;
			Node* parent_; // weak reference
			unsigned int stoppedAt_;
			// bool canBreak_;
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

		/**
		 * Base class for nodes with variabl number of children
		 */
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

		void reset(EvaluationContext& context);
		bool tick(EvaluationContext& context); // returns whether the tick succesfully ended or whether a breakpoint was reached

		void setRoot(Node* node) { root_.reset(node); }

	private:
		bool breakpointReached_;
		std::unique_ptr<Node> root_;
	};
}

#endif // _BEHAVIOURTREE_H

