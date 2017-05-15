#ifndef _GROUPREPORTER_H
#define _GROUPREPORTER_H

#include "SpringCommand.h"

namespace BT {
  class GroupReporter : public SpringCommand {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	private:
    int maxUnitReported_;
  public:
    explicit GroupReporter(const std::string& id, springai::OOAICallback* callback, int maxUnitReported = 5) : SpringCommand(id, callback), maxUnitReported_(maxUnitReported) {}
    ~GroupReporter() {}

    std::string name() override { return "GroupReporter"; }
    EvaluationResult execute(const EvaluationContext& context) override;

		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}

			std::string typeName() const override { return "groupReporter"; }
		protected:
			std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const override;
			std::string tooltip() const override;
		};
	};
}

#endif // _GROUPREPORTER_H