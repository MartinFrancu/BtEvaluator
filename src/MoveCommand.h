#ifndef _MOVECOMMAND_H
#define _MOVECOMMAND_H

#include "SpringCommand.h"
#include "Unit.h"
#include "AIFloat3.h"

namespace BT {
	class MoveCommand : public SpringCommand {
	public:

		explicit MoveCommand(const std::string& id, springai::OOAICallback* callback)
			: SpringCommand(id, callback) {}
		

    virtual std::string name() override { return "MoveCommand"; }

		EvaluationResult execute(const std::vector<springai::Unit*> units) override{
			for (auto it = units.begin(); it != units.end(); ++it)
			{
				auto unit(*it);
				std::string msg("Moving unit " + std::to_string(unit->GetUnitId()));
				callback->GetGame()->SendTextMessage(msg.c_str(), 0);
				unit->MoveTo(springai::AIFloat3(0,0,0)); 
			}
      
      return btRunning;
		}

		class Factory : public SpringCommand::Factory {
		public:
			Factory(springai::OOAICallback* callback) : SpringCommand::Factory(callback) {}
			virtual std::string typeName() const { return "move"; }
		protected:
			virtual std::unique_ptr<LeafNode> createNode(
				const std::string& id,
				const std::map<std::string, ParameterValuePlaceholder>& parameters
				) const	{
				return std::unique_ptr<BehaviourTree::LeafNode>(
					new MoveCommand(id, callback_)
					);
			}
		};
	};
}

#endif // _MOVECOMMAND_H

