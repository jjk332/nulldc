//////////////////////////////////////////////////////////////////////////////////////////
// Project description
// �������������������
// Name: nullDC 
// Description: A nullDC Compatible Input Plugin
//
// Author: Falcon4ever (nullDC@falcon4ever.com)
// Site: www.multigesture.net
// Copyright (C) 2007-2009 nullDC Project.
//


#include <windows.h>    // includes basic windows functionality
#include <Windowsx.h>
#include <stdio.h>
#include <commctrl.h>   // includes the common control header
#include "resource.h"

#pragma comment(lib, "comctl32.lib")

//////////////////////////////////////////////////////////////////////////////////////////
// Config dialog functions
// �����������������������

INT_PTR CALLBACK OpenConfig( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

bool GetButtons(HWND hDlg, int buttonid, int controller);
bool GetHats(HWND hDlg, int buttonid, int controller);
bool GetAxis(HWND hDlg, int buttonid, int controller);

void UpdateVisibleItems(HWND hDlg, int controllertype);

void GetControllerAll(HWND hDlg, int controller);
void SetControllerAll(HWND hDlg, int controller);

int GetButton(HWND hDlg, int item);
void SetButton(HWND hDlg, int item, int value);

void OpenAbout(HINSTANCE abouthInstance, HWND _hParent);

BOOL CALLBACK AboutDlg(HWND abouthWnd, UINT message, WPARAM wParam, LPARAM lParam);
