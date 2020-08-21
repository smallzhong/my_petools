// my_petools.cpp : Defines the entry point for the application.
//

#include "Stdafx.h"
#include "resource.h"
#include "head.h"
#include "Utlis.h"

LPVOID pFileBuffer = NULL;
PIMAGE_DOS_HEADER pDosHeader = NULL;
PIMAGE_NT_HEADERS32 pNTHeader = NULL;
PIMAGE_SECTION_HEADER pSectionHeader = NULL;
PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL;
PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor_origin = NULL;
DWORD file_size;

HINSTANCE g_hInstance;
TCHAR g_strFilename[MAX_PATH] = { 0 }; //用于接收文件名

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
	INITCOMMONCONTROLSEX icex;				
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);				
	icex.dwICC = ICC_WIN95_CLASSES;				
	InitCommonControlsEx(&icex);				

    g_hInstance = hInstance;
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, MainDlgProc);

    return 0;
}

