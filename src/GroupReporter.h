#ifndef _GROUPREPORTER_H
#define _GROUPREPORTER_H

#include "SpringCommand.h"

namespace BT {
  class GroupReporter : public SpringCommand {
  private:
    int maxUnitReported_;
  public:
    explicit GroupReporter(const std::string& id, springai::OOAICallback* callback, int maxUnitReported = 5) : SpringCommand(id, callback), maxUnitReported_(maxUnitReported) {}
    ~GroupReporter() {}

    virtual std::string name() { return "GroupReporter"; }
    virtual EvaluationResult execute(const std::vector<springai::Unit*> units) override;

		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}
			virtual std::string typeName() const { return "groupReporter"; }
		protected:
			virtual std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const;
		};
	};
}

#endif // _GROUPREPORTER_H