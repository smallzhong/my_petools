#include "StdAfx.h"
#include "head.h"

extern LPVOID pFileBuffer;
extern PIMAGE_DOS_HEADER pDosHeader;
extern PIMAGE_NT_HEADERS32 pNTHeader;
extern PIMAGE_SECTION_HEADER pSectionHeader;
extern DWORD file_size;

extern HINSTANCE g_hInstance;
extern TCHAR g_strFilename[MAX_PATH];

#define SETTEXT(num1, num2)                                                    \
    do                                                                         \
    {                                                                          \
        hEditUser = GetDlgItem(hwndDlg, IDC_EDIT##num1);                       \
        sprintf(                                                               \
            string_buffer, "%08x",                                             \
            pNTHeader->OptionalHeader.DataDirectory[num1 - 4].VirtualAddress); \
        SetWindowText(hEditUser, string_buffer);                               \
                                                                               \
        memset(string_buffer, 0, sizeof(string_buffer));                       \
        sprintf(string_buffer, "%08x",                                         \
                pNTHeader->OptionalHeader.DataDirectory[num1 - 4].Size);       \
        hEditUser = GetDlgItem(hwndDlg, IDC_EDIT##num2);                       \
        SetWindowText(hEditUser, string_buffer);                               \
    } while (0)

BOOL CALLBACK DataDirectoryViewerDlgProc(HWND hwndDlg,   // handle to dialog box
										 UINT uMsg,      // message
										 WPARAM wParam,  // first message parameter
										 LPARAM lParam   // second message parameter
										 )
{
    HWND hEditUser = NULL;     // 一个指向文本框的句柄
    TCHAR szUserBuff[0x50];    // 用来记录文本框文字的缓冲区
    char string_buffer[0x50];  // 用来存储sprintf结果的缓冲区
    int i = 0;

    switch (uMsg)
    {
        case WM_INITDIALOG:

            SETTEXT(4, 19);
            SETTEXT(5, 20);
            SETTEXT(6, 21);
            SETTEXT(7, 22);
            SETTEXT(8, 23);
            SETTEXT(9, 24);
            SETTEXT(10, 25);
            SETTEXT(11, 26);
            SETTEXT(12, 27);
            SETTEXT(13, 28);
            SETTEXT(14, 29);
            SETTEXT(15, 30);
            SETTEXT(16, 31);
            SETTEXT(17, 32);
            SETTEXT(18, 33);

            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wParam))
			{
			case IDC_BUTTON2:
				DialogBox(g_hInstance,
					MAKEINTRESOURCE(IDD_DIALOG_IMPORT_TABLE), NULL,
									ImportTableDlgProc);
				break;
			}
			return TRUE;

        case WM_CLOSE:
            EndDialog(hwndDlg, 0);
            return TRUE;
    }
    return FALSE;
}
