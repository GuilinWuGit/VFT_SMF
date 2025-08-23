@echo off
echo ========================================
echo VFT_SMF V3 - B737_Taxi Build Script (Architecture Optimized)
echo ========================================
echo.

echo Compiling EventDrivenSimulation_NewArchitecture.exe with optimized architecture...
g++ -std=c++17 -I../../src -I../../src/I_ThirdPartyTools -o EventDrivenSimulation_NewArchitecture.exe ^
../../src/G_SimulationManager/D_EventDrivenArchitecture/EventDrivenMain_NewArchitecture.cpp ^
../../src/G_SimulationManager/D_EventDrivenArchitecture/AgentThreadFunctions.cpp ^
../../src/G_SimulationManager/D_EventDrivenArchitecture/EventDispatcher.cpp ^
../../src/E_GlobalSharedDataSpace/GlobalSharedDataSpace.cpp ^
../../src/E_GlobalSharedDataSpace/GlobalSharedDataStruct.cpp ^
../../src/G_SimulationManager/LogAndData/DataRecorder.cpp ^
../../src/G_SimulationManager/A_TimeSYNC/Simulation_Clock.cpp ^
../../src/G_SimulationManager/B_SimManage/EventMonitor.cpp ^
../../src/G_SimulationManager/C_ConfigManager/ConfigManager.cpp ^
../../src/F_ScenarioModelling/A_FlightPlanParser/FlightPlanParser.cpp ^
../../src/A_PilotAgentModel/PilotAgent.cpp ^
../../src/A_PilotAgentModel/Pilot_001/Pilot_001_Strategy.cpp ^
../../src/A_PilotAgentModel/Pilot_002/Pilot_002_Strategy.cpp ^
../../src/A_PilotAgentModel/Pilot_001/ServiceTwin/PilotATCCommandHandler.cpp ^
../../src/A_PilotAgentModel/Pilot_001/ServiceTwin/PilotManualControlHandler.cpp ^
../../src/B_AircraftAgentModel/AircraftAgent.cpp ^
../../src/B_AircraftAgentModel/AircraftDigitalTwinFactory.cpp ^
../../src/B_AircraftAgentModel/B737/B737DigitalTwin.cpp ^
../../src/B_AircraftAgentModel/B737/ModelTwin/FlightControl/B737_AutoFlightControlLaw.cpp ^
../../src/B_AircraftAgentModel/B737/ServiceTwin/ServiceTwin_StateManager.cpp ^
../../src/B_AircraftAgentModel/B737/ServiceTwin/ControlPriorityManager.cpp ^
../../src/C_EnvirnomentAgentModel/EnvironmentAgent.cpp ^
../../src/C_EnvirnomentAgentModel/EnvironmentConfigManager.cpp ^
../../src/D_ATCAgentModel/A_StandardBase/ATCAgent.cpp ^
../../src/D_ATCAgentModel/ATC_001/ATC_001_Strategy.cpp ^
../../src/D_ATCAgentModel/ATC_002/ATC_002_Strategy.cpp ^
../../src/E_FlightDynamics/FlightDynamicsAgent.cpp ^
../../src/E_FlightDynamics/B737/B737_FlightDynamicsModel_New.cpp ^
-lpthread

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Build Successful!
    echo ========================================
    echo Executable: EventDrivenSimulation_NewArchitecture.exe
    echo.
    echo Architecture Optimization Features:
    echo - Pilot Model: Focuses on operation intent definition
    echo - Aircraft Model: Focuses on control algorithm execution
    echo - Control Priority Management System integrated
    echo - Clear separation of responsibilities
    echo.
    echo Note: Program will automatically clean previous output files on startup
    echo.
    echo Build completed, you can manually run EventDrivenSimulation_NewArchitecture.exe
) else (
    echo.
    echo ========================================
    echo Build Failed! Please check error messages.
    echo ========================================
)

echo.
pause
