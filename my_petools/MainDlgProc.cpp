#include "StdAfx.h"
#include "head.h"
#include <MALLOC.H>
#include <windows.h>
#include <LOCALE.h>
#include <TLHELP32.H>
#include "psapi.h"
#pragma comment(lib,"psapi.lib")

#include <vector>

#ifdef _UNICODE
#define _tprintf wprintf
#else
#define _tprintf printf
#endif


extern LPVOID pFileBuffer;
extern PIMAGE_DOS_HEADER pDosHeader;
extern PIMAGE_NT_HEADERS32 pNTHeader;
extern PIMAGE_SECTION_HEADER pSectionHeader;
extern DWORD file_size;
extern HINSTANCE g_hInstance;
extern TCHAR g_strFilename[MAX_PATH];

struct ModuleInfo
{
	TCHAR szExeFile[MAX_PATH];	// ģ���ļ���
	DWORD ImageBase;
	DWORD SizeOfImage;
};

struct ProcessInfo
{
	ModuleInfo MainModuleInfo;	// ��ģ����Ϣ
	DWORD dwPID;				// ����ID
	ModuleInfo *modules;		// ��ģ������
	DWORD dwModules;			// ��ģ������
	
	// 	~ProcessInfo()
	// 	{
	// 		free(modules);
	// 	}
};

std::vector<ProcessInfo> processInfos; // ��Ž�����Ϣ

static VOID init_headers_process(HWND hwndDlg, HWND hListProcessInfo);  // �����б�ͷ��Ϣ
static VOID init_headers_module(HWND hwndDlg, HWND hListProcessInfo);  // �����б�ͷ��Ϣ
static VOID set_process_items(HWND hwndDlg, HWND hListProcessInfo);  // �����б�����Ϣ
static VOID set_module_info(HWND hListProcessInfo, HWND hModuleInfo,
                                WPARAM wParam, LPARAM lParam, HWND hwndDlg);  // �����б�����Ϣ

DWORD TakeProcessSnapshot(std::vector<ProcessInfo> &processInfos);
DWORD EnumModulesHandle(HANDLE hProcess, HMODULE **lpModule);

BOOL CALLBACK MainDlgProc(HWND hwndDlg,   // handle to dialog box
                          UINT uMsg,      // message
                          WPARAM wParam,  // first message parameter
                          LPARAM lParam   // second message parameter
)
{
	HWND hListProcessInfo = 0;
    HWND hListModuleInfo = 0;
	NMHDR* pnmhdr = NULL;
    switch (uMsg)
    {
        case WM_INITDIALOG:

			//��ȡIDC_LIST_SECTION_VIEWER���
            hListProcessInfo = GetDlgItem(hwndDlg, IDC_LIST_PROCESS);
            MY_ASSERT(hListProcessInfo);
			
            hListModuleInfo = GetDlgItem(hwndDlg, IDC_LIST_MODULE);
            MY_ASSERT(hListModuleInfo);
            init_headers_process(hwndDlg, hListProcessInfo);
            init_headers_module(hwndDlg, hListModuleInfo);
			
			set_process_items(hwndDlg, hListProcessInfo);
			
            return TRUE;

		case WM_NOTIFY:
            pnmhdr = (NMHDR*)lParam;
            if (wParam == IDC_LIST_PROCESS && pnmhdr->code == NM_CLICK)
			{
				set_module_info(GetDlgItem(hwndDlg, IDC_LIST_PROCESS),
					GetDlgItem(hwndDlg, IDC_LIST_MODULE),
                                    wParam, lParam, hwndDlg);
			}
			break;
        case WM_CLOSE:
            EndDialog(hwndDlg, 0);
            exit(0);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_BUTTON_EXIT:
                    EndDialog(hwndDlg, 0);
                    break;
                case IDC_BUTTON_PE_VIEWER:

                    OPENFILENAME ofn = {0};
                    // TCHAR g_strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���

                    ofn.lStructSize = sizeof(OPENFILENAME);  //�ṹ���С
                    ofn.hwndOwner =
                        NULL;  //ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
                    ofn.lpstrFilter = TEXT("�����ļ�\0*.*\0C/C++ "
                                           "Flie\0*.cpp;*.c;*.h\0\0");  //���ù���
                    ofn.nFilterIndex = 1;  //����������
                    ofn.lpstrFile =
                        g_strFilename;  //���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
                    ofn.nMaxFile = sizeof(g_strFilename);  //����������
                    ofn.lpstrInitialDir = NULL;  //��ʼĿ¼ΪĬ��
                    ofn.lpstrTitle =
                        TEXT("��ѡ��һ���ļ�");  //ʹ��ϵͳĬ�ϱ������ռ���
                    ofn.Flags =
                        OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
                        OFN_HIDEREADONLY;  //�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
                    if (GetOpenFileName(&ofn))
                    {
                        // MessageBox(0, g_strFilename, 0, 0);
                        DialogBox(g_hInstance,
                                  MAKEINTRESOURCE(IDD_DIALOG_PE_VIEWER), NULL,
                                  PEViewerDlgProc);
                    }
                    else
                        break;
            }

            break;
    }
	
    return FALSE;
}


static VOID set_module_info(HWND hListProcessInfo, HWND hModuleInfo,
                                WPARAM wParam, LPARAM lParam, HWND hwndDlg)
{
    DWORD selected_line = 0;
    selected_line =
        SendMessage(hListProcessInfo, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    if (selected_line == -1)
        return;  // ���û��ѡ���κ�һ��ֱ�ӷ��ء���ֹ[-1]���´���
	DbgPrintf("selected line: %d", selected_line);
	// ListView_DeleteAllItems(hListThunkDataInfo); // �������У�Ҫ��ʱ��ȡ����
	ListView_DeleteAllItems(GetDlgItem(hwndDlg, IDC_LIST_MODULE)); // �Ƚ�֮ǰ�б����������ȫ��ɾ��


    LV_ITEM vitem;  //��ʼ��
    memset(&vitem, 0, sizeof(LV_ITEM));
    vitem.mask = LVIF_TEXT;

	char string_buffer[0x200];

	for (int j = 0; j < processInfos[selected_line].dwModules; j++)
	{	
		int t_len = 0; // ��¼�ַ�������
		t_len = strlen(processInfos[selected_line].modules[j].szExeFile);
		int t_start = 0; // ��¼���һ��'/'�ַ���λ��
		for (int i = t_len - 1; i > 0; i -- )
		{
			if (processInfos[selected_line].modules[j].szExeFile[i] == '\\')
			{
				t_start = i;
				break;
			}
		}
		MY_ASSERT(t_len);
		MY_ASSERT(t_start);
		DbgPrintf("t_len = %d", t_len);

		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(
			string_buffer, "%s", processInfos[selected_line].modules[j].szExeFile + t_start + 1); // + t_start + 1 �ض�ǰ���·��
		/*sprintf(
			string_buffer, "%s", processInfos[selected_line].modules[j].szExeFile);*/
		vitem.pszText = string_buffer;
		vitem.iItem = j;
		vitem.iSubItem = 0;
		// ListView_InsertItem(hListProcess, &vitem);
		SendMessage(hModuleInfo, LVM_INSERTITEM, 0, (DWORD)&vitem);

		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%x", processInfos[selected_line].modules[j].ImageBase);
		vitem.pszText = string_buffer;
		vitem.iItem = j;
		vitem.iSubItem = 1;
		ListView_SetItem(hModuleInfo, &vitem);

		memset(string_buffer, 0, sizeof(string_buffer));
		sprintf(string_buffer, "%x", processInfos[selected_line].modules[j].SizeOfImage);
		vitem.pszText = string_buffer;
		vitem.iItem = j;
		vitem.iSubItem = 2;
		ListView_SetItem(hModuleInfo, &vitem);
		
	}
}

static VOID set_process_items(HWND hwndDlg, HWND hListProcessInfo)
{
    LV_ITEM vitem;
    //��ʼ��
    memset(&vitem, 0, sizeof(LV_ITEM));
    vitem.mask = LVIF_TEXT;

	char string_buffer[0x200];

	setlocale(LC_ALL, "");
	
	TakeProcessSnapshot(processInfos);

	for (int i = 0; i < processInfos.size(); i++)
	{
		memset(string_buffer, 0, 0x200);
		sprintf(string_buffer, "%s", processInfos[i].MainModuleInfo.szExeFile);
		vitem.pszText = string_buffer;
		vitem.iItem = i;
		vitem.iSubItem = 0;
		SendMessage(hListProcessInfo, LVM_INSERTITEM, 0, (DWORD)&vitem);

		memset(string_buffer, 0, 0x200);
		sprintf(string_buffer, "%x", processInfos[i].dwPID);
		vitem.pszText = string_buffer;
		vitem.iItem = i;
		vitem.iSubItem = 1;		
		ListView_SetItem(hListProcessInfo, &vitem);		
		
		memset(string_buffer, 0, 0x200);
		sprintf(string_buffer, "%x", processInfos[i].MainModuleInfo.ImageBase);
		vitem.pszText = string_buffer;
		vitem.iItem = i;
		vitem.iSubItem = 2;		
		ListView_SetItem(hListProcessInfo, &vitem);

		memset(string_buffer, 0, 0x200);
		if (processInfos[i].dwModules)
			sprintf(string_buffer, "%x", processInfos[i].modules[0].ImageBase);
		else 
			sprintf(string_buffer, "-");
		vitem.pszText = string_buffer;
		vitem.iItem = i;
		vitem.iSubItem = 2;		
		ListView_SetItem(hListProcessInfo, &vitem);

		memset(string_buffer, 0, 0x200);
		if (processInfos[i].dwModules)
			sprintf(string_buffer, "%x", processInfos[i].modules[0].SizeOfImage);
		else 
			sprintf(string_buffer, "-");
		vitem.pszText = string_buffer;
		vitem.iItem = i;
		vitem.iSubItem = 3;		
		ListView_SetItem(hListProcessInfo, &vitem);
		
		/*
		for (int j = 0; j < processInfos[i].dwModules; j++)
		{
			_tprintf(TEXT("\t\t%x\t%x\t%s\n"), processInfos[i].modules[j].ImageBase, processInfos[i].modules[j].SizeOfImage, processInfos[i].modules[j].szExeFile);
		}*/
	}

}

static VOID init_headers_module(
    HWND hwndDlg, HWND hListProcessInfo)  // �����б�ͷ��Ϣ
{
    LV_COLUMN lv;

    //��ʼ��
    memset(&lv, 0, sizeof(LV_COLUMN));

    //��������ѡ��
    SendMessage(hListProcessInfo, LVM_SETEXTENDEDLISTVIEWSTYLE,
                LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��һ��
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = TEXT("ģ������");  //�б���
    lv.cx = 270;                     //�п�
    lv.iSubItem = 0;
    // ListView_InsertColumn(hListProcess, 0, &lv);
    SendMessage(hListProcessInfo, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

    //�ڶ���
	lv.pszText = TEXT("�����С");
    lv.cx = 105;
    lv.iSubItem = 1;
    // ListView_InsertColumn(hListProcess, 1, &lv);
    SendMessage(hListProcessInfo, LVM_INSERTCOLUMN, 1, (DWORD)&lv);

	//������
    lv.pszText = TEXT("�����ַ");
    lv.cx = 110;
    lv.iSubItem = 1;
    // ListView_InsertColumn(hListProcess, 1, &lv);
    SendMessage(hListProcessInfo, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
}

static VOID init_headers_process(HWND hwndDlg, HWND hListProcessInfo)
{
    LV_COLUMN lv;

    //��ʼ��
    memset(&lv, 0, sizeof(LV_COLUMN));

    //��������ѡ��
    SendMessage(hListProcessInfo, LVM_SETEXTENDEDLISTVIEWSTYLE,
                LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��һ��
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lv.pszText = TEXT("����");  //�б���
    lv.cx = 200;                    //�п�
    lv.iSubItem = 0;
    // ListView_InsertColumn(hListProcess, 0, &lv);
    SendMessage(hListProcessInfo, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

    //�ڶ���
    lv.pszText = TEXT("PID");
    lv.cx = 70;
    lv.iSubItem = 1;
    // ListView_InsertColumn(hListProcess, 1, &lv);
	ListView_InsertColumn(hListProcessInfo, 1, &lv);

    //������
    lv.pszText = TEXT("�����ַ");
    lv.cx = 105;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListProcessInfo, 2, &lv);
    //������
    lv.pszText = TEXT("�����С");
    lv.cx = 110;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListProcessInfo, 3, &lv);
}

// ö�ٽ��̵�ַ�ռ��ڵ�ģ�������������鳤��
DWORD EnumModulesHandle(HANDLE hProcess, HMODULE **lpModule)
{
	DWORD cbBytesNeeded = 0;
	// ��ע��EnumProcessModules �����޷�ö��64λ���̵�ģ�飬���ǳ�����64λ����
	EnumProcessModules(hProcess, NULL, 0, &cbBytesNeeded); // ���������С
	*lpModule = (HMODULE *)malloc(cbBytesNeeded + 0x1000);
	EnumProcessModules(hProcess, *lpModule, cbBytesNeeded + 0x1000, &cbBytesNeeded); // ö��ģ����
	return cbBytesNeeded / sizeof(HMODULE);
}

// ��ȡ��ǰ���н��̵���Ϣ
DWORD TakeProcessSnapshot(std::vector<ProcessInfo> &processInfos)
{
	processInfos.clear();
	// ��ȡ���̿��գ��õ���ǰ���н��̵�PID
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnapshot == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, TEXT("��ȡ���̿���ʧ��"), TEXT("Error"), MB_OK);
		return -1;
	}	
	// ��������
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
