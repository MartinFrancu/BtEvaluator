#ifndef WAITNODE_H
#define WAITNODE_H

#include "SpringCommand.h"

namespace BT {
  class WaitNode : public SpringCommand {
  private:
    std::size_t counter_;
    std::size_t tickCount_;
  public:
    explicit WaitNode(const std::string& id, springai::OOAICallback* callback, std::size_t tickCount)
			: SpringCommand(id, callback), counter_(0), tickCount_(tickCount) {}
    ~WaitNode() {}

    virtual std::string name() { return "WaitNode"; }
    virtual EvaluationResult execute(const std::vector<springai::Unit*> units) override;
    virtual void reset() override;

		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}
			virtual std::string typeName() const { return "wait"; }
		protected:
			virtual std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::string& treeInstanceId,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const;
		};
	};
}

#endif // WAITNODE_H