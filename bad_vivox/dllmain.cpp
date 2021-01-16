#include "pch.h"

#define DEBUG_OUTPUT

HINSTANCE m_hinst_dll = nullptr;

extern "C" UINT_PTR mProcs[180]{ 0 };

LPCSTR import_names[] = { 
	"auxGetDevCapsA", "auxGetDevCapsW", "auxGetNumDevs", "auxGetVolume", "auxOutMessage", "auxSetVolume", "CloseDriver", "DefDriverProc", "DriverCallback", 
	"DrvGetModuleHandle", "GetDriverModuleHandle", "joyConfigChanged", "joyGetDevCapsA", "joyGetDevCapsW", "joyGetNumDevs", "joyGetPos", "joyGetPosEx", "joyGetThreshold", "joyReleaseCapture", 
	"joySetCapture", "joySetThreshold", "mciDriverNotify", "mciDriverYield", "mciExecute", "mciFreeCommandResource", "mciGetCreatorTask", "mciGetDeviceIDA", "mciGetDeviceIDFromElementIDA", 
	"mciGetDeviceIDFromElementIDW", "mciGetDeviceIDW", "mciGetDriverData", "mciGetErrorStringA", "mciGetErrorStringW", "mciGetYieldProc", "mciLoadCommandResource", "mciSendCommandA", 
	"mciSendCommandW", "mciSendStringA", "mciSendStringW", "mciSetDriverData", "mciSetYieldProc", "midiConnect", "midiDisconnect", "midiInAddBuffer", "midiInClose", "midiInGetDevCapsA", 
	"midiInGetDevCapsW", "midiInGetErrorTextA", "midiInGetErrorTextW", "midiInGetID", "midiInGetNumDevs", "midiInMessage", "midiInOpen", "midiInPrepareHeader", "midiInReset", "midiInStart", 
	"midiInStop", "midiInUnprepareHeader", "midiOutCacheDrumPatches", "midiOutCachePatches", "midiOutClose", "midiOutGetDevCapsA", "midiOutGetDevCapsW", "midiOutGetErrorTextA", "midiOutGetErrorTextW", 
	"midiOutGetID", "midiOutGetNumDevs", "midiOutGetVolume", "midiOutLongMsg", "midiOutMessage", "midiOutOpen", "midiOutPrepareHeader", "midiOutReset", "midiOutSetVolume", "midiOutShortMsg", 
	"midiOutUnprepareHeader", "midiStreamClose", "midiStreamOpen", "midiStreamOut", "midiStreamPause", "midiStreamPosition", "midiStreamProperty", "midiStreamRestart", "midiStreamStop", 
	"mixerClose", "mixerGetControlDetailsA", "mixerGetControlDetailsW", "mixerGetDevCapsA", "mixerGetDevCapsW", "mixerGetID", "mixerGetLineControlsA", "mixerGetLineControlsW", "mixerGetLineInfoA", 
	"mixerGetLineInfoW", "mixerGetNumDevs", "mixerMessage", "mixerOpen", "mixerSetControlDetails", "mmDrvInstall", "mmGetCurrentTask", "mmioAdvance", "mmioAscend", "mmioClose", "mmioCreateChunk", 
	"mmioDescend", "mmioFlush", "mmioGetInfo", "mmioInstallIOProcA", "mmioInstallIOProcW", "mmioOpenA", "mmioOpenW", "mmioRead", "mmioRenameA", "mmioRenameW", "mmioSeek", "mmioSendMessage", 
	"mmioSetBuffer", "mmioSetInfo", "mmioStringToFOURCCA", "mmioStringToFOURCCW", "mmioWrite", "mmsystemGetVersion", "mmTaskBlock", "mmTaskCreate", "mmTaskSignal", "mmTaskYield", "OpenDriver", 
	"PlaySound", "PlaySoundA", "PlaySoundW", "SendDriverMessage", "sndPlaySoundA", "sndPlaySoundW", "timeBeginPeriod", "timeEndPeriod", "timeGetDevCaps", "timeGetSystemTime", "timeGetTime", 
	"timeKillEvent", "timeSetEvent", "waveInAddBuffer", "waveInClose", "waveInGetDevCapsA", "waveInGetDevCapsW", "waveInGetErrorTextA", "waveInGetErrorTextW", "waveInGetID", "waveInGetNumDevs", 
	"waveInGetPosition", "waveInMessage", "waveInOpen", "waveInPrepareHeader", "waveInReset", "waveInStart", "waveInStop", "waveInUnprepareHeader", "waveOutBreakLoop", "waveOutClose", 
	"waveOutGetDevCapsA", "waveOutGetDevCapsW", "waveOutGetErrorTextA", "waveOutGetErrorTextW", "waveOutGetID", "waveOutGetNumDevs", "waveOutGetPitch", "waveOutGetPlaybackRate", "waveOutGetPosition", 
	"waveOutGetVolume", "waveOutMessage", "waveOutOpen", "waveOutPause", "waveOutPrepareHeader", "waveOutReset", "waveOutRestart", "waveOutSetPitch", "waveOutSetPlaybackRate", "waveOutSetVolume", 
	"waveOutUnprepareHeader", "waveOutWrite", "WOWAppExit" 
};

using tVivoxMethod = void(__fastcall*)(void*);
tVivoxMethod rVivoxInitialize = nullptr;
tVivoxMethod rVivoxLogout = nullptr;
tVivoxMethod rVivoxUninitialize = nullptr;
tVivoxMethod rApplySFXVolumes = nullptr; //Not a vivox method, but caused a crash for me, so I don't like it

/// <summary>
/// A hook to handle 3 vivox functions that are called
/// </summary>
/// <param name="this__">Reference to caller</param>
/// <returns></returns>
void __fastcall hVivoxMethod(void* this__)
{
#ifdef DEBUG_OUTPUT
	printf("A vivox method was called! Doing nothing instead...\n");
#endif
	return;
}


using tToggleSpeaking = void(__fastcall*)(void*, bool, bool);
tToggleSpeaking rToggleSpeaking = nullptr;

/// <summary>
/// Hook to the ToggleSpeaking method in order to make it do nothing.. or we crash
/// </summary>
/// <param name="this__">Reference to caller</param>
/// <param name="bSpeaking"></param>
/// <param name="bSuperRange"></param>
/// <returns></returns>
void __fastcall hToggleSpeaking(void* this__, bool bSpeaking, bool bSuperRange)
{
#ifdef DEBUG_OUTPUT
	printf("Toggle speaking was called! Doing nothing instead...\n");
#endif
	return;
}

/// <summary>
/// Template used to wait for an object to be ready. Thanks paracord :)
/// </summary>
/// <typeparam name="Fn"></typeparam>
/// <param name="function"></param>
/// <param name="interval"></param>
/// <param name="time_out"></param>
/// <returns></returns>
template <class Fn>
auto wait_on_object(Fn function, std::chrono::steady_clock::duration interval = std::chrono::milliseconds(10), std::chrono::steady_clock::duration time_out = std::chrono::minutes(2)) -> decltype(function())
{
	const auto start_time = std::chrono::steady_clock::now();

	while (std::chrono::steady_clock::now() - start_time < time_out)
	{
		if (const auto result = function(); result)
			return result;

		std::this_thread::sleep_for(interval);
	}

	return {};
}

/// <summary>
/// Handle everything in a thread since we need to wait for some things like the window and the module to both be ready
/// </summary>
/// <param name=""></param>
/// <returns></returns>
unsigned long MainThread(void*)
{
	const auto dedi_window = wait_on_object([]() { return FindWindowA(0, "ShooterGame (64-bit, PCD3D_SM5)"); });

	if (!dedi_window)
		return FALSE;

	const auto shootergame_handle = wait_on_object([]() { return GetModuleHandleW(L"UE4Editor-ShooterGame.dll"); });

	if (!shootergame_handle) 
		return FALSE;

	{
		auto address = FindPattern(GetModuleHandleW(L"UE4Editor-ShooterGame.dll"), reinterpret_cast<const unsigned char*>("\xE8\x00\x00\x00\x00\x48\x8B\x8C\x24\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00"), "x????xxxx????xx????xx????");
		if (address == -1) 
			return FALSE; 
		rVivoxInitialize = (tVivoxMethod)(address + *(DWORD*)(address + 1) + 5);
	}

	{
		auto address = FindPattern(GetModuleHandleW(L"UE4Editor-ShooterGame.dll"),reinterpret_cast<const unsigned char*>("\xE8\x00\x00\x00\x00\x48\x83\xC4\x38\xC3\xCC\xCC\x48\x89\x4C\x24\x00\x56"),"x????xxxxxxxxxxx?x");
		if (address == -1)
			return FALSE;
		rVivoxUninitialize = (tVivoxMethod)(address + *(DWORD*)(address + 1) + 5);
	}

	rVivoxLogout = (tVivoxMethod)FindPattern(GetModuleHandleW(L"UE4Editor-ShooterGame.dll"),reinterpret_cast<const unsigned char*>("\x48\x89\x4C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x48\xC7\x44\x24\x00\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x0F\xB6\x00\x85\xC0\x74\x07"),"xxxx?xxx????xxxx?????xxx????xxxxxxx");
	rApplySFXVolumes = (tVivoxMethod)FindPattern(GetModuleHandleW(L"UE4Editor-ShooterGame.dll"), reinterpret_cast<const unsigned char*>("\x48\x89\x4C\x24\x00\x56\x57\x48\x81\xEC\x00\x00\x00\x00\x48\xC7\x84\x24\x00\x00\x00\x00\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x0F\xB6\x00\x85\xC0\x0F\x85\x00\x00\x00\x00"), "xxxx?xxxxx????xxxx????????xxx????xxxxxxx????");
	rToggleSpeaking = (tToggleSpeaking)FindPattern(GetModuleHandleW(L"UE4Editor-ShooterGame.dll"), reinterpret_cast<const unsigned char*>("\x44\x88\x44\x24\x00\x88\x54\x24\x10\x48\x89\x4C\x24\x00\x48\x83\xEC\x68"), "xxxx?xxxxxxxx?xxxx");

#ifdef DEBUG_OUTPUT
	printf("<%p> UE4Editor-ShooterGame.dll\n", GetModuleHandleW(L"UE4Editor-ShooterGame.dll"));
	printf("<%p> UShooterGameInstance::VivoxInitialize\n", rVivoxInitialize);
	printf("<%p> UShooterGameInstance::VivoxLogout\n", rVivoxLogout);
	printf("<%p> UShooterGameInstance::VivoxUninitialize\n", rVivoxUninitialize);
	printf("<%p> UShooterGameUserSettings::ApplySFXVolumes\n", rApplySFXVolumes);
	printf("<%p> AShooterPlayerController::ToggleSpeaking\n", rToggleSpeaking);
#endif

	if (DetourTransactionBegin() != NO_ERROR ||
		DetourUpdateThread(GetCurrentThread()) != NO_ERROR ||
		DetourAttach(&(LPVOID&)rVivoxInitialize, (PVOID)hVivoxMethod) != NO_ERROR ||
		DetourAttach(&(LPVOID&)rVivoxUninitialize, (PVOID)hVivoxMethod) != NO_ERROR ||
		DetourAttach(&(LPVOID&)rVivoxLogout, (PVOID)hVivoxMethod) != NO_ERROR ||
		DetourAttach(&(LPVOID&)rApplySFXVolumes, (PVOID)hVivoxMethod) != NO_ERROR ||
		DetourAttach(&(LPVOID&)rToggleSpeaking, (PVOID)hToggleSpeaking) != NO_ERROR ||
		DetourTransactionCommit() != NO_ERROR)
		return FALSE;
	return TRUE;
}

/// <summary>
/// The entrypoint where we handle our dll proxying
/// </summary>
/// <param name="hinst_dll"></param>
/// <param name="fdw_reason"></param>
/// <param name=""></param>
/// <returns></returns>
BOOL APIENTRY DllMain(HINSTANCE hinst_dll, DWORD fdw_reason, LPVOID)
{
	if (fdw_reason == DLL_PROCESS_ATTACH)
	{
#ifdef DEBUG_OUTPUT
		FILE* pFile = nullptr;

		AllocConsole();
		freopen_s(&pFile, "CONOUT$", "w", stdout);
#endif

		DisableThreadLibraryCalls(hinst_dll);

		CHAR sys_dir[MAX_PATH];
		GetSystemDirectoryA(sys_dir, MAX_PATH);

		char buffer[MAX_PATH];
		sprintf_s(buffer, "%s\\winmm.dll", sys_dir);

		m_hinst_dll = LoadLibraryA(buffer);

		if (m_hinst_dll == nullptr)
			return FALSE;

		for (int i = 0; i < 180; i++)
			mProcs[i] = reinterpret_cast<UINT_PTR>(GetProcAddress(m_hinst_dll, import_names[i]));

		if (!CreateThread(0, 0, &MainThread, 0, 0, 0))
			return FALSE;
	}
	else if (fdw_reason == DLL_PROCESS_DETACH)
	{
		if (DetourTransactionBegin() != NO_ERROR ||
			DetourUpdateThread(GetCurrentThread()) != NO_ERROR ||
			DetourDetach(&(LPVOID&)rVivoxInitialize, (PVOID)hVivoxMethod) != NO_ERROR ||
			DetourDetach(&(LPVOID&)rVivoxUninitialize, (PVOID)hVivoxMethod) != NO_ERROR ||
			DetourDetach(&(LPVOID&)rVivoxLogout, (PVOID)hVivoxMethod) != NO_ERROR ||
			DetourDetach(&(LPVOID&)rApplySFXVolumes, (PVOID)hVivoxMethod) != NO_ERROR ||
			DetourDetach(&(LPVOID&)rToggleSpeaking, (PVOID)hToggleSpeaking) != NO_ERROR ||
			DetourTransactionCommit() != NO_ERROR)
			return FALSE;

		FreeLibrary(m_hinst_dll);
	}
	return TRUE;
}

// Don't know why I picked a dll with 180 exports but no turning back now
extern "C" void auxGetDevCapsA_wrapper();
extern "C" void auxGetDevCapsW_wrapper();
extern "C" void auxGetNumDevs_wrapper();
extern "C" void auxGetVolume_wrapper();
extern "C" void auxOutMessage_wrapper();
extern "C" void auxSetVolume_wrapper();
extern "C" void CloseDriver_wrapper();
extern "C" void DefDriverProc_wrapper();
extern "C" void DriverCallback_wrapper();
extern "C" void DrvGetModuleHandle_wrapper();
extern "C" void GetDriverModuleHandle_wrapper();
extern "C" void joyConfigChanged_wrapper();
extern "C" void joyGetDevCapsA_wrapper();
extern "C" void joyGetDevCapsW_wrapper();
extern "C" void joyGetNumDevs_wrapper();
extern "C" void joyGetPos_wrapper();
extern "C" void joyGetPosEx_wrapper();
extern "C" void joyGetThreshold_wrapper();
extern "C" void joyReleaseCapture_wrapper();
extern "C" void joySetCapture_wrapper();
extern "C" void joySetThreshold_wrapper();
extern "C" void mciDriverNotify_wrapper();
extern "C" void mciDriverYield_wrapper();
extern "C" void mciExecute_wrapper();
extern "C" void mciFreeCommandResource_wrapper();
extern "C" void mciGetCreatorTask_wrapper();
extern "C" void mciGetDeviceIDA_wrapper();
extern "C" void mciGetDeviceIDFromElementIDA_wrapper();
extern "C" void mciGetDeviceIDFromElementIDW_wrapper();
extern "C" void mciGetDeviceIDW_wrapper();
extern "C" void mciGetDriverData_wrapper();
extern "C" void mciGetErrorStringA_wrapper();
extern "C" void mciGetErrorStringW_wrapper();
extern "C" void mciGetYieldProc_wrapper();
extern "C" void mciLoadCommandResource_wrapper();
extern "C" void mciSendCommandA_wrapper();
extern "C" void mciSendCommandW_wrapper();
extern "C" void mciSendStringA_wrapper();
extern "C" void mciSendStringW_wrapper();
extern "C" void mciSetDriverData_wrapper();
extern "C" void mciSetYieldProc_wrapper();
extern "C" void midiConnect_wrapper();
extern "C" void midiDisconnect_wrapper();
extern "C" void midiInAddBuffer_wrapper();
extern "C" void midiInClose_wrapper();
extern "C" void midiInGetDevCapsA_wrapper();
extern "C" void midiInGetDevCapsW_wrapper();
extern "C" void midiInGetErrorTextA_wrapper();
extern "C" void midiInGetErrorTextW_wrapper();
extern "C" void midiInGetID_wrapper();
extern "C" void midiInGetNumDevs_wrapper();
extern "C" void midiInMessage_wrapper();
extern "C" void midiInOpen_wrapper();
extern "C" void midiInPrepareHeader_wrapper();
extern "C" void midiInReset_wrapper();
extern "C" void midiInStart_wrapper();
extern "C" void midiInStop_wrapper();
extern "C" void midiInUnprepareHeader_wrapper();
extern "C" void midiOutCacheDrumPatches_wrapper();
extern "C" void midiOutCachePatches_wrapper();
extern "C" void midiOutClose_wrapper();
extern "C" void midiOutGetDevCapsA_wrapper();
extern "C" void midiOutGetDevCapsW_wrapper();
extern "C" void midiOutGetErrorTextA_wrapper();
extern "C" void midiOutGetErrorTextW_wrapper();
extern "C" void midiOutGetID_wrapper();
extern "C" void midiOutGetNumDevs_wrapper();
extern "C" void midiOutGetVolume_wrapper();
extern "C" void midiOutLongMsg_wrapper();
extern "C" void midiOutMessage_wrapper();
extern "C" void midiOutOpen_wrapper();
extern "C" void midiOutPrepareHeader_wrapper();
extern "C" void midiOutReset_wrapper();
extern "C" void midiOutSetVolume_wrapper();
extern "C" void midiOutShortMsg_wrapper();
extern "C" void midiOutUnprepareHeader_wrapper();
extern "C" void midiStreamClose_wrapper();
extern "C" void midiStreamOpen_wrapper();
extern "C" void midiStreamOut_wrapper();
extern "C" void midiStreamPause_wrapper();
extern "C" void midiStreamPosition_wrapper();
extern "C" void midiStreamProperty_wrapper();
extern "C" void midiStreamRestart_wrapper();
extern "C" void midiStreamStop_wrapper();
extern "C" void mixerClose_wrapper();
extern "C" void mixerGetControlDetailsA_wrapper();
extern "C" void mixerGetControlDetailsW_wrapper();
extern "C" void mixerGetDevCapsA_wrapper();
extern "C" void mixerGetDevCapsW_wrapper();
extern "C" void mixerGetID_wrapper();
extern "C" void mixerGetLineControlsA_wrapper();
extern "C" void mixerGetLineControlsW_wrapper();
extern "C" void mixerGetLineInfoA_wrapper();
extern "C" void mixerGetLineInfoW_wrapper();
extern "C" void mixerGetNumDevs_wrapper();
extern "C" void mixerMessage_wrapper();
extern "C" void mixerOpen_wrapper();
extern "C" void mixerSetControlDetails_wrapper();
extern "C" void mmDrvInstall_wrapper();
extern "C" void mmGetCurrentTask_wrapper();
extern "C" void mmioAdvance_wrapper();
extern "C" void mmioAscend_wrapper();
extern "C" void mmioClose_wrapper();
extern "C" void mmioCreateChunk_wrapper();
extern "C" void mmioDescend_wrapper();
extern "C" void mmioFlush_wrapper();
extern "C" void mmioGetInfo_wrapper();
extern "C" void mmioInstallIOProcA_wrapper();
extern "C" void mmioInstallIOProcW_wrapper();
extern "C" void mmioOpenA_wrapper();
extern "C" void mmioOpenW_wrapper();
extern "C" void mmioRead_wrapper();
extern "C" void mmioRenameA_wrapper();
extern "C" void mmioRenameW_wrapper();
extern "C" void mmioSeek_wrapper();
extern "C" void mmioSendMessage_wrapper();
extern "C" void mmioSetBuffer_wrapper();
extern "C" void mmioSetInfo_wrapper();
extern "C" void mmioStringToFOURCCA_wrapper();
extern "C" void mmioStringToFOURCCW_wrapper();
extern "C" void mmioWrite_wrapper();
extern "C" void mmsystemGetVersion_wrapper();
extern "C" void mmTaskBlock_wrapper();
extern "C" void mmTaskCreate_wrapper();
extern "C" void mmTaskSignal_wrapper();
extern "C" void mmTaskYield_wrapper();
extern "C" void OpenDriver_wrapper();
extern "C" void PlaySound_wrapper();
extern "C" void PlaySoundA_wrapper();
extern "C" void PlaySoundW_wrapper();
extern "C" void SendDriverMessage_wrapper();
extern "C" void sndPlaySoundA_wrapper();
extern "C" void sndPlaySoundW_wrapper();
extern "C" void timeBeginPeriod_wrapper();
extern "C" void timeEndPeriod_wrapper();
extern "C" void timeGetDevCaps_wrapper();
extern "C" void timeGetSystemTime_wrapper();
extern "C" void timeGetTime_wrapper();
extern "C" void timeKillEvent_wrapper();
extern "C" void timeSetEvent_wrapper();
extern "C" void waveInAddBuffer_wrapper();
extern "C" void waveInClose_wrapper();
extern "C" void waveInGetDevCapsA_wrapper();
extern "C" void waveInGetDevCapsW_wrapper();
extern "C" void waveInGetErrorTextA_wrapper();
extern "C" void waveInGetErrorTextW_wrapper();
extern "C" void waveInGetID_wrapper();
extern "C" void waveInGetNumDevs_wrapper();
extern "C" void waveInGetPosition_wrapper();
extern "C" void waveInMessage_wrapper();
extern "C" void waveInOpen_wrapper();
extern "C" void waveInPrepareHeader_wrapper();
extern "C" void waveInReset_wrapper();
extern "C" void waveInStart_wrapper();
extern "C" void waveInStop_wrapper();
extern "C" void waveInUnprepareHeader_wrapper();
extern "C" void waveOutBreakLoop_wrapper();
extern "C" void waveOutClose_wrapper();
extern "C" void waveOutGetDevCapsA_wrapper();
extern "C" void waveOutGetDevCapsW_wrapper();
extern "C" void waveOutGetErrorTextA_wrapper();
extern "C" void waveOutGetErrorTextW_wrapper();
extern "C" void waveOutGetID_wrapper();
extern "C" void waveOutGetNumDevs_wrapper();
extern "C" void waveOutGetPitch_wrapper();
extern "C" void waveOutGetPlaybackRate_wrapper();
extern "C" void waveOutGetPosition_wrapper();
extern "C" void waveOutGetVolume_wrapper();
extern "C" void waveOutMessage_wrapper();
extern "C" void waveOutOpen_wrapper();
extern "C" void waveOutPause_wrapper();
extern "C" void waveOutPrepareHeader_wrapper();
extern "C" void waveOutReset_wrapper();
extern "C" void waveOutRestart_wrapper();
extern "C" void waveOutSetPitch_wrapper();
extern "C" void waveOutSetPlaybackRate_wrapper();
extern "C" void waveOutSetVolume_wrapper();
extern "C" void waveOutUnprepareHeader_wrapper();
extern "C" void waveOutWrite_wrapper();
extern "C" void WOWAppExit_wrapper();