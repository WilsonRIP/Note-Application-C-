#ifndef UNICODE
#define UNICODE
#endif

// Define menu identifiers
#define IDM_OPEN 1
#define IDM_SAVE 2
#define IDM_EXIT 3

#include <windows.h>
#include <string>
#include <fstream>
#include <vector>

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Global variables
HWND hEdit; // Handle to the edit control
std::wstring filePath; // Path to the currently open file

// Function to read text from a file
std::wstring ReadTextFromFile(const std::wstring& filePath) {
    std::wifstream file(filePath);
    if (file.is_open()) {
        std::wstring content((std::istreambuf_iterator<wchar_t>(file)), std::istreambuf_iterator<wchar_t>());
        file.close();
        return content;
    }
    return L""; // Return empty string if file cannot be opened
}

// Function to write text to a file
bool WriteTextToFile(const std::wstring& filePath, const std::wstring& text) {
    std::wofstream file(filePath);
    if (file.is_open()) {
        file << text;
        file.close();
        return true;
    }
    return false; // Return false if file cannot be opened
}

// Function to open a file
void OpenFile(HWND hwnd) {
    OPENFILENAMEW ofn = {};
    wchar_t szFile[260] = {};
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        filePath = ofn.lpstrFile;
        std::wstring fileContent = ReadTextFromFile(filePath);
        SetWindowTextW(hEdit, fileContent.c_str());
    }
}

// Function to save a file
void SaveFile(HWND hwnd) {
    if (filePath.empty()) {
        // If no file is currently open, prompt for a new file name
        OPENFILENAMEW ofn = {};
        wchar_t szFile[260] = {};
        ofn.lStructSize = sizeof(OPENFILENAMEW);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
        ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

        if (GetSaveFileNameW(&ofn)) {
            filePath = ofn.lpstrFile;
        } else {
            return; // If the user cancels the save operation, exit the function
        }
    }

    // Get the text from the edit control
    int textLength = GetWindowTextLengthW(hEdit);
    std::vector<wchar_t> buffer(textLength + 1);
    GetWindowTextW(hEdit, buffer.data(), textLength + 1);
    std::wstring text(buffer.data());

    // Write the text to the file
    if (WriteTextToFile(filePath, text)) {
        MessageBoxW(hwnd, L"File saved successfully!", L"Save File", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBoxW(hwnd, L"Failed to save file!", L"Save File", MB_OK | MB_ICONERROR);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    // Define the window class structure
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASSW wc = {}; // Zero-initialize the structure

    wc.lpfnWndProc = WindowProc; // Pointer to the window procedure
    wc.hInstance = hInstance;     // Handle to the instance that contains the window procedure
    wc.lpszClassName = CLASS_NAME; // Class name

    // Register the window class
    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the window
    HWND hwnd = CreateWindowExW(
        0,                          // Optional window styles
        CLASS_NAME,                 // Window class name
        L"Notepad--",    // Window title
        WS_OVERLAPPEDWINDOW,        // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

        NULL,       // Parent window (if any)
        NULL,       // Menu handle (if any)
        hInstance,  // Instance handle
        NULL        // Pointer to window-creation data
    );

    if (hwnd == NULL) {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the edit control
    hEdit = CreateWindowExW(
        0,                          // Optional window styles
        L"EDIT",                     // Edit control class name
        NULL,                       // No initial text
        WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
        0, 0, 800, 600,            // Size and position
        hwnd,                       // Parent window
        NULL,                       // Control ID
        hInstance,                  // Instance handle
        NULL                        // Pointer to window-creation data
    );

    if (hEdit == NULL) {
        MessageBoxW(NULL, L"Edit control creation failed.", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

   // Create the menu
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();

    AppendMenuW(hFileMenu, MF_STRING, IDM_OPEN, L"&Open");
    AppendMenuW(hFileMenu, MF_STRING, IDM_SAVE, L"&Save");
    AppendMenuW(hFileMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenuW(hFileMenu, MF_STRING, IDM_EXIT, L"&Exit");

    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"&File");
    SetMenu(hwnd, hMenu);

    ShowWindow(hwnd, nCmdShow);
    ShowWindow(hEdit, nCmdShow);

    // Run the message loop
    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Translate virtual-key messages into character messages
        DispatchMessageW(&msg);  // Dispatch the message to the window procedure
    }

    return 0;
}

// Window procedure to handle messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case IDM_OPEN:
                    OpenFile(hwnd);
                    return 0;
                case IDM_SAVE:
                    SaveFile(hwnd);
                    return 0;
                case IDM_EXIT:
                    DestroyWindow(hwnd);
                    return 0;
            }
            break;
        }
        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            MoveWindow(hEdit, 0, 0, width, height, TRUE);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
} 