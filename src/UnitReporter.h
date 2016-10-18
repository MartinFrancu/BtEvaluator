#ifndef UNITREPORTER_H
#define UNITREPORTER_H

#include "SpringCommand.h"

namespace BT {
  class UnitReporter : public SpringCommand {
  private:
    int maxUnitReported_;
  public:
    explicit UnitReporter(springai::OOAICallback* callback, int maxUnitReported = 5) : SpringCommand(callback), maxUnitReported_(maxUnitReported) {}
    ~UnitReporter() {}

    virtual std::string name() { return "UnitReporter"; }
    virtual EvaluationResult execute(const std::vector<springai::Unit*> units) override;
  };
}

#endif // UNITREPORTER_H