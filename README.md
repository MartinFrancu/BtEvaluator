# BtEvaluator
Part of BETS software project at MFF UK. Repository of C++ based AI for real time evaluation of behaviour trees for Spring-based games.

## Location
The project should be checked out into the "AI/Skrimish/BtEvaluator/" directory within the spring source code repository. CMake should be configured to produce MinGW makefiles within the "build/" direcotry within the spring source code repository.

Once checked out, the "BtEvaluator/SpringPaths.props" should be copied as "BtEvaluator/SpringPaths.props.user" and the "SPRINGDATA_DIR" property should be set-up to point to the appropriate SpringData directory. If necessary, the "SPRING_SOURCE_DIR" property can also be changed. "*.user" files should be properly set-up to be ignored by git, so they should never get commited.

