//»ñÈ¡IDC_LIST_SECTION_VIEWER¾ä±ú
hListImportTableInfo = GetDlgItem(hwndDlg, IDC_LIST_IMPORT_TABLE);
MY_ASSERT(hListImportTableInfo);

hListThunkDataInfo = GetDlgItem(hwndDlg, IDC_LIST_THUNK_DATA);
MY_ASSERT(hListThunkDataInfo);
init_headers_import_table(hwndDlg, hListImportTableInfo);
init_headers_thunk_data(hwndDlg, hListThunkDataInfo);

set_import_table_items(hwndDlg, hListImportTableInfo);

            return TRUE;