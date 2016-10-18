#include "SequenceNode.h"

using namespace BT;

EvaluationResult SequenceNode::tick(EvaluationContext& context)
{
  for (; nextChildIndex_ < count(); ++nextChildIndex_)
  {
    Node* currentChild = child(nextChildIndex_);
    EvaluationResult childResult = context.tickNode(currentChild);
    if (childResult == btRunning)
      return btRunning;
    else if (childResult == btFailure)
    {
      reset();
      return btFailure;
    }
  }
  reset();
  return btSuccess;
}

void SequenceNode::reset()
{
  nextChildIndex_ = 0;
}
