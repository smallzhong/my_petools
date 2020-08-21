#include "StdAfx.h"
#include "head.h"

extern LPVOID pFileBuffer;
extern PIMAGE_DOS_HEADER pDosHeader;
extern PIMAGE_NT_HEADERS32 pNTHeader;
extern PIMAGE_SECTION_HEADER pSectionHeader;

extern DWORD file_size;

extern HINSTANCE g_hInstance;
extern TCHAR g_strFilename[MAX_PATH];

static char string_buffer[0x50];  // 用来存储sprintf结果的缓冲区
extern PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor;
extern PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor_origin;

static VOID init_headers_import_table(
    HWND hwndDlg, HWND hListImportTableInfo);  // 设置列表头信息
static VOID init_headers_thunk_data(
    HWND hwndDlg, HWND hListImportTableInfo);  // 设置列表头信息
static VOID set_import_table_items(
    HWND hwndDlg, HWND hListImportTableInfo);  // 设置列表内信息
static VOID set_thunk_data_info(HWND hListImportTableInfo, HWND hThunkDataInfo,
                                WPARAM wParam, LPARAM lParam, HWND hwndDlg);  // 设置列表内信息

BOOL CALLBACK ImportTableDlgProc(HWND hwndDlg,   // handle to dialog box
                                 UINT uMsg,      // message
                                 WPARAM wParam,  // first message parameter
                                 LPARAM lParam   // second message parameter
)
{
    HWND hListImportTableInfo;
    HWND hListThunkDataInfo;
    switch (uMsg)
    {
        case WM_INITDIALOG:

            //获取IDC_LIST_SECTION_VIEWER句柄
            hListImportTableInfo = GetDlgItem(hwndDlg, IDC_LIST_IMPORT_TABLE);
            MY_ASSERT(hListImportTableInfo);

            hListThunkDataInfo = GetDlgItem(hwndDlg, IDC_LIST_THUNK_DATA);
            MY_ASSERT(hListThunkDataInfo);
            init_headers_import_table(hwndDlg, hListImportTableInfo);
            init_headers_thunk_data(hwndDlg, hListThunkDataInfo);

            set_import_table_items(hwndDlg, hListImportTableInfo);

            return TRUE;

        case WM_CLOSE:
            EndDialog(hwndDlg, 0);
            return TRUE;
            break;
        case WM_NOTIFY:
            NMHDR* pnmhdr = (NMHDR*)lParam;
            if (wParam == IDC_LIST_IMPORT_TABLE && pnmhdr->code == NM_CLICK)
            {
                // set_thunk_data_info(hListImportTableInfo, wParam, lParam); //
                // 这样不行

                set_thunk_data_info(GetDlgItem(hwndDlg, IDC_LIST_IMPORT_TABLE),
                                    GetDlgItem(hwndDlg, IDC_LIST_THUNK_DATA),
                                    wParam, lParam, hwndDlg);
            }
    }

    return FALSE;
}

static VOID set_thunk_data_info(HWND hListImportTableInfo, HWND hThunkDataInfo,
                                WPARAM wParam, LPARAM lParam, HWND hwndDlg)
{
    DWORD selected_line = 0;
    selected_line =
        SendMessage(hListImportTableInfo, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    if (selected_line == -1)
        return;  // 如果没有选中任何一行直接返回。防止[-1]导致错误

	// ListView_DeleteAllItems(hListThunkDataInfo); // 这样不行，要及时获取才行
	ListView_DeleteAllItems(GetDlgItem(hwndDlg, IDC_LIST_THUNK_DATA)); // 先将之前列表里面的内容全部删除

    PDWORD pThunkData_INT = NULL;
    pThunkData_INT = (PDWORD)(
        (PBYTE)pFileBuffer +
        RVA_TO_FOA(pFileBuffer, pDosHeader, pNTHeader, pSectionHeader,
                   pImportDescriptor_origin[selected_line].OriginalFirstThunk));
    PIMAGE_IMPORT_BY_NAME pImportByName = NULL;

    LV_ITEM vitem;  //初始化
    memset(&vitem, 0, sizeof(LV_ITEM));
    vitem.mask = LVIF_TEXT;

    int ct = 0;  // 用来记录当前是第几行（ct + 1）

    while (*pThunkData_INT)
    {
        if ((*pThunkData_INT) & 0x80000000)
        {
			memset(string_buffer, 0, sizeof(string_buffer));
            sprintf(
                string_buffer, "%08X",
                (pImportDescriptor_origin[selected_line].OriginalFirstThunk +
				ct * 4));
            vitem.pszText = string_buffer;
            vitem.iItem = ct;
            vitem.iSubItem = 0;
            // ListView_InsertItem(hListProcess, &vitem);
            SendMessage(hThunkDataInfo, LVM_INSERTITEM, 0, (DWORD)&vitem);
			
            memset(string_buffer, 0, sizeof(string_buffer));
            sprintf(string_buffer, "%08X",
				RVA_TO_FOA(pFileBuffer, pDosHeader, pNTHeader,
				pSectionHeader,
				(pImportDescriptor_origin[selected_line]
				.OriginalFirstThunk +
				ct * 4)));
            vitem.pszText = string_buffer;
            vitem.iItem = ct;
            vitem.iSubItem = 1;
            // ListView_InsertItem(hListProcess, &vitem);
            ListView_SetItem(hThunkDataInfo, &vitem);
			
            memset(string_buffer, 0, sizeof(string_buffer));
            sprintf(string_buffer, "%08X", *pThunkData_INT);
            vitem.pszText = string_buffer;
            vitem.iItem = ct;
            vitem.iSubItem = 2;
            ListView_SetItem(hThunkDataInfo, &vitem);

			memset(string_buffer, 0, sizeof(string_buffer));
            sprintf(string_buffer, "-");
            vitem.pszText = string_buffer;
            vitem.iItem = ct;
            vitem.iSubItem = 3;
            ListView_SetItem(hThunkDataInfo, &vitem);

			memset(string_buffer, 0, sizeof(string_buffer));
            sprintf(string_buffer, "函数序号：%04xh %dd", (*pThunkData_INT) & 0x7fffffff, (*pThunkData_INT) & 0x7fffffff);
            vitem.pszText = string_buffer;
            vitem.iItem = ct;
            vitem.iSubItem = 4;
            ListView_SetItem(hThunkDataInfo, &vitem);

        }
        else
        {
            pImportByName = (PIMAGE_IMPORT_BY_NAME)(
                (PBYTE)pFileBuffer +
                RVA_TO_FOA(pFileBuffer, pDosHeader, pNTHeader, pSectionHeader,
                           (*pThunkData_INT) & 0x7fffffff));
            MY_ASSERT(pImportByName);

            memset(string_buffer, 0, sizeof(string_buffer));
            sprintf(
                string_buffer, "%08X",
                (pImportDescriptor_origin[selected_line].OriginalFirstThunk +
                 ct * 4));
            vitem.pszText = string_buffer;
            vitem.iItem = ct;
            vitem.iSubItem = 0;
            // ListView_InsertItem(hListProcess, &vitem);
            SendMessage(hThunkDataInfo, LVM_INSERTITEM, 0, (DWORD)&vitem);

            memset(string_buffer, 0, sizeof(string_buffer));
            sprintf(string_buffer, "%08X",
                    RVA_TO_FOA(pFileBuffer, pDosHeader, pNTHeader,
                               pSectionHeader,
                               (pImportDescriptor_origin[selected_line]
                                    .OriginalFirstThunk +
                                ct * 4)));
            vitem.pszText = string_buffer;
            vitem.iItem = ct;
            vitem.iSubItem = 1;
            // ListView_InsertItem(hListProcess, &vitem);
            ListView_SetItem(hThunkDataInfo, &vitem);

            memset(string_buffer, 0, sizeof(string_buffer));
            sprintf(string_buffer, "%08X", *pThunkData_INT);
            vitem.pszText = string_buffer;
            vitem.iItem = ct;
            vitem.iSubItem = 2;
            ListView_SetItem(hThunkDataInfo, &vitem);

			memset(string_buffer, 0, sizeof(string_buffer));
            sprintf(string_buffer, "%04X", pImportByName->Hint);
            vitem.pszText = string_buffer;
            vitem.iItem = ct;
            vitem.iSubItem = 3;
            ListView_SetItem(hThunkDataInfo, &vitem);

			memset(string_buffer, 0, sizeof(string_buffer));
            sprintf(string_buffer, "%s", pImportByName->Name);
            vitem.pszText = string_buffer;
            vitem.iItem = ct;
            vitem.iSubItem = 4;
            ListView_SetItem(hThunkDataInfo, &vitem);
        }
        ct++;
        pThunkData_INT++;
    }
}

static VOID set_import_table_items(HWND hwndDlg, HWND hListImportTableInfo)
{
    LV_ITEM vitem;

    //初始化
    memset(&vitem, 0, sizeof(LV_ITEM));
    vitem.mask = LVIF_TEXT;

    int ct = 0;  // iItem的值
    while (pImportDescriptor->Name != 0 || pImportDescriptor->FirstThunk != 0)
    {
        memset(string_buffer, 0, sizeof(string_buffer));
        sprintf(string_buffer, "%s",
                (PCHAR)(PBYTE)pFileBuffer +
                    RVA_TO_FOA(pFileBuffer, pDosHeader, pNTHeader,
                               pSectionHeader, pImportDescriptor->Name));
        vitem.pszText = string_buffer;
        vitem.iItem = ct;
        vitem.iSubItem = 0;
        // ListView_InsertItem(hListProcess, &vitem);
        SendMessage(hListImportTableInfo, LVM_INSERTITEM, 0, (DWORD)&vitem);

        memset(string_buffer, 0, sizeof(string_buffer));
        sprintf(string_buffer, "%08X", pImportDescriptor->OriginalFirstThunk);
        vitem.pszText = string_buffer;
        vitem.iItem = ct;
        vitem.iSubItem = 1;
        ListView_SetItem(hListImportTableInfo, &vitem);

        memset(string_buffer, 0, sizeof(string_buffer));
        sprintf(string_buffer, "%08X", pImportDescriptor->TimeDateStamp);
        vitem.pszText = string_buffer;
        vitem.iItem = ct;
        vitem.iSubItem = 2;
        ListView_SetItem(hListImportTableInfo, &vitem);

        memset(string_buffer, 0, sizeof(string_buffer));
        sprintf(string_buffer, "%08X", pImportDescriptor->ForwarderChain);
        vitem.pszText = string_buffer;
        vitem.iItem = ct;
        vitem.iSubItem = 3;
        ListView_SetItem(hListImportTableInfo, &vitem);

        memset(string_buffer, 0, sizeof(string_buffer));
        sprintf(string_buffer, "%08X", pImportDescriptor->Name);
        vitem.pszText = string_buffer;
        vitem.iItem = ct;
        vitem.iSubItem = 4;
        ListView_SetItem(hListImportTableInfo, &vitem);

        memset(string_buffer, 0, sizeof(string_buffer));
        sprintf(string_buffer, "%08X", pImportDescriptor->FirstThunk);
        vitem.pszText = string_buffer;
        vitem.iItem = ct;
        vitem.iSubItem = 5;
        ListView_SetItem(hListImportTableInfo, &vitem);

        ct++;                 // iItem向后移动
        pImportDescriptor++;  // 将pImportDescriptor向后移动
    }
}

static VOID init_headers_thunk_data(
    HWND hwndDlg, HWND hListImportTableInfo)  // 设置列表头信息
{
    LV_COLUMN lv;

    //初始化
    memset(&lv, 0, sizeof(LV_COLUMN));

    //设置整行选中
    SendMessage(hListImportTableInfo, LVM_SETEXTENDEDLISTVIEWSTYLE,
                LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //第一列
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = TEXT("Thunk RVA");  //列标题
    lv.cx = 100;                     //列宽
    lv.iSubItem = 0;
    // ListView_InsertColumn(hListProcess, 0, &lv);
    SendMessage(hListImportTableInfo, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

    //第二列
    lv.pszText = TEXT("Thunk Offset");
    lv.cx = 100;
    lv.iSubItem = 1;
    // ListView_InsertColumn(hListProcess, 1, &lv);
    SendMessage(hListImportTableInfo, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
    //第三列
    lv.pszText = TEXT("Thunk Value");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListImportTableInfo, 2, &lv);
    //第四列
    lv.pszText = TEXT("Hint");
    lv.cx = 80;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListImportTableInfo, 3, &lv);
    //第五列
    lv.pszText = TEXT("API Name");
    lv.cx = 300;
    lv.iSubItem = 4;
    ListView_InsertColumn(hListImportTableInfo, 4, &lv);
}

static VOID init_headers_import_table(HWND hwndDlg, HWND hListImportTableInfo)
{
    LV_COLUMN lv;

    //初始化
    memset(&lv, 0, sizeof(LV_COLUMN));

    //设置整行选中
    SendMessage(hListImportTableInfo, LVM_SETEXTENDEDLISTVIEWSTYLE,
                LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //第一列
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = TEXT("DLL Name");  //列标题
    lv.cx = 120;                    //列宽
    lv.iSubItem = 0;
    // ListView_InsertColumn(hListProcess, 0, &lv);
    SendMessage(hListImportTableInfo, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

    //第二列
    lv.pszText = TEXT("Original First Thunk");
    lv.cx = 100;
    lv.iSubItem = 1;
    // ListView_InsertColumn(hListProcess, 1, &lv);
    SendMessage(hListImportTableInfo, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
    //第三列
    lv.pszText = TEXT("Time/Date Stamp");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListImportTableInfo, 2, &lv);
    //第四列
    lv.pszText = TEXT("Forwarder Chain");
    lv.cx = 100;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListImportTableInfo, 3, &lv);
    //第五列
    lv.pszText = TEXT("Name(RVA)");
    lv.cx = 100;
    lv.iSubItem = 4;
    ListView_InsertColumn(hListImportTableInfo, 4, &lv);
    //第五列
    lv.pszText = TEXT("First Thunk");
    lv.cx = 100;
    lv.iSubItem = 5;
    ListView_InsertColumn(hListImportTableInfo, 5, &lv);
}
