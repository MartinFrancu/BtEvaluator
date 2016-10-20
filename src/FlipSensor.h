#ifndef FLIPSENSOR_H
#define FLIPSENSOR_H

#include "SpringCommand.h"

namespace BT {
  class FlipSensor : public SpringCommand {
  private:
    EvaluationResult currentState_;
  public:
    explicit FlipSensor(const std::string& id, springai::OOAICallback* callback) : SpringCommand(id, callback), currentState_(btUndefined) {}
    ~FlipSensor() {}

    virtual std::string name() { return "FlipSensor"; }
    virtual EvaluationResult execute(const std::vector<springai::Unit*> units) override;

		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}
			virtual std::string typeName() const { return "flipSensor"; }
		protected:
			virtual std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const;
		};
  };
}

#endif // FLIPSENSOR_H