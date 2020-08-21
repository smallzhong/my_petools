#include "StdAfx.h"
#include "head.h"

extern LPVOID pFileBuffer;
extern PIMAGE_DOS_HEADER pDosHeader;
extern PIMAGE_NT_HEADERS32 pNTHeader;
extern PIMAGE_SECTION_HEADER pSectionHeader;

extern DWORD file_size;

extern HINSTANCE g_hInstance;
extern TCHAR g_strFilename[MAX_PATH];

static char string_buffer[0x50];  // �����洢sprintf����Ļ�����
extern PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor;
extern PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor_origin;

static VOID init_headers_import_table(
    HWND hwndDlg, HWND hListImportTableInfo);  // �����б�ͷ��Ϣ
static VOID init_headers_thunk_data(
    HWND hwndDlg, HWND hListImportTableInfo);  // �����б�ͷ��Ϣ
static VOID set_import_table_items(
    HWND hwndDlg, HWND hListImportTableInfo);  // �����б�����Ϣ
static VOID set_thunk_data_info(HWND hListImportTableInfo, HWND hThunkDataInfo,
                                WPARAM wParam, LPARAM lParam, HWND hwndDlg);  // �����б�����Ϣ

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

            //��ȡIDC_LIST_SECTION_VIEWER���
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
                // ��������

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
        return;  // ���û��ѡ���κ�һ��ֱ�ӷ��ء���ֹ[-1]���´���

	// ListView_DeleteAllItems(hListThunkDataInfo); // �������У�Ҫ��ʱ��ȡ����
	ListView_DeleteAllItems(GetDlgItem(hwndDlg, IDC_LIST_THUNK_DATA)); // �Ƚ�֮ǰ�б����������ȫ��ɾ��

    PDWORD pThunkData_INT = NULL;
    pThunkData_INT = (PDWORD)(
        (PBYTE)pFileBuffer +
        RVA_TO_FOA(pFileBuffer, pDosHeader, pNTHeader, pSectionHeader,
                   pImportDescriptor_origin[selected_line].OriginalFirstThunk));
    PIMAGE_IMPORT_BY_NAME pImportByName = NULL;

    LV_ITEM vitem;  //��ʼ��
    memset(&vitem, 0, sizeof(LV_ITEM));
    vitem.mask = LVIF_TEXT;

    int ct = 0;  // ������¼��ǰ�ǵڼ��У�ct + 1��

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
            sprintf(string_buffer, "������ţ�%04xh %dd", (*pThunkData_INT) & 0x7fffffff, (*pThunkData_INT) & 0x7fffffff);
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

    //��ʼ��
    memset(&vitem, 0, sizeof(LV_ITEM));
    vitem.mask = LVIF_TEXT;

    int ct = 0;  // iItem��ֵ
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

        ct++;                 // iItem����ƶ�
        pImportDescriptor++;  // ��pImportDescriptor����ƶ�
    }
}

static VOID init_headers_thunk_data(
    HWND hwndDlg, HWND hListImportTableInfo)  // �����б�ͷ��Ϣ
{
    LV_COLUMN lv;

    //��ʼ��
    memset(&lv, 0, sizeof(LV_COLUMN));

    //��������ѡ��
    SendMessage(hListImportTableInfo, LVM_SETEXTENDEDLISTVIEWSTYLE,
                LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��һ��
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = TEXT("Thunk RVA");  //�б���
    lv.cx = 100;                     //�п�
    lv.iSubItem = 0;
    // ListView_InsertColumn(hListProcess, 0, &lv);
    SendMessage(hListImportTableInfo, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

    //�ڶ���
    lv.pszText = TEXT("Thunk Offset");
    lv.cx = 100;
    lv.iSubItem = 1;
    // ListView_InsertColumn(hListProcess, 1, &lv);
    SendMessage(hListImportTableInfo, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
    //������
    lv.pszText = TEXT("Thunk Value");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListImportTableInfo, 2, &lv);
    //������
    lv.pszText = TEXT("Hint");
    lv.cx = 80;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListImportTableInfo, 3, &lv);
    //������
    lv.pszText = TEXT("API Name");
    lv.cx = 300;
    lv.iSubItem = 4;
    ListView_InsertColumn(hListImportTableInfo, 4, &lv);
}

static VOID init_headers_import_table(HWND hwndDlg, HWND hListImportTableInfo)
{
    LV_COLUMN lv;

    //��ʼ��
    memset(&lv, 0, sizeof(LV_COLUMN));

    //��������ѡ��
    SendMessage(hListImportTableInfo, LVM_SETEXTENDEDLISTVIEWSTYLE,
                LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��һ��
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = TEXT("DLL Name");  //�б���
    lv.cx = 120;                    //�п�
    lv.iSubItem = 0;
    // ListView_InsertColumn(hListProcess, 0, &lv);
    SendMessage(hListImportTableInfo, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

    //�ڶ���
    lv.pszText = TEXT("Original First Thunk");
    lv.cx = 100;
    lv.iSubItem = 1;
    // ListView_InsertColumn(hListProcess, 1, &lv);
    SendMessage(hListImportTableInfo, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
    //������
    lv.pszText = TEXT("Time/Date Stamp");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListImportTableInfo, 2, &lv);
    //������
    lv.pszText = TEXT("Forwarder Chain");
    lv.cx = 100;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListImportTableInfo, 3, &lv);
    //������
    lv.pszText = TEXT("Name(RVA)");
    lv.cx = 100;
    lv.iSubItem = 4;
    ListView_InsertColumn(hListImportTableInfo, 4, &lv);
    //������
    lv.pszText = TEXT("First Thunk");
    lv.cx = 100;
    lv.iSubItem = 5;
    ListView_InsertColumn(hListImportTableInfo, 5, &lv);
}
