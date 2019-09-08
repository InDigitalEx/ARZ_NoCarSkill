#include <Windows.h>
#include "main.h"

using std::string;

SAMPFUNCS *SF = new SAMPFUNCS();
bool ActiveStatus = false;

bool CALLBACK IncomingRPC(stRakNetHookParams* params)
{
	if (params->packetId == RPC_ScrSetVehicleVelocity)
	{
		bool value = ActiveStatus;
		if (SF->getGame()->isKeyDown(160)) {
			value = !value;
		}
		return !value;
	}
	return true;
}

void CALLBACK cmd_nocarskill(string params)
{
	ActiveStatus = !ActiveStatus;
	string message = "[NCS]: {FFFFFF}Обход carskill'а ";
	message += ActiveStatus ? "{FF0000}включен." : "{00FF00}выключен.";
	SF->getSAMP()->getChat()->AddChatMessage(0x54AD10, message.c_str());
}

void CALLBACK MainLoop()
{
	static bool initialized = false;
	if (!initialized)
	{
		if (GAME && GAME->GetSystemState() == eSystemState::GS_PLAYING_GAME && SF->getSAMP()->IsInitialized())
		{
			initialized = true;
			SF->getRakNet()->registerRakNetCallback(RakNetScriptHookType::RAKHOOK_TYPE_INCOMING_RPC, IncomingRPC);
			SF->getSAMP()->registerChatCommand("nocarskill", cmd_nocarskill);
			SF->LogConsole("{0x54AD10}[ARZ]NoCarSkill by InDigital загружен. {FFFFFF}Активация: /nocarskill");
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
	if (dwReasonForCall == DLL_PROCESS_ATTACH)
	{
		SF->initPlugin(MainLoop, hModule);
	}
	return true;
}