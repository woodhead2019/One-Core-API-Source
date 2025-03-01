#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <winreg.h>
#include <stdbool.h>
#include <stdio.h>

// IDs dos controles
#define IDC_COMBOBOX 101
#define IDC_APPLY_BUTTON 102
#define IDC_DELETE_BUTTON 103
#define IDC_CANCEL_BUTTON 104

// Opções do dropdown
const TCHAR* versions[] = {
    _T("Windows 2000"),
    _T("Windows XP SP3"),
    _T("Windows Server 2003 SP2"),
    _T("Windows Vista SP2"),
    _T("Windows 7 SP1"),
    _T("Windows 8.1"),
    _T("Windows 10 1511"),
    _T("Windows 10 1607"),
    _T("Windows 10 1809"),
    _T("Windows 10 22H2"),
    _T("Windows 11 24H2")
};


// Valores associados às opções
const TCHAR* versionValues[] = {
    _T("5.0.2195"),
    _T("5.1.2600"),
    _T("5.2.3790"),
    _T("6.0.6002"),
    _T("6.1.7601"),
    _T("6.3.9600"),
    _T("10.0.10586"),
    _T("10.0.14393"),
    _T("10.0.17763"),
    _T("10.0.19045"),
    _T("10.0.22600")
};

// Prototipo das funções
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void WriteToRegistry(const TCHAR*, bool);
void DeleteRegistryKey(bool);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    LPTSTR cmdLine = GetCommandLine(); // Gets the full command line

    // Skip the program's own name
    if (cmdLine[0] == _T('"'))
    {
        // If the program name is quoted, skip until the closing quote
        cmdLine = _tcschr(cmdLine + 1, _T('"'));
        if (cmdLine)
            cmdLine++;
    }
    else
    {
        // Otherwise, skip until the first space
        cmdLine = _tcschr(cmdLine, _T(' '));
    }

    // Skip any spaces after the program name
    if (cmdLine)
        while (*cmdLine == _T(' '))
            cmdLine++;

    // Check if there's anything left in the command line
    if (cmdLine && *cmdLine)
    {
        if (_tcsicmp(cmdLine, _T("/win2000")) == 0)
        {
            WriteToRegistry(versionValues[0], false); // Windows 2000
            return 0;
        }
        else if (_tcsicmp(cmdLine, _T("/winxp")) == 0)
        {
            WriteToRegistry(versionValues[1], false); // Windows XP SP3
            return 0;
        }
        else if (_tcsicmp(cmdLine, _T("/win2003")) == 0)
        {
            WriteToRegistry(versionValues[2], false); // Windows Server 2003 SP2
            return 0;
        }
        else if (_tcsicmp(cmdLine, _T("/winvista")) == 0)
        {
            WriteToRegistry(versionValues[3], false); // Windows Vista SP2
            return 0;
        }
        else if (_tcsicmp(cmdLine, _T("/win7")) == 0)
        {
            WriteToRegistry(versionValues[4], false); // Windows 7 SP1
            return 0;
        }
        else if (_tcsicmp(cmdLine, _T("/win8")) == 0)
        {
            WriteToRegistry(versionValues[5], false); // Windows 8.1
            return 0;
        }
        else if (_tcsicmp(cmdLine, _T("/win10")) == 0)
        {
            WriteToRegistry(versionValues[9], false); // Windows 10 22H2
            return 0;
        }
        else if (_tcsicmp(cmdLine, _T("/win11")) == 0)
        {
            WriteToRegistry(versionValues[10], false); // Windows 11 24H2
            return 0;
        }
        else if (_tcsicmp(cmdLine, _T("/delete")) == 0)
        {
            DeleteRegistryKey(false);
            return 0;
        }
        else
        {
            MessageBox(
                NULL,
                _T("Usage: wct.exe [options]\n\n"
                   "Options:\n"
                   "/win2000 - Set Windows 2000 compatibility\n"
                   "/winxp - Set Windows XP SP3 compatibility\n"
                   "/win2003 - Set Windows Server 2003 SP2 compatibility\n"
                   "/winvista - Set Windows Vista SP2 compatibility\n"
                   "/win7 - Set Windows 7 SP1 compatibility\n"
                   "/win8 - Set Windows 8.1 compatibility\n"
                   "/win10 - Set Windows 10 22H2 compatibility\n"
                   "/win11 - Set Windows 11 24H2 compatibility\n"
                   "/delete - Remove the compatibility"),
                _T("Help"), MB_OK | MB_ICONINFORMATION);
            return 1;
        }
    }

    WNDCLASS wc = {0};
	HWND hwnd;
	MSG msg = {0};
	
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("DropdownApp");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) return -1;

    hwnd = CreateWindowExW(
        WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME,
        _T("DropdownApp"), 
        _T("Windows Compatibility Tool"), 
        WS_VISIBLE | WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        398, 165, 
        NULL, NULL, hInstance, NULL
    );
    
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Function to enable or disable the window and its controls
void HandleWindow(bool enable, HWND hComboBox, HWND hApplyButton, HWND hDeleteButton, HWND hCancelButton, HWND hwnd)
{
    EnableWindow(hwnd, enable);
    EnableWindow(hComboBox, enable);
    EnableWindow(hApplyButton, enable);
    EnableWindow(hDeleteButton, enable);
    EnableWindow(hCancelButton, enable);
    EnableMenuItem(
        GetSystemMenu(hwnd, FALSE), SC_CLOSE,
        enable ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    EnableMenuItem(
        GetSystemMenu(hwnd, FALSE), SC_MINIMIZE,
        enable ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hComboBox, hApplyButton, hDeleteButton, hCancelButton;
    static HFONT hFont;
	int i;

    switch (msg) {
        case WM_CREATE: {
	        // Criar a fonte Arial
	        hFont = CreateFont(
	            20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
	            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	            DEFAULT_PITCH | FF_SWISS, TEXT("Arial")
	        );        	
        	
            // Dropdown List
            hComboBox = CreateWindow(
                WC_COMBOBOXW, NULL,
                CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
                15, 20, 360, 200,
                hwnd, (HMENU)IDC_COMBOBOX,
                GetModuleHandle(NULL), NULL
            );

            // Adiciona as opções ao ComboBox
            for (i = 0; i < sizeof(versions) / sizeof(versions[0]); ++i) {
                SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)versions[i]);
            }
            
            // Definir o índice padrão (por exemplo, 1 - "Item 2")
            SendMessage(hComboBox, CB_SETCURSEL, 0, 0);
            // Botão Aplicar
            hApplyButton = CreateWindow(
                _T("BUTTON"), _T("Apply"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                35, 80, 100, 30,
                hwnd, (HMENU)IDC_APPLY_BUTTON,
                GetModuleHandle(NULL), NULL
            );

            // Botão Deletar
            hDeleteButton = CreateWindow(
                _T("BUTTON"), _T("Delete"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD,
                145, 80, 100, 30,
                hwnd, (HMENU)IDC_DELETE_BUTTON,
                GetModuleHandle(NULL), NULL
            );

            // Botão Cancelar
            hCancelButton = CreateWindow(
                _T("BUTTON"), _T("Cancel"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD,
                255, 80, 100, 30,
                hwnd, (HMENU)IDC_CANCEL_BUTTON,
                GetModuleHandle(NULL), NULL
            );
            
	        // Aplicar a fonte Arial a todos os controles
	        SendMessage(hComboBox, WM_SETFONT, (WPARAM)hFont, TRUE);
	        SendMessage(hApplyButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	        SendMessage(hDeleteButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	        SendMessage(hCancelButton, WM_SETFONT, (WPARAM)hFont, TRUE);            
            break;
        }

        case WM_COMMAND: {
            if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == IDC_COMBOBOX) {
                // Seleção mudou no ComboBox
            }

            switch (LOWORD(wParam)) {
                case IDC_APPLY_BUTTON: {
                    // Obtém a seleção do ComboBox
				    int idx = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
                    // Disable the window
                    HandleWindow(FALSE, hComboBox, hApplyButton, hDeleteButton, hCancelButton, hwnd);
				    if (idx != CB_ERR) {
				        const TCHAR* selectedValue = versionValues[idx];
				        WriteToRegistry(selectedValue, true);
				    } else {
				        MessageBox(hwnd, _T("Please select a version."), _T("Error"), MB_OK | MB_ICONERROR);
				    }
                    // Enable the window
                    HandleWindow(TRUE, hComboBox, hApplyButton, hDeleteButton, hCancelButton, hwnd);
				    break;
                }

                case IDC_DELETE_BUTTON: {
                    SendMessage(hComboBox, CB_SETCURSEL, -1, 0);
                    // Disable the window
                    HandleWindow(FALSE, hComboBox, hApplyButton, hDeleteButton, hCancelButton, hwnd);
                    DeleteRegistryKey(true);
                    //MessageBox(hwnd, _T("Seleção deletada e chave de registro removida!"), _T("Deletar"), MB_OK | MB_ICONINFORMATION);
                    // Enable the window
                    HandleWindow(TRUE, hComboBox, hApplyButton, hDeleteButton, hCancelButton, hwnd);
                    break;
                }

                case IDC_CANCEL_BUTTON: {
                    PostQuitMessage(0);
                    break;
                }
            }
            break;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void WriteToRegistry(const TCHAR* value, bool useMessageBox) {
    HKEY hKey;
    int msgboxID;
    LONG result;
#ifdef _M_AMD64      
    LONG resultWow64;
    HKEY Wow64hKey;
#endif	    
    
    result = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
        0, KEY_SET_VALUE, &hKey
    );
    
#ifdef _M_AMD64     
    resultWow64 = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows NT\\CurrentVersion"),
        0, KEY_SET_VALUE, &Wow64hKey
    );  
#endif	  

    if (result == ERROR_SUCCESS) {
        RegSetValueEx(hKey, _T("EmulatedVersion"), 0, REG_SZ, (const BYTE*)value, (_tcslen(value) + 1) * sizeof(TCHAR));
#ifdef _M_AMD64 
        if(resultWow64 == ERROR_SUCCESS){
            RegSetValueEx(Wow64hKey, _T("EmulatedVersion"), 0, REG_SZ, (const BYTE*)value, (_tcslen(value) + 1) * sizeof(TCHAR));	
		}
#endif       
        RegCloseKey(hKey);
        if (useMessageBox)
        {
            msgboxID = MessageBox(NULL, _T("Value saved with success on registry!"), _T("Success"), MB_OK | MB_ICONINFORMATION);

            if (msgboxID == IDOK)
            {
                PostQuitMessage(0);
            }
        }
        else
        {
            PostQuitMessage(0);
        }
    } else {
        if (useMessageBox)
        {
            MessageBox(NULL, _T("Error while trying access the registry key."), _T("Error"), MB_OK | MB_ICONERROR);
        }
        else
        {
            PostQuitMessage(1);
        }
    }
}

void DeleteRegistryKey(bool useMessageBox) {
    HKEY hKey;
	LONG result;
	int msgboxID;
#ifdef _M_AMD64      
    LONG resultWow64;
    HKEY Wow64hKey;
#endif	

    result = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
        0, KEY_SET_VALUE, &hKey
    );
	
#ifdef _M_AMD64     
    resultWow64 = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows NT\\CurrentVersion"),
        0, KEY_SET_VALUE, &Wow64hKey
    );  
#endif	

    if (result == ERROR_SUCCESS) {
        result = RegDeleteValue(hKey, _T("EmulatedVersion"));
#ifdef _M_AMD64 
        if(resultWow64 == ERROR_SUCCESS){
            resultWow64 = RegDeleteValue(Wow64hKey, _T("EmulatedVersion"));	
		}
#endif 		
        RegCloseKey(hKey);
#ifdef _M_AMD64 
        RegCloseKey(Wow64hKey);
#endif		

        if (result == ERROR_SUCCESS) {
            if (useMessageBox)
            {
                msgboxID = MessageBox(NULL, _T("Registry Key removed with success!"), _T("Success"), MB_OK | MB_ICONINFORMATION);
                if (msgboxID == IDOK)
                {
                    PostQuitMessage(0);
                }
            }
            else
            {
                PostQuitMessage(0);
            }		
        } else {
            if (useMessageBox)
            {
                MessageBox(NULL, _T("Error while trying remove the registry key value."), _T("Error"), MB_OK | MB_ICONERROR);
            }
            else
            {
                PostQuitMessage(1);
            }
        }
    } else {
        if (useMessageBox)
        {
            MessageBox(NULL, _T("Error trying open the registry key."), _T("Error"), MB_OK | MB_ICONERROR);
        }
        else
        {
            PostQuitMessage(1);
        }
    }
}

