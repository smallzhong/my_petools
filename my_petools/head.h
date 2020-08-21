#ifndef HEAD_H_
#define HEAD_H_

#include <Commdlg.h>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
// #include <atlconv.h>

//选择文件夹对话框
#include <Shlobj.h>

#pragma comment(lib, "Shell32.lib")
#include <commctrl.h>				
#pragma comment(lib,"comctl32.lib")	
#include "resource.h"
#include "PEReader.h"			
#include "Utlis.h"

#define EXIT_ERROR(x)                                                          \
    do                                                                         \
    {                                                                          \
		DbgPrintf("error in file %s, line %d\n", __FILE__, __LINE__);							   \
        DbgPrintf("%s\n", x);												   \
		MessageBox(0, TEXT("ASSERTION failed!"), TEXT("ASSERTION failed!"), 0);\
        exit(EXIT_FAILURE);                                                    \
    } while (0)

#define MY_ASSERT(x)                         \
    do                                       \
    {                                        \
        if (!x)                              \
            EXIT_ERROR("ASSERTION failed!"); \
    } while (0)


BOOL CALLBACK MainDlgProc(HWND hwndDlg,   // handle to dialog box
                          UINT uMsg,      // message
                          WPARAM wParam,  // first message parameter
                          LPARAM lParam   // second message parameter
);

BOOL CALLBACK PEViewerDlgProc(HWND hwndDlg,   // handle to dialog box
                              UINT uMsg,      // message
                              WPARAM wParam,  // first message parameter
                              LPARAM lParam   // second message parameter
);

BOOL CALLBACK SectionViewerDlgProc(HWND hwndDlg,   // handle to dialog box
						  UINT uMsg,      // message
						  WPARAM wParam,  // first message parameter
						  LPARAM lParam   // second message parameter
);

BOOL CALLBACK DataDirectoryViewerDlgProc(HWND hwndDlg,   // handle to dialog box
								   UINT uMsg,      // message
								   WPARAM wParam,  // first message parameter
								   LPARAM lParam   // second message parameter
);

BOOL CALLBACK ImportTableDlgProc(HWND hwndDlg,   // handle to dialog box
										 UINT uMsg,      // message
										 WPARAM wParam,  // first message parameter
										 LPARAM lParam   // second message parameter
);

#endif // HEAD_H_
