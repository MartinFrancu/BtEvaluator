echo off
set SPRING_SOURCE_DIR=%1
set SPRINGDATA_DIR=%2
set SolutionDir=%3

REM Building
cd %SPRING_SOURCE_DIR%build && mingw32-make -j4 BtEvaluator 
if %errorlevel% neq 0 exit /b %errorlevel%

REM Releasing the hold of DLL by spring
cd %SolutionDir%\BtEvaluator && PowerShell -executionpolicy bypass -File SendCommandToSpring.ps1 -message "aikill 0"
waitfor SomethingThatIsNeverHappening /t 1 2>NUL

REM Copying to SpringData
xcopy /y %SolutionDir%data\*.lua  %SPRINGDATA_DIR%AI\Skirmish\BtEvaluator\0.0001\
xcopy /y %SPRING_SOURCE_DIR%build\AI\Skirmish\BtEvaluator\SkirmishAI.dll %SPRINGDATA_DIR%AI\Skirmish\BtEvaluator\0.0001\
xcopy /y %SolutionDir%widgets\* %SPRINGDATA_DIR%LuaUI\Widgets\

REM Reloading widgets (which should also activate the AI again)
cd %SolutionDir%\BtEvaluator && PowerShell -executionpolicy bypass -File SendCommandToSpring.ps1 -message "luaui reload"
exit /b 0
