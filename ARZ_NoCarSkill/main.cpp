#include <Windows.h>
#include "main.h"

using std::string;

SAMPFUNCS *SF = new SAMPFUNCS();
static bool activeStatus = false;

bool CALLBACK OutcomingData(stRakNetHookParams* params)
{
	if (activeStatus && params->packetId == PacketEnumeration::ID_VEHICLE_SYNC)
	{
		stInCarData data;													// Создаем структуру
		memset(&data, 0, sizeof(stInCarData));								// Обнуляем всё
		byte packet;														// переменная для ID пакета

		params->bitStream->ResetReadPointer();								// Сбрасываем оффсет чтения
		params->bitStream->Read(packet);									// Читаем ID пакета
		params->bitStream->Read((PCHAR)&data, sizeof(stInCarData));			// Читаем сам пакет
		params->bitStream->ResetReadPointer();								// Снова сбрасываем оффсет

		if (!data.sLeftRightKeys)
		{
			params->bitStream->ResetWritePointer();							// Сбрасываем оффсет записи
			data.sLeftRightKeys = bool(rand() % 2) ? 128 : 65408;			// Записываем кнопку
			params->bitStream->Write(packet);								// Записываем айди пакета
			params->bitStream->Write((PCHAR)&data, sizeof(stInCarData));	// Отправляем пакет
		}
	}
	return true;
}

void CALLBACK cmd_nocarskill(string params)
{
	string message = "[NCS]: {FFFFFF}Обход carskill'а ";
	if (activeStatus == false) {
		message += "{FF0000}включен.";
	}
	else {
		message += "{00FF00}выключен.";
	}

	SF->getSAMP()->getChat()->AddChatMessage(0x54AD10, message.c_str());
	activeStatus = !activeStatus;
}

void CALLBACK MainLoop()
{
	static bool initialized = false;
	if (!initialized)
	{
		if (GAME && GAME->GetSystemState() == eSystemState::GS_PLAYING_GAME && SF->getSAMP()->IsInitialized())
		{
			initialized = true;
			SF->getRakNet()->registerRakNetCallback(RakNetScriptHookType::RAKHOOK_TYPE_OUTCOMING_PACKET, OutcomingData);
			SF->getSAMP()->registerChatCommand("nocarskill", cmd_nocarskill);
			SF->LogConsole("[ARZ]NoCarSkill by InDigital загружен. Активация: /nocarskill");
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