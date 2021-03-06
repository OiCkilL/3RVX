#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <malloc.h>
#include <memory.h>
#include <prsht.h>
#include <stdlib.h>
#include <tchar.h>

#include "../3RVX/3RVX.h"
#include "resource.h"

/* Forward Declarations */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int CALLBACK PropSheetProc(HWND hwndDlg, UINT uMsg, LPARAM lParam);

DLGPROC GeneralTabProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DLGPROC DisplayTabProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DLGPROC HotkeyTabProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DLGPROC AboutTabProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);