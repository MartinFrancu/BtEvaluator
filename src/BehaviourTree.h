#ifndef _BEHAVIOURTREE_H
#define _BEHAVIOURTREE_H

#include <OOAICallback.h>

#include <vector>
#include <memory>

namespace BT
{
  enum EvaluationResult
  {
    btUndefined = 0,

    // final results
    btSuccess = 1,
    btFailure = 2,

    // intermediary result
    btRunning = 4
  };

  class BehaviourTree
  {
  public:
    class Node;

    class EvaluationContext
    {
    public:
      springai::OOAICallback* callback_;
      EvaluationContext(springai::OOAICallback* callback)
        : callback_(callback), units_(callback->GetSelectedUnits())
      {
      }

      const std::vector<springai::Unit*> units() const { return units_; }
      void setUnits(const std::vector<springai::Unit*> units) { units_ = units; }

      void initialize();
      EvaluationResult tickNode(Node* node);
      void finalize();
    private:
      std::vector<springai::Unit*> units_;
      std::vector<Node*> currentlyRunning;
      std::vector<Node*> previouslyRunning;
    };

    class Node
    {
    public:
      Node() : parent_(nullptr) { }
      virtual ~Node() {}

      virtual std::string name() = 0;
      virtual EvaluationResult tick(EvaluationContext& context) = 0;
      virtual void reset() {} // reset can quite often do nothing
    protected:
      void connectTo(Node* node, std::unique_ptr<Node>& link);
    private:
      Node* parent_; // weak reference
    };

    class LeafNode : public Node
    { };

    class UnaryNode : public Node
    {
    public:
      virtual ~UnaryNode() {}

      void setChild(Node* node) { connectTo(node, child_); }

      Node* child() { return child_.get(); }
    private:
      std::unique_ptr<Node> child_;
    };

    class BinaryNode : public Node
    {
    public:
      virtual ~BinaryNode() {}

      void setChildren(Node* left, Node* right) { setLeftChild(left); setRightChild(right); }
      void setLeftChild(Node* node) { connectTo(node, leftChild_); }
      void setRightChild(Node* node) { connectTo(node, rightChild_); }

      Node* leftChild() { return leftChild_.get(); }
      Node* rightChild() { return rightChild_.get(); }
    private:
      std::unique_ptr<Node> leftChild_;
      std::unique_ptr<Node> rightChild_;
    };

    class TernaryNode : public Node
    {
    public:
      virtual ~TernaryNode() {}

      void setChildren(Node* first, Node* second, Node* third) { setFirstChild(first); setSecondChild(second); setThirdChild(third); }
      void setFirstChild(Node* node) { connectTo(node, firstChild_); }
      void setSecondChild(Node* node) { connectTo(node, secondChild_); }
      void setThirdChild(Node* node) { connectTo(node, thirdChild_); }

      Node* firstChild() { return firstChild_.get(); }
      Node* secondChild() { return secondChild_.get(); }
      Node* thirdChild() { return thirdChild_.get(); }
    private:
      std::unique_ptr<Node> firstChild_;
      std::unique_ptr<Node> secondChild_;
      std::unique_ptr<Node> thirdChild_;
    };

    class GenericNode : public Node
    {
    public:
      virtual ~GenericNode() {}

      void add(Node* node);
      std::size_t count() { return children_.size(); }
      Node* child(std::size_t i) { return children_[i].get(); }
    private:
      std::vector<std::unique_ptr<Node>> children_;
    };

    BehaviourTree() { }

    void tick(EvaluationContext& context);

    void setRoot(Node* node) { root_.reset(node); }

  private:
    std::unique_ptr<Node> root_;
  };
}

#endif // _BEHAVIOURTREE_H

