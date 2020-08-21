#include "StdAfx.h"
#include "head.h"

extern LPVOID pFileBuffer;
extern PIMAGE_DOS_HEADER pDosHeader;
extern PIMAGE_NT_HEADERS32 pNTHeader;
extern PIMAGE_SECTION_HEADER pSectionHeader;
extern DWORD file_size;

extern HINSTANCE g_hInstance;
extern TCHAR g_strFilename[MAX_PATH];

char string_buffer[0x50];  // 用来存储sprintf结果的缓冲区

static VOID init_headers_section(HWND hwndDlg, HWND hListSectionInfo);  // 设置列表头信息
static VOID set_section_items(HWND hwndDlg, HWND hListSectionInfo);  // 设置列表内信息

BOOL CALLBACK SectionViewerDlgProc(HWND hwndDlg,   // handle to dialog box
                                   UINT uMsg,      // message
                                   WPARAM wParam,  // first message parameter
                                   LPARAM lParam   // second message parameter
)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:

            HWND hListSectionInfo;
            //获取IDC_LIST_SECTION_VIEWER句柄
            hListSectionInfo = GetDlgItem(hwndDlg, IDC_LIST_SECTION_VIEWER);

            init_headers_section(hwndDlg, hListSectionInfo);

            set_section_items(hwndDlg, hListSectionInfo);

            return TRUE;

        case WM_CLOSE:
            EndDialog(hwndDlg, 0);
            return TRUE;
            break;
    }

    return FALSE;
}

static VOID init_headers_section(HWND hwndDlg, HWND hListSectionInfo)
{
    LV_COLUMN lv;

    //初始化
    memset(&lv, 0, sizeof(LV_COLUMN));

    //设置整行选中
    SendMessage(hListSectionInfo, LVM_SETEXTENDEDLISTVIEWSTYLE,
                LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //第一列
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = TEXT("名称");  //列标题
    lv.cx = 120;                //列宽
    lv.iSubItem = 0;
    // ListView_InsertColumn(hListProcess, 0, &lv);
    SendMessage(hListSectionInfo, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

    //第二列
    lv.pszText = TEXT("内存中偏移");
    lv.cx = 100;
    lv.iSubItem = 1;
    // ListView_InsertColumn(hListProcess, 1, &lv);
    SendMessage(hListSectionInfo, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
    //第三列
    lv.pszText = TEXT("内存中大小");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListSectionInfo, 2, &lv);
    //第四列
    lv.pszText = TEXT("文件中偏移");
    lv.cx = 100;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListSectionInfo, 3, &lv);
    //第五列
    lv.pszText = TEXT("文件中大小");
    lv.cx = 100;
    lv.iSubItem = 4;
    ListView_InsertColumn(hListSectionInfo, 4, &lv);
    //第五列
    lv.pszText = TEXT("标志");
    lv.cx = 100;
    lv.iSubItem = 5;
    ListView_InsertColumn(hListSectionInfo, 5, &lv);
}

static VOID set_section_items(HWND hwndDlg, HWND hListSectionInfo)
{
    LV_ITEM vitem;

    //初始化
    memset(&vitem, 0, sizeof(LV_ITEM));
    vitem.mask = LVIF_TEXT;

    for (int i = 0; i < pNTHeader->FileHeader.NumberOfSections;
         i++)  // 遍历整个节表
    {
        memset(string_buffer, 0, sizeof(string_buffer));
        sprintf(string_buffer, "%s", pSectionHeader[i].Name);
        vitem.pszText = string_buffer;
        vitem.iItem = i;
        vitem.iSubItem = 0;
        // ListView_InsertItem(hListProcess, &vitem);
        SendMessage(hListSectionInfo, LVM_INSERTITEM, 0, (DWORD)&vitem);

        memset(string_buffer, 0, sizeof(string_buffer));
        sprintf(string_buffer, "%08x", pSectionHeader[i].VirtualAddress); 
        vitem.pszText = string_buffer;
        vitem.iItem = i;
        vitem.iSubItem = 1;
        ListView_SetItem(hListSectionInfo, &vitem);
		
		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%08x", pSectionHeader[i].Misc.VirtualSize);
		vitem.pszText = string_buffer;
		vitem.iItem = i;
		vitem.iSubItem = 2;
		ListView_SetItem(hListSectionInfo, &vitem);

		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%08x", pSectionHeader[i].PointerToRawData);
		vitem.pszText = string_buffer;
		vitem.iItem = i;
		vitem.iSubItem = 3;
		ListView_SetItem(hListSectionInfo, &vitem);

		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%08x", pSectionHeader[i].SizeOfRawData);
		vitem.pszText = string_buffer;
		vitem.iItem = i;
		vitem.iSubItem = 4;
		ListView_SetItem(hListSectionInfo, &vitem);

		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%08x", pSectionHeader[i].Characteristics);
		vitem.pszText = string_buffer;
		vitem.iItem = i;
		vitem.iSubItem = 5;
		ListView_SetItem(hListSectionInfo, &vitem);
    }
}