
#include "ntdll.h"
#pragma comment(lib, "ntdll")

#include "shlwapi.h"
#pragma comment(lib, "shlwapi")

#include <iostream>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

int main()
{
	// Obtain the full path to the DLL, whose PE image will serve as a starting point for a new process
	wchar_t wsImagePathBuffer[MAX_PATH] = { 0 };
	GetModuleFileNameW(nullptr, wsImagePathBuffer, MAX_PATH);
	PathRemoveFileSpecW(wsImagePathBuffer);
	wstring imagePath = wstring(wsImagePathBuffer) + L"\\TestDll.dll";
	if (!fs::exists(fs::path{ imagePath })) {
		std::cout << "Starting dll image was not found" << std::endl;
		return 1;
	}
	wstring nativeImagePath = L"\\??\\" + imagePath;

	// Path to the image file from which the process will be created
	UNICODE_STRING NtImagePath = { 0 };
	RtlInitUnicodeString(&NtImagePath, (PWSTR)nativeImagePath.c_str());

	// Create the process parameters
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters = nullptr;
	NTSTATUS st = RtlCreateProcessParametersEx(&ProcessParameters, &NtImagePath, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, RTL_USER_PROCESS_PARAMETERS_NORMALIZED);
	if (!NT_SUCCESS(st))
		return 2;

	// Initialize the PS_CREATE_INFO structure
	PS_CREATE_INFO CreateInfo = { 0 };
	CreateInfo.Size = sizeof(CreateInfo);
	CreateInfo.State = PsCreateInitialState;

	// Initialize the PS_ATTRIBUTE_LIST structure
	PPS_ATTRIBUTE_LIST AttributeList = (PS_ATTRIBUTE_LIST*)RtlAllocateHeap(RtlProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PS_ATTRIBUTE));
	if (!AttributeList) {
		RtlDestroyProcessParameters(ProcessParameters);
		return 3;
	}
	AttributeList->TotalLength = sizeof(PS_ATTRIBUTE_LIST) - sizeof(PS_ATTRIBUTE);
	AttributeList->Attributes[0].Attribute = PS_ATTRIBUTE_IMAGE_NAME;
	AttributeList->Attributes[0].Size = NtImagePath.Length;
	AttributeList->Attributes[0].Value = (ULONG_PTR)NtImagePath.Buffer;

	// Create the process
	HANDLE hProcess = nullptr, hThread = nullptr;
	st = NtCreateUserProcess(&hProcess, &hThread, PROCESS_ALL_ACCESS, THREAD_ALL_ACCESS, nullptr, nullptr, 0, 0, ProcessParameters, &CreateInfo, AttributeList);
	if (NT_SUCCESS(st) ) {
		std::cout << "Process id:     " << GetProcessId(hProcess) << std::endl;
		std::cout << "Main thread id: " << GetThreadId(hThread) << std::endl;
	}
	else {
		std::cout << "NtCreateUserProcess failed with 0x" << std::hex << std::setw(8) << std::setfill('0') << st;
	}

	// Clean up
	RtlFreeHeap(RtlProcessHeap(), 0, AttributeList);
	RtlDestroyProcessParameters(ProcessParameters);

	return 0;
}
