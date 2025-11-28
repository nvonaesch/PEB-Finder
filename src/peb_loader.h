#pragma once
#include <windows.h>
#include <stdio.h>

struct _PEB;
typedef struct _PEB *PPEB;

typedef struct _PROCESS_BASIC_INFORMATION
{
    PVOID Reserved1;
    PPEB PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWCH Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _CURDIR {
	UNICODE_STRING	DosPath;
	HANDLE 			Handle;
} CURDIR;

typedef struct _RTL_DRIVE_LETTER_CURDIR {
	USHORT Flags;
	USHORT Length;
	ULONG TimeStamp;
	UNICODE_STRING  DosPath;
} RTL_DRIVE_LETTER_CURDIR;


typedef struct _RTL_USER_PROCESS_PARAMETERS {
	ULONG          		MaximumLength;
	ULONG 		   		Length;
	ULONG		   		Flags;
	ULONG 		   		DebugFlags;
	HANDLE         		ConsoleHandle;
	ULONG          		ConsoleFlags;
	HANDLE         		StandardInput;
	HANDLE         		StandardOutput;
	HANDLE         		StandardError;
	CURDIR         		CurrentDirectory;
	UNICODE_STRING 		DllPath;
	UNICODE_STRING 		ImagePathName;
	UNICODE_STRING 		CommandLine;
	PVOID 		   		Environment;
	ULONG 		   		StartingX;
	ULONG 		   		StartingY;
	ULONG 		   		CountCharsX;
	ULONG 		   		CountCharsY;
	ULONG 		   		FillAttribute;
	ULONG 		   		WindowFlags;
	ULONG 		   		ShowWindowFlags;
	UNICODE_STRING 		WindowTitle;
	UNICODE_STRING 		DesktopInfo;
	UNICODE_STRING 		ShellInfo;
	UNICODE_STRING 		RuntimeData;
	RTL_DRIVE_LETTER_CURDIR CurrentDirectories[32];
	ULONG_PTR 			EnvironmentSize;
	ULONG_PTR 			EnvironmentVersion;
	PVOID 				PackageDependencyData;
	ULONG 				ProcessGroupId;
	ULONG 				LoaderThreads;
	UNICODE_STRING 		RedirectionDllName;
	UNICODE_STRING 		HeapPartitionName;
	ULONGLONG 			*DefaultThreadpoolCpuSetMasks;
	ULONG 				DefaultThreadpoolCpuSetMaskCount;
	ULONG 				DefaultThreadpoolThreadMaximum;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB_LDR_DATA {
  ULONG Length;                                
  UCHAR Initialized;                          
  PVOID SsHandle;                              
  LIST_ENTRY InLoadOrderModuleList;            
  LIST_ENTRY InMemoryOrderModuleList;          
  LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;               // +0x00
    LIST_ENTRY InMemoryOrderLinks;             // +0x10
    LIST_ENTRY InInitializationOrderLinks;     // +0x20
    PVOID DllBase;                             // +0x30
    PVOID EntryPoint;                          // +0x38
    ULONG SizeOfImage;                         // +0x40
    UNICODE_STRING FullDllName;                // +0x48
    UNICODE_STRING BaseDllName;                // +0x58
    ULONG Flags;                               // +0x68
    USHORT LoadCount;                          // +0x6C
    USHORT TlsIndex;                           // +0x6E
    LIST_ENTRY HashLinks;                      // +0x70
    ULONG TimeDateStamp;                       // +0x80
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

VOID NTAPI PS_POST_PROCESS_INIT_ROUTINE(VOID);
typedef VOID (NTAPI *PPS_POST_PROCESS_INIT_ROUTINE)(VOID);

typedef struct _PEB {
  BYTE                          InheritedAddressSpace;
  BYTE                          ReadImageFileExecOptions;
  BYTE                          BeingDebugged;
  BYTE                          BitField;
  PVOID                         Mutant;
  PVOID                         ImageBaseAddress;
  PPEB_LDR_DATA                 Ldr;
  PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
  PVOID                         Reserved4[3];
  PVOID                         AtlThunkSListPtr;
  PVOID                         Reserved5;
  ULONG                         Reserved6;
  PVOID                         Reserved7;
  ULONG                         Reserved8;
  ULONG                         AtlThunkSListPtr32;
  PVOID                         Reserved9[45];
  BYTE                          Reserved10[96];
  PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
  BYTE                          Reserved11[128];
  PVOID                         Reserved12[1];
  ULONG                         SessionId;
} PEB, *PPEB;

typedef NTSTATUS (NTAPI *ZwUnmapViewOfSection_t)(HANDLE hProcess, PVOID baseAddress);

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation = 0
} PROCESSINFOCLASS;

typedef NTSTATUS (NTAPI *NtQueryInformationProcess_t)(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);

HANDLE obtainProcessHandle(DWORD pid);
HMODULE loadNTDLLModule();
NtQueryInformationProcess_t obtainNtQueryInformationProcessAddress(HMODULE ntdll);
PEB obtainProcessEnvironmentBlock(NtQueryInformationProcess_t NtQueryInformationProcess, HANDLE hProcess);
PEB_LDR_DATA loadLDRData(HANDLE hProcess, PEB peb);
void showLoadedModules(HWND hListView, HANDLE hProcess, PEB peb);