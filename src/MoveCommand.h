#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "SpringCommand.h"
#include "Unit.h"
#include "AIFloat3.h"

namespace BT {
	class MoveCommand : public SpringCommand {
	public:

		explicit MoveCommand(springai::OOAICallback* callback)
			: SpringCommand(callback) {}
		

    virtual std::string name() override { return "MoveCommand"; }

		EvaluationResult execute() override{
			auto units(callback->GetSelectedUnits()); // FIXME gets all units ... ?

			for (auto it = units.begin(); it != units.end(); ++it)
			{
				auto unit(*it);
				std::string msg("Moving unit " + std::to_string(unit->GetUnitId()));
				callback->GetGame()->SendTextMessage(msg.c_str(), 0);
				unit->MoveTo(springai::AIFloat3(0,0,0)); 
			}
      
      return btRunning;
		}

	};
}

#endif

