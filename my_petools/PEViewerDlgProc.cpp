#include "StdAfx.h"
#include "head.h"

extern LPVOID pFileBuffer;
extern PIMAGE_DOS_HEADER pDosHeader;
extern PIMAGE_NT_HEADERS32 pNTHeader;
extern PIMAGE_SECTION_HEADER pSectionHeader;
extern PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor;
extern PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor_origin;
extern DWORD file_size;

extern HINSTANCE g_hInstance;
extern TCHAR g_strFilename[MAX_PATH];

BOOL CALLBACK PEViewerDlgProc(									
							  HWND hwndDlg,  // handle to dialog box			
							  UINT uMsg,     // message			
							  WPARAM wParam, // first message parameter			
							  LPARAM lParam  // second message parameter			
							  )
{
	
	HWND hEditUser = NULL; // 一个指向文本框的句柄
	TCHAR szUserBuff[0x50]; // 用来记录文本框文字的缓冲区
	char string_buffer[0x50]; // 用来存储sprintf结果的缓冲区
	
    switch (uMsg)
    {
	case WM_INITDIALOG:

		file_size = ReadPEFile(g_strFilename, pFileBuffer, pDosHeader,
			pNTHeader, pSectionHeader);
		
		MY_ASSERT(pDosHeader->e_magic == IMAGE_DOS_SIGNATURE);
		
		pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(
			(PBYTE)pFileBuffer +
			RVA_TO_FOA(pFileBuffer, pDosHeader, pNTHeader, pSectionHeader,
			pNTHeader->OptionalHeader.DataDirectory[1].VirtualAddress));
		MY_ASSERT(pImportDescriptor);
		pImportDescriptor_origin = pImportDescriptor;
		
		hEditUser = GetDlgItem(hwndDlg,IDC_EDIT_OEP);
		sprintf(string_buffer, "%x", pNTHeader->OptionalHeader.AddressOfEntryPoint);
		SetWindowText(hEditUser, string_buffer);
		
		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%x", pNTHeader->OptionalHeader.ImageBase);
		hEditUser = GetDlgItem(hwndDlg,IDC_EDIT_IMAGE_BASE);
		SetWindowText(hEditUser, string_buffer);
		
		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%x", pNTHeader->OptionalHeader.SizeOfImage);
		hEditUser = GetDlgItem(hwndDlg,IDC_EDIT_SIZEOFIMAGE);
		SetWindowText(hEditUser, string_buffer);
		
		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%x", pNTHeader->FileHeader.NumberOfSections);
		hEditUser = GetDlgItem(hwndDlg,IDC_EDIT_NUMBEROFSECTIONS);
		SetWindowText(hEditUser, string_buffer);
		
		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%x", pNTHeader->OptionalHeader.SizeOfHeaders);
		hEditUser = GetDlgItem(hwndDlg,IDC_EDIT_SIZEOFHEADERS);
		SetWindowText(hEditUser, string_buffer);
		
		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%x", pDosHeader->e_lfanew);
		hEditUser = GetDlgItem(hwndDlg,IDC_EDIT_ELFANEW);
		SetWindowText(hEditUser, string_buffer);
		
		return TRUE;
		
	case WM_CLOSE:
		//PostQuitMessage(0);
		EndDialog(hwndDlg, 0);
		
		break;
		
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_SECTION:
			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_SECTION_VIEWER), NULL, SectionViewerDlgProc);
			break;
		case IDC_BUTTON_DATA_DIRECTORY_VIEWER:
			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_DATA_DIRECTORY), NULL, DataDirectoryViewerDlgProc);
		}
		
		break;
    }
	
    return FALSE;
}

