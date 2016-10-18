#ifndef WAITNODE_H
#define WAITNODE_H

#include "SpringCommand.h"

namespace BT {
  class WaitNode : public SpringCommand {
  private:
    std::size_t counter_;
    std::size_t tickCount_;
  public:
    explicit WaitNode(springai::OOAICallback* callback, std::size_t tickCount) : SpringCommand(callback), counter_(0), tickCount_(tickCount) {}
    ~WaitNode() {}

    virtual std::string name() { return "WaitNode"; }
    virtual EvaluationResult execute(const std::vector<springai::Unit*> units) override;
    virtual void reset() override;
  };
}

#endif // WAITNODE_H