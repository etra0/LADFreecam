#include "FreecamGameLADIW.h"
#include "PatternScan.h"
#include "memory.h"

ccamera_set_info FreecamGameLADIW::GetCameraSetFunction()
{
	return (ccamera_set_info)PatternScan("48 89 5C 24 10 48 89 74 24 18 57 48 83 EC ? 48 8B FA 48 8B F1 48 8D 99 E0 00 00 00");
}

void* FreecamGameLADIW::GetPadUpdateFunction()
{
	return PatternScan("48 89 5C 24 18 48 89 6C 24 20 56 57 41 56 48 83 EC ? 48 8B E9 80 3D ? ? ? ? ?");
}

float* FreecamGameLADIW::GetSpeedValues() 
{
	return (float*)resolve_relative_addr(PatternScan(GetModuleHandle(NULL), "48 8D 0D ? ? ? ? C5 FA 11 0C 81 C3"), 7);
}

void* FreecamGameLADIW::GetAuthPauseFunction()
{
	return PatternScan("C7 40 04 ? ? ? ? 48 8B 87 60 08 00 00");
}

void FreecamGameLADIW::pause_auth(bool pause)
{
	if (auth_pause_func == nullptr)
		return;

	if (!pause)
	{
		BYTE patch[] = { 0xC7, 0x40, 0x04, 0x0, 0x0, 0x80, 0x3F };
		BYTE patch2[] = { 0xC7, 0x40, 0x08, 0x0, 0x0, 0x80, 0x3F };
		mem::Patch((BYTE*)auth_pause_func, patch, 7);
		mem::Patch((BYTE*)auth_pause_func + 14, patch2, 7);
	}
	else
	{
		BYTE patch[] = { 0xC7, 0x40, 0x04, 0x0A, 0xD7, 0x23, 0x3C };
		BYTE patch2[] = { 0xC7, 0x40, 0x08, 0x0A, 0xD7, 0x23, 0x3C };
		mem::Patch((BYTE*)auth_pause_func, patch, 7);
		mem::Patch((BYTE*)auth_pause_func + 14, patch2, 7);
	}
}