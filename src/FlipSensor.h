#ifndef FLIPSENSOR_H
#define FLIPSENSOR_H

#include "SpringCommand.h"

namespace BT {
  class FlipSensor : public SpringCommand {
  private:
    EvaluationResult currentState_;
  public:
    explicit FlipSensor(springai::OOAICallback* callback) : SpringCommand(callback), currentState_(btUndefined) {}
    ~FlipSensor() {}

    virtual std::string name() { return "FlipSensor"; }
    virtual EvaluationResult execute(const std::vector<springai::Unit*> units) override;
  };
}

#endif // FLIPSENSOR_H