#include <Windows.h>
#include "main.h"

using std::string;

SAMPFUNCS *SF = new SAMPFUNCS();
static bool activeStatus = false;

bool CALLBACK OutcomingData(stRakNetHookParams* params)
{
	if (activeStatus && params->packetId == PacketEnumeration::ID_VEHICLE_SYNC)
	{
		stInCarData data;													// ������� ���������
		memset(&data, 0, sizeof(stInCarData));								// �������� ��
		byte packet;														// ���������� ��� ID ������

		params->bitStream->ResetReadPointer();								// ���������� ������ ������
		params->bitStream->Read(packet);									// ������ ID ������
		params->bitStream->Read((PCHAR)&data, sizeof(stInCarData));			// ������ ��� �����
		params->bitStream->ResetReadPointer();								// ����� ���������� ������

		if (!data.sLeftRightKeys)
		{
			params->bitStream->ResetWritePointer();							// ���������� ������ ������
			data.sLeftRightKeys = bool(rand() % 2) ? 128 : 65408;			// ���������� ������
			params->bitStream->Write(packet);								// ���������� ���� ������
			params->bitStream->Write((PCHAR)&data, sizeof(stInCarData));	// ���������� �����
		}
	}
	return true;
}

void CALLBACK cmd_nocarskill(string params)
{
	string message = "[NCS]: {FFFFFF}����� carskill'� ";
	if (activeStatus == false) {
		message += "{FF0000}�������.";
	}
	else {
		message += "{00FF00}��������.";
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
			SF->LogConsole("[ARZ]NoCarSkill by InDigital ��������. ���������: /nocarskill");
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