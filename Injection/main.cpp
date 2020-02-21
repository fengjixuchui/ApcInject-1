#include<Windows.h>
#include<stdio.h>
#include <Tlhelp32.h>

DWORD GetProcessIdByName(LPCTSTR lpszProcessName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof pe;

	if (Process32First(hSnapshot, &pe))
	{
		do {
			if (lstrcmpi(lpszProcessName, pe.szExeFile) == 0)
			{
				CloseHandle(hSnapshot);
				return pe.th32ProcessID;
			}
		} while (Process32Next(hSnapshot, &pe));
	}

	CloseHandle(hSnapshot);
	return 0;
}


BOOL GetAllThreadIdByProcessId(DWORD dwProcessId)
{

	DWORD dwBufferLength = 1000;
	THREADENTRY32 te32 = { 0 };
	HANDLE hSnapshot = NULL;
	BOOL bRet = TRUE;


		// ��ȡ�߳̿���
		::RtlZeroMemory(&te32, sizeof(te32));
		te32.dwSize = sizeof(te32);
		hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

		// ��ȡ��һ���߳̿�����Ϣ
		bRet = ::Thread32First(hSnapshot, &te32);
		while (bRet)
		{
			// ��ȡ���̶�Ӧ���߳�ID
			if (te32.th32OwnerProcessID == dwProcessId)
			{
				return te32.th32ThreadID;
			}

			// ������һ���߳̿�����Ϣ
			bRet = ::Thread32Next(hSnapshot, &te32);
		}
		return 0;
}

int main() {
	FARPROC pLoadLibrary = NULL;
	HANDLE hThread = NULL;
	HANDLE hProcess = 0;
	DWORD Threadid = 0;
	DWORD ProcessId = 0;
	BYTE DllName[] = "C:\\Users\\Black Sheep\\source\\repos\\ApcInject\\x64\\Debug\\TestDll.dll";
	LPVOID AllocAddr = NULL;

	ProcessId = GetProcessIdByName(L"explorer.exe");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessId);
	pLoadLibrary = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	AllocAddr = VirtualAllocEx(hProcess, 0, sizeof(DllName)+1, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, AllocAddr, DllName, sizeof(DllName) + 1, 0);
	Threadid = GetAllThreadIdByProcessId(ProcessId);
	hThread = OpenThread(THREAD_ALL_ACCESS, 0, Threadid);
	QueueUserAPC((PAPCFUNC)pLoadLibrary, hThread, (ULONG_PTR)AllocAddr);
	BOOL ret = GetLastError();
	return 0;
		
}