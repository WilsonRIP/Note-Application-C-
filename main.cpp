#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <string>

// Window procedure declaration
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    // Register window class
    const wchar_t CLASS_NAME[] = L"HelloWindowClass";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc)) {
        MessageBoxW(nullptr, L"Window registration failed", L"Error", MB_ICONERROR);
        return 0;
    }

    // Create the window
    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Hello World",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd) {
        MessageBoxW(nullptr, L"Window creation failed", L"Error", MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Main message loop
    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

// Window procedure implementation
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Draw "Hello, World!" text
            RECT rect;
            GetClientRect(hwnd, &rect);
            DrawTextW(hdc, L"Hello, World!", -1, &rect, 
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
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