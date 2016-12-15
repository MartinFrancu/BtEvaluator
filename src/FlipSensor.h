#ifndef FLIPSENSOR_H
#define FLIPSENSOR_H

#include "SpringCommand.h"

namespace BT {
	class FlipSensor : public SpringCommand {
		typedef BehaviourTree::EvaluationContext EvaluationContext;
	private:
		EvaluationResult currentState_;
	public:
		explicit FlipSensor(const std::string& id, springai::OOAICallback* callback) : SpringCommand(id, callback), currentState_(btUndefined) {}
		~FlipSensor() {}

		virtual std::string name() { return "FlipSensor"; }
		EvaluationResult execute(const EvaluationContext& context) override;

		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}

			std::string typeName() const override { return "flipSensor"; }
		protected:
			std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
			) const override;
		};
	};
}

#endif // FLIPSENSOR_H