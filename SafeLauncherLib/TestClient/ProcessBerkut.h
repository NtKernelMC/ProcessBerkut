/*
	Process Berkut Library
	Module for AntiCheat Client
	Task: Prevent Process Starter Injection
	Copyright: NtKernelMC (c)

	> [ENG] FEATURES ================================
	~ Protected from intercepting process handle by ZwCreateUserProcess hooks
	~ Protected from launching process by fake launcher
	~ Hard and tangled algorithm for launcher -> client communication
	~ Support for x86-x64 architectures and Windows Vista+ OS
	> [RUS] Функционал ================================
	~ Защита от перехвата процесс-хендла ZwCreateUserProcess хукам
	~ Защита от запуска процесса игры с помощью фейкового лаунчера
	~ Сложный и запутаный алгоритм общения между модулями лаунчера и клиента
	~ Поддержка х86-х64 битных архитектур и оперативных систем Windows начиная с Vista и выше
*/
#pragma once
#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <map>
#include <winternl.h>
#include <vector>
#include <tuple>
namespace SafeLaunch
{
	class ProcessGate sealed
	{
	private:
		static void checkForSafeLaunch()
		{
			THREADENTRY32 th32; HANDLE hSnapshot = NULL; th32.dwSize = sizeof(THREADENTRY32);
			hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
			if (Thread32First(hSnapshot, &th32))
			{
				do
				{
					if (th32.th32OwnerProcessID == GetCurrentProcessId() && th32.th32ThreadID != GetCurrentThreadId())
					{
						typedef NTSTATUS(__stdcall *tNtQueryInformationThread)
						(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass,
						PVOID ThreadInformation, ULONG ThreadInformationLength,
						PULONG ReturnLength); tNtQueryInformationThread NtQueryInformationThread =
						(tNtQueryInformationThread)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread");
						HANDLE pThread = OpenThread(THREAD_ALL_ACCESS, FALSE, th32.th32ThreadID);
						if (pThread)
						{
							MEMORY_BASIC_INFORMATION mbi = { 0 }; DWORD_PTR tempBase = 0x0;
							SuspendThread(pThread); CONTEXT context = { 0 };
							NtQueryInformationThread(pThread, (THREADINFOCLASS)9, &tempBase, sizeof(DWORD_PTR), NULL);
							VirtualQuery((void*)tempBase, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
							if (tempBase >= (DWORD_PTR)GetModuleHandleA(NULL) && tempBase <=
							((DWORD_PTR)GetModuleHandleA(NULL) + mbi.RegionSize))
							{
								context.ContextFlags = CONTEXT_ALL; 
								GetThreadContext(pThread, &context);
								if (context.Dr7 != NULL)
								{
									DWORD_PTR ctrlAddr = context.Dr2;
									if (ctrlAddr != NULL)
									{
										BYTE nop[1] = { 0x0 }; memcpy(nop, (void*)ctrlAddr, 0x1);
										if (nop[0] == 0x90)
										{
											VirtualFree((void*)ctrlAddr, 0, MEM_RELEASE);
											context.Dr2 = 0x0; context.Dr7 = 0x0;
											SetThreadContext(pThread, &context);
											ResumeThread(pThread); CloseHandle(pThread);
										}
										else memcpy((void*)0xFFFFFF, (void*)0xFFFFFF, 222222);
									}
									else memcpy((void*)0xFFFFFF, (void*)0xFFFFFF, 222222);
								}
								else memcpy((void*)0xFFFFFF, (void*)0xFFFFFF, 222222);
							}
							else
							{
								ResumeThread(pThread); CloseHandle(pThread);
							}
						}
					}
				}
				while (Thread32Next(hSnapshot, &th32));
			}
			if (hSnapshot != NULL) CloseHandle(hSnapshot);
		}
	public:
		void CheckForSafeLaunch()
		{
			HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)checkForSafeLaunch, 0, 0, 0);
			WaitForSingleObject(hThread, INFINITE);
		}
	};
}