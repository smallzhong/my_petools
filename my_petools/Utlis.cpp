#include "StdAfx.h"
#include "Utlis.h"

/*
#include <stdio.h>
#include <MALLOC.H>
#include <windows.h>
#include <LOCALE.h>
#include <TLHELP32.H>
#include "psapi.h""
#pragma comment(lib,"psapi.lib")

#include <vector>

#ifdef _UNICODE
#define _tprintf wprintf
#else
#define _tprintf printf
#endif

struct ModuleInfo
{
	TCHAR szExeFile[MAX_PATH];	// 模块文件名
	DWORD ImageBase;
	DWORD SizeOfImage;
};

struct ProcessInfo
{
	ModuleInfo MainModuleInfo;	// 主模块信息
	DWORD dwPID;				// 进程ID
	ModuleInfo *modules;		// 子模块数组
	DWORD dwModules;			// 子模块数量

// 	~ProcessInfo()
// 	{
// 		free(modules);
// 	}
};

DWORD TakeProcessSnapshot(std::vector<ProcessInfo> &processInfos);
DWORD EnumModulesHandle(HANDLE hProcess, HMODULE **lpModule);

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	std::vector<ProcessInfo> processInfos;
	TakeProcessSnapshot(processInfos);
	
	for (int i = 0; i < processInfos.size(); i++)
	{
		_tprintf(TEXT("PID: %6d\t%s\n"), processInfos[i].dwPID, processInfos[i].MainModuleInfo.szExeFile);
		for (int j = 0; j < processInfos[i].dwModules; j++)
		{
			_tprintf(TEXT("\t\t%x\t%x\t%s\n"), processInfos[i].modules[j].ImageBase, processInfos[i].modules[j].SizeOfImage, processInfos[i].modules[j].szExeFile);
		}
	}
	printf("Bye\n");
	getchar();
	return 0;
}

// 枚举进程地址空间内的模块句柄，返回数组长度
DWORD EnumModulesHandle(HANDLE hProcess, HMODULE **lpModule)
{
	DWORD cbBytesNeeded = 0;
	// 备注：EnumProcessModules 函数无法枚举64位进程的模块，除非程序以64位编译
	EnumProcessModules(hProcess, NULL, 0, &cbBytesNeeded); // 计算数组大小
	*lpModule = (HMODULE *)malloc(cbBytesNeeded + 0x1000);
	EnumProcessModules(hProcess, *lpModule, cbBytesNeeded + 0x1000, &cbBytesNeeded); // 枚举模块句柄
	return cbBytesNeeded / sizeof(HMODULE);
}

// 获取当前所有进程的信息
DWORD TakeProcessSnapshot(std::vector<ProcessInfo> &processInfos)
{
	processInfos.clear();
	// 获取进程快照，得到当前所有进程的PID
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnapshot == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, TEXT("获取进程快照失败"), TEXT("Error"), MB_OK);
		return -1;
	}	
	// 遍历进程
	BOOL bNext = Process32First(hProcessSnapshot, &pe32);
	while (bNext)
	{
		ProcessInfo psi;
		memset(&psi, 0, sizeof(ProcessInfo));
		lstrcpy(psi.MainModuleInfo.szExeFile, pe32.szExeFile);
		psi.dwPID = pe32.th32ProcessID;		
		
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, psi.dwPID);
		if (hProcess != NULL)
		{
			HMODULE *lpModuleHandle = NULL;
			psi.dwModules = EnumModulesHandle(hProcess, &lpModuleHandle);
			psi.modules = (ModuleInfo*)malloc(psi.dwModules * sizeof(ModuleInfo));
			MODULEINFO moduleInfo;
			for (size_t i = 0; i < psi.dwModules; i++)
			{
				GetModuleInformation(hProcess, lpModuleHandle[i], &moduleInfo, sizeof(MODULEINFO));
				//printf("\t%x\t%x\n", lpModuleHandle[i], moduleInfo.SizeOfImage);
				GetModuleFileNameEx(hProcess, lpModuleHandle[i], (psi.modules)[i].szExeFile, MAX_PATH);
				(psi.modules)[i].ImageBase = (DWORD)(lpModuleHandle[i]);
				(psi.modules)[i].SizeOfImage = moduleInfo.SizeOfImage;
			}
			free(lpModuleHandle);
		}
		processInfos.push_back(psi);
		bNext = Process32Next(hProcessSnapshot, &pe32);
	}
	return 0;
}

*/

void __cdecl OutputDebugStringF(const char *format, ...)  
{  
    va_list vlArgs;  
    char    *strBuffer = (char*)GlobalAlloc(GPTR, 4096);  
	
    va_start(vlArgs, format);  
    _vsnprintf(strBuffer, 4096 - 1, format, vlArgs);  
    va_end(vlArgs);  
    strcat(strBuffer, "\n");  
    OutputDebugStringA(strBuffer);  
    GlobalFree(strBuffer);  
    return;  
}


char* TCHARToChar(const TCHAR* pTchar)
{
    char* pChar = NULL;
#ifdef _UNICODE
    int nLen = wcslen(pTchar) + 1;
    pChar = new char[nLen * 2];
    WideCharToMultiByte(CP_ACP, 0, pTchar, nLen, pChar, 2 * nLen, NULL, NULL);
#else
    int nLen = strlen(pTchar) + 1;
    pChar = new char[nLen];
    memcpy(pChar, pTchar, nLen*sizeof(char));
#endif
    return pChar;
}
