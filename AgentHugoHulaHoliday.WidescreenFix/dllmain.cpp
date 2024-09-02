// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "MemoryMgr.h"

using namespace Memory::VP;

int gResolutionX, gResolutionY;
float gAspectRatio, gLifebarOffset, gGadgetGunOffset;

void Set2DScale()
{
	float x = *(float*)0x54360C;


	float _4_3 = 4.0f / 3.0f;
	float scaleRatio = gAspectRatio / _4_3;

	x *= scaleRatio;

	Patch<float>(0x54360C, x);

	float lifebarXOffset = *(float*)0x51F038;
	lifebarXOffset *= scaleRatio * 1.1f;
	gLifebarOffset = lifebarXOffset;
	float gadgetGunXOffset = *(float*)0x51E138;
	gadgetGunXOffset *= scaleRatio * 1.1f;
	gGadgetGunOffset = gadgetGunXOffset;
}

void __declspec(naked) LifebarOffset_Hook()
{
	static int jmpContinue = 0x43E5EA;
	_asm pushad
	Patch<float>(0x51F038, gLifebarOffset);
	Patch<float>(0x51E138, gGadgetGunOffset);
	_asm {
		popad
		movss   xmm0, ds:0x51F038
		jmp jmpContinue
	}
}


void __declspec(naked) Set2DScale_Hook()
{
	static int jmpContinue = 0x405D10;

	_asm pushad

	Set2DScale();

	_asm {
		popad
		mov[ebp - 4], ebx
		mov     ds : 0x543610, eax
		jmp jmpContinue
	}
}


void __declspec(naked) SetAspectRatio_Hook()
{
	static int jmpContinue = 0x47A3D5;
	_asm pushad

	Patch<float>(0x522368, gAspectRatio);

	_asm {
		popad
		jmp jmpContinue
	}

}

void Init()
{

	gResolutionX = GetPrivateProfileInt(L"Video Card (Adapter 0)", L"Width", 800, L".\\Game.ini");
	gResolutionY = GetPrivateProfileInt(L"Video Card (Adapter 0)", L"Height", 600, L".\\Game.ini");

	gAspectRatio = (float)gResolutionX / (float)gResolutionY;

	InjectHook(0x47A3CE, SetAspectRatio_Hook, PATCH_JUMP);
	InjectHook(0x405D08, Set2DScale_Hook, PATCH_JUMP);
	InjectHook(0x43E5E2, LifebarOffset_Hook, PATCH_JUMP);
}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Init();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

