// Copyright 2021 Liwei Zhen
// The logic are forked from Marin Stone's TouchCursor project. So the probject is still GPL'ed. See the original copyright claim below.

// Copyright (C) 2010 Martin Stone.
// 
// This file is part of TouchCursor.
// 
// TouchCursor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// TouchCursor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with TouchCursor.  If not, see <http://www.gnu.org/licenses/>.


#include "framework.h"
#include "sfn.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

namespace {

    const int taskbarNotificationMsg = WM_USER;

    DWORD showMenu(HWND hwnd) {
        POINT pt;
        GetCursorPos(&pt);
        HMENU hmenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1));
        // TrackPopupMenu cannot display the menu bar so get 
        // a handle to the first shortcut menu. 
        HMENU hmenuTrackPopup = GetSubMenu(hmenu, 0);

        // Display the shortcut menu. 
        DWORD command = TrackPopupMenu(hmenuTrackPopup,
            TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
            pt.x, pt.y, 0, hwnd, NULL);

        DestroyMenu(hmenu);
        return command;
    }

    void runConfigure() {
        //LaunchProgRelative(L"tcconfig.exe");
    }

    void showHelp() {
        //LaunchLocalHtml(L"docs\\help.html");
    }

    void refreshIconState(HWND hwnd, bool justDelete = false) {
        // delete, maybe re-add after

        NOTIFYICONDATA nid;
        nid.cbSize = sizeof(nid);
        nid.hWnd = hwnd;
        nid.uID = 0;

        Shell_NotifyIcon(NIM_DELETE, &nid);

        if (!justDelete) {
            // add the item & set version
            nid.uFlags = NIF_MESSAGE;
            nid.uCallbackMessage = taskbarNotificationMsg;
            nid.uVersion = NOTIFYICON_VERSION;
            Shell_NotifyIcon(NIM_ADD, &nid);
            Shell_NotifyIcon(NIM_SETVERSION, &nid);

            // set the icon
            nid.uFlags = NIF_ICON | NIF_TIP;
            nid.hIcon = LoadIcon(hInst,
                MAKEINTRESOURCE(IDI_SMALL));
            lstrcpy(nid.szTip, L"sfn");
            //if (!IsEnabled()) lstrcat(nid.szTip, L" (Disabled)");
            Shell_NotifyIcon(NIM_MODIFY, &nid);
        }
    }

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SFN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // only allow one instance to run:
    CreateMutex(0, 0, L"sfn_process");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return 0;
    }

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SFN));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SFN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SFN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_HIDE);
   //ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CREATE:
        //sfn business initialization
        InstallHook();
        refreshIconState(hWnd);
        //SetTimer(w, 1, 500, 0);
        break;
    case WM_DESTROY:
        RemoveHook();
        refreshIconState(hWnd, true);
        PostQuitMessage(0);
        break;
    case taskbarNotificationMsg:
        // taskbar event
        switch (lParam) {
        case WM_CONTEXTMENU:
        case WM_RBUTTONDOWN:
        {
            SetForegroundWindow(hWnd);
            DWORD command = showMenu(hWnd);
            switch (command) {
            case ID_NOTIFYAREA_EXIT:
                PostQuitMessage(0);
                break;
            case ID_NOTIFYAREA_CONFIGURE:
                runConfigure();
                break;
            case ID_NOTIFYAREA_HELP:
                showHelp();
                break;
            default: break;
            }
            NOTIFYICONDATA nid = { sizeof(nid), hWnd, 0 };
            Shell_NotifyIcon(NIM_SETFOCUS, &nid);
            break;
        }

        case WM_LBUTTONDBLCLK:
        case NIN_KEYSELECT:
            runConfigure();
            break;

        default:
            return FALSE;
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
