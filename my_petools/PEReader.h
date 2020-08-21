#ifndef PEREADER_H_
#define PEREADER_H_

#include <Commdlg.h>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
// #include <atlconv.h>

//选择文件夹对话框
#include <Shlobj.h>
#include "Utlis.h"
#pragma comment(lib, "Shell32.lib")

DWORD ReadPEFile(IN LPCSTR file_in, OUT LPVOID& pFileBuffer,
				 PIMAGE_DOS_HEADER& pDosHeader, PIMAGE_NT_HEADERS32& pNTHeader,
				 PIMAGE_SECTION_HEADER& pSectionHeader);

DWORD RVA_TO_FOA(LPVOID pFileBuffer, PIMAGE_DOS_HEADER pDosHeader,
				 PIMAGE_NT_HEADERS32 pNTHeader,
				 PIMAGE_SECTION_HEADER pSectionHeader, IN DWORD RVA);

DWORD FOA_TO_RVA(LPVOID pFileBuffer, PIMAGE_DOS_HEADER pDosHeader,
				 PIMAGE_NT_HEADERS32 pNTHeader,
    PIMAGE_SECTION_HEADER pSectionHeader, IN DWORD FOA);



#endif // PEREADER_H_