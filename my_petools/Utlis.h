#ifndef UTLIS_H_
#define UTLIS_H_

#include <Commdlg.h>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
// #include <atlconv.h>

//选择文件夹对话框
#include <Shlobj.h>
#pragma comment(lib, "Shell32.lib")

void __cdecl OutputDebugStringF(const char *format, ...); 
#ifdef _DEBUG  
#define DbgPrintf   OutputDebugStringF  
#else  
#define DbgPrintf  
#endif

char* TCHARToChar(const TCHAR* pTchar);

#endif // UTLIS_H_