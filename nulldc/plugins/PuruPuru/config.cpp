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

#include "PuruPuru.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Variables
// ���������

extern u32 current_port;
extern bool emulator_running;

extern CONTROLLER_INFO	*joyinfo;
extern CONTROLLER_MAPPING joysticks[4];
extern HINSTANCE PuruPuru_hInst;
//HINSTANCE config_hInst;
//HWND config_HWND;

static const wchar* ControllerType[] =
{
	L"Joystick (SDL)",	
	L"Joystick (Xinput)",	// Perhaps just for the triggers...
	L"Keyboard (Win)"		// SDL for keys is meh... 
};

//////////////////////////////////////////////////////////////////////////////////////////
// Config dialog functions
// �����������������������

INT_PTR CALLBACK OpenConfig( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	int Beep = GetSystemMetrics( SPI_SETBEEP );
	
	switch( uMsg )
	{
	case WM_INITDIALOG:

		// Disables the DING sound when using the keyboard...			
		SystemParametersInfo( SPI_SETBEEP , FALSE , NULL , 0);

		TCITEM tci; 
		tci.mask = TCIF_TEXT | TCIF_IMAGE;
		tci.iImage = -1; 
		tci.pszText = current_port == 0 ? L"->Controller 1<-" : L"Controller 1"; 
		TabCtrl_InsertItem(GetDlgItem(hDlg,IDC_PORTTAB), 0, &tci); 
		tci.pszText = current_port == 1 ? L"->Controller 2<-" : L"Controller 2"; 
		TabCtrl_InsertItem(GetDlgItem(hDlg,IDC_PORTTAB), 1, &tci); 
		tci.pszText = current_port == 2 ? L"->Controller 3<-" : L"Controller 3"; 
		TabCtrl_InsertItem(GetDlgItem(hDlg,IDC_PORTTAB), 2, &tci); 
		tci.pszText = current_port == 3 ? L"->Controller 4<-" : L"Controller 4"; 
		TabCtrl_InsertItem(GetDlgItem(hDlg,IDC_PORTTAB), 3, &tci); 

		TabCtrl_SetCurSel(GetDlgItem(hDlg,IDC_PORTTAB),current_port);							

		//if(emulator_running)
		if(emulator_running)
		{
			ComboBox_Enable(GetDlgItem(hDlg, IDC_JOYNAME), FALSE);
			ComboBox_Enable(GetDlgItem(hDlg, IDC_CONTROLTYPE), FALSE);				
		}
		else
		{
			ComboBox_Enable(GetDlgItem(hDlg, IDC_JOYNAME), TRUE);				
			ComboBox_Enable(GetDlgItem(hDlg, IDC_CONTROLTYPE), TRUE);				
		}
			
			
		// Search for devices and add the to the device list
		if( SDL_NumJoysticks() > 0 )
		{
			HWND CB = GetDlgItem(hDlg, IDC_JOYNAME);

			for(int x = 0; x < SDL_NumJoysticks(); x++)
			{					
				SendMessage(CB, CB_ADDSTRING, 0, (LPARAM)joyinfo[x].Name);					
			}
				
			CB = GetDlgItem(hDlg, IDC_CONTROLTYPE);				
			SendMessage(CB, CB_ADDSTRING, 0, (LPARAM)ControllerType[CTL_TYPE_JOYSTICK_SDL]);				
			//SendMessage(CB, CB_ADDSTRING, 0, (LPARAM)ControllerType[CTL_TYPE_JOYSTICK_XINPUT]);
			//SendMessage(CB, CB_ADDSTRING, 0, (LPARAM)ControllerType[CTL_TYPE_KEYBOARD]);
				
			wchar buffer[8];				
			CB = GetDlgItem(hDlg, IDC_DEADZONE);
			SendMessage(CB, CB_RESETCONTENT, 0, 0);
			for(int x = 1; x <= 100; x++)
			{				
				wsprintf(buffer, L"%d %%", x);
				SendMessage(CB, CB_ADDSTRING, 0, (LPARAM)buffer);				
			}

			SetControllerAll(hDlg, current_port);
			return TRUE;
		}
		else
		{
			HWND CB = GetDlgItem(hDlg, IDC_JOYNAME);				
			SendMessage(CB, CB_ADDSTRING, 0, (LPARAM)L"No Joystick detected!");
			SendMessage(CB, CB_SETCURSEL, 0, 0);
			return FALSE;
		}			
	break;

	case WM_NOTIFY:
		{
			if ( ((LPNMHDR)lParam)->idFrom==IDC_PORTTAB && ((LPNMHDR)lParam)->code == TCN_SELCHANGE  )
			{
				current_port = TabCtrl_GetCurSel(GetDlgItem(hDlg,IDC_PORTTAB));
				SetControllerAll(hDlg, current_port);				
			}
			
			return true;
		}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			
			case IDOK:
			{
				GetControllerAll(hDlg, current_port);
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			
			case IDC_JOYNAME:
			{
				// Selected a different joystick
				if(HIWORD(wParam) == CBN_SELCHANGE)
				{
					joysticks[current_port].ID = (int)SendMessage(GetDlgItem(hDlg, IDC_JOYNAME), CB_GETCURSEL, 0, 0);						
				}
				return TRUE;
			}
			break;

			case IDC_CONTROLTYPE:
			{
				// Selected a different joystick
				if(HIWORD(wParam) == CBN_SELCHANGE)
				{
					joysticks[current_port].controllertype = (int)SendMessage(GetDlgItem(hDlg, IDC_CONTROLTYPE), CB_GETCURSEL, 0, 0);					
					UpdateVisibleItems(hDlg, joysticks[current_port].controllertype);
				}
				return TRUE;
			}
			break;
				
			case IDC_SHOULDERL:
			case IDC_SHOULDERR:
			case IDC_A:
			case IDC_B:
			case IDC_X:
			case IDC_Y:				
			case IDC_START:
			case IDC_HALFPRESS:												
			case IDC_DPAD_UP:
			case IDC_DPAD_DOWN:
			case IDC_DPAD_LEFT:
			case IDC_DPAD_RIGHT:
			case IDC_MX_L:
			case IDC_MX_R:
			case IDC_MY_U:
			case IDC_MY_D:
			{
				GetInputSDL(hDlg, LOWORD(wParam), current_port);
				return TRUE;
			}
			break;

			case IDABOUT:
			{
				// yay an about box ;p
				OpenAbout(PuruPuru_hInst, hDlg);
				return TRUE;
			}
			break;
		}			
	break;
					
	case WM_CLOSE:
	case WM_DESTROY:	
		
		// Restore it.
		SystemParametersInfo( SPI_SETBEEP , Beep , NULL , 0);
		
		KillTimer(hDlg,0);
		EndDialog(hDlg,0);
	return true;

	default: break;
	}	
	return FALSE;
}

// Wait for button/hat press
// ���������������������

bool GetInputSDL(HWND hDlg, int buttonid, int controller)
{
	char key[256];
	
	buttonid += 1000;
		
	SDL_Joystick *joy;
	joy=SDL_JoystickOpen(joysticks[controller].ID);

	wchar format[128];

	int buttons = SDL_JoystickNumButtons(joy);
	int hats = SDL_JoystickNumHats(joy);
	int axes = SDL_JoystickNumAxes(joy);
	Sint16 value;

	bool KEY = false;
	bool HAT = false;
	bool AXIS = false;
	bool plus = false;

	bool waiting = true;
	bool succeed = false;
	int pressed = 0;

	int counter1 = 0;
	int counter2 = 10;
	
	wsprintf(format, L"[%d]", counter2);
	SetDlgItemText(hDlg, buttonid, format);
	
	while(waiting)
	{			
		SDL_JoystickUpdate();
		GetKeyState(key);

		// AXIS
		for(int b = 0; b < axes; b++)
		{		
			value = SDL_JoystickGetAxis(joy, b);
			if(value > 10000)
			{
				pressed = b;	
				plus = true;
				waiting = false;
				succeed = true;
				AXIS = true;
				break;
			}
			else if ( value < -10000 )
			{
				pressed = b;	
				waiting = false;
				succeed = true;
				AXIS = true;
				break;
			}
		}
		
		// BUTTONS
		for(int b = 0; b < buttons; b++)
		{			
			if(SDL_JoystickGetButton(joy, b))
			{
				pressed = b;	
				waiting = false;
				succeed = true;
				break;
			}			
		}

		// HATS
		for(int b = 0; b < hats; b++)
		{			
			switch (SDL_JoystickGetHat(joy, b))
			{
				case SDL_HAT_LEFT:
					{
						pressed = SDL_HAT_LEFT;	
						waiting = false;
						succeed = true;
						HAT = true;
						break;
					}
				case SDL_HAT_RIGHT:
					{
						pressed = SDL_HAT_RIGHT;	
						waiting = false;
						succeed = true;
						HAT = true;
						break;
					}
				case SDL_HAT_UP:
					{
						pressed = SDL_HAT_UP;	
						waiting = false;
						succeed = true;
						HAT = true;
						break;
					}
				case SDL_HAT_DOWN:
					{
						pressed = SDL_HAT_DOWN;	
						waiting = false;
						succeed = true;
						HAT = true;
						break;
					}
			}

		}

		for(int k = 0; k < 256; k++)
		{
			if( key[k] )
			{
				pressed = k;
				waiting = false;
				succeed = true;
				KEY = true;
				break;
			}
		}

		counter1++;
		if(counter1==100)
		{
			counter1=0;
			counter2--;
			
			wsprintf(format, L"[%d]", counter2);
			SetDlgItemText(hDlg, buttonid, format);

			if(counter2<0)
				waiting = false;
		}	
		Sleep(10);
	}
			
	if(succeed)
	{
		if(HAT) wsprintf(format, L"H%d", pressed);

		else if (AXIS)
		{
			if(plus)
				wsprintf(format, L"A%d+", pressed);
			else
				wsprintf(format, L"A%d-", pressed);
		}
		
		else if (KEY) wsprintf(format, L"K%d", pressed);
		
		else    wsprintf(format, L"B%d", pressed);

		
	}
	else
		wsprintf(format, L"-1", pressed);
	SetDlgItemText(hDlg, buttonid, format);

	if(SDL_JoystickOpened(joysticks[controller].ID))
		SDL_JoystickClose(joy);

	return true;
}

void UpdateVisibleItems(HWND hDlg, int controllertype)
{	
	if(controllertype == CTL_TYPE_KEYBOARD)	
	{
		ComboBox_Enable(GetDlgItem(hDlg, IDC_JOYNAME), FALSE);		
	}

}

// Set dialog items
// ����������������
void SetControllerAll(HWND hDlg, int controller)
{	
	SendMessage(GetDlgItem(hDlg, IDC_JOYNAME), CB_SETCURSEL, joysticks[controller].ID, 0);

	if( joysticks[controller].enabled )
	{
		ShowWindow(GetDlgItem(hDlg, IDC_CONFIG_ON), TRUE);
		ShowWindow(GetDlgItem(hDlg, IDC_CONFIG_OFF), FALSE);
	}
	else
	{
		ShowWindow(GetDlgItem(hDlg, IDC_CONFIG_ON), FALSE);
		ShowWindow(GetDlgItem(hDlg, IDC_CONFIG_OFF), TRUE);
	}

	SetButton(hDlg, IDTEXT_SHOULDERL,	joysticks[controller].control[MAP_LT]);
	SetButton(hDlg, IDTEXT_SHOULDERR,	joysticks[controller].control[MAP_RT]);
	SetButton(hDlg, IDTEXT_A,			joysticks[controller].control[MAP_A]);
	SetButton(hDlg, IDTEXT_B,			joysticks[controller].control[MAP_B]);
	SetButton(hDlg, IDTEXT_X,			joysticks[controller].control[MAP_X]);
	SetButton(hDlg, IDTEXT_Y,			joysticks[controller].control[MAP_Y]);

	SetButton(hDlg, IDTEXT_START,		joysticks[controller].control[MAP_START]);
	
	SetButton(hDlg, IDTEXT_HALFPRESS,	joysticks[controller].control[MAP_HALF]);
	
	SetButton(hDlg, IDTEXT_MX_L,		joysticks[controller].control[MAP_A_XL]);
	SetButton(hDlg, IDTEXT_MX_R,		joysticks[controller].control[MAP_A_XR]);
	SetButton(hDlg, IDTEXT_MY_U,		joysticks[controller].control[MAP_A_YU]);	
	SetButton(hDlg, IDTEXT_MY_D,		joysticks[controller].control[MAP_A_YD]);

	SendMessage(GetDlgItem(hDlg, IDC_CONTROLTYPE), CB_SETCURSEL, joysticks[controller].controllertype, 0);	
	SendMessage(GetDlgItem(hDlg, IDC_DEADZONE), CB_SETCURSEL, joysticks[controller].deadzone, 0);	

	UpdateVisibleItems(hDlg, joysticks[controller].controllertype);

	SetButton(hDlg, IDTEXT_DPAD_UP,		joysticks[controller].control[MAP_D_UP]);
	SetButton(hDlg, IDTEXT_DPAD_DOWN,	joysticks[controller].control[MAP_D_DOWN]);
	SetButton(hDlg, IDTEXT_DPAD_LEFT,	joysticks[controller].control[MAP_D_LEFT]);
	SetButton(hDlg, IDTEXT_DPAD_RIGHT,	joysticks[controller].control[MAP_D_RIGHT]);		
}

// Get dialog items
// ����������������
void GetControllerAll(HWND hDlg, int controller)
{
	
	joysticks[controller].ID = (int)SendMessage(GetDlgItem(hDlg, IDC_JOYNAME), CB_GETCURSEL, 0, 0); 

	GetButton(hDlg, IDTEXT_SHOULDERL,	joysticks[controller].control[MAP_LT]);
	GetButton(hDlg, IDTEXT_SHOULDERR,	joysticks[controller].control[MAP_RT]);
	GetButton(hDlg, IDTEXT_A,			joysticks[controller].control[MAP_A]);
	GetButton(hDlg, IDTEXT_B,			joysticks[controller].control[MAP_B]);
	GetButton(hDlg, IDTEXT_X,			joysticks[controller].control[MAP_X]);
	GetButton(hDlg, IDTEXT_Y,			joysticks[controller].control[MAP_Y]);

	GetButton(hDlg, IDTEXT_START,		joysticks[controller].control[MAP_START]);
	
	GetButton(hDlg, IDTEXT_HALFPRESS,	joysticks[controller].control[MAP_HALF]);
		
	GetButton(hDlg, IDTEXT_DPAD_UP,		joysticks[controller].control[MAP_D_UP]);
	GetButton(hDlg, IDTEXT_DPAD_DOWN,	joysticks[controller].control[MAP_D_DOWN]);
	GetButton(hDlg, IDTEXT_DPAD_LEFT,	joysticks[controller].control[MAP_D_LEFT]);
	GetButton(hDlg, IDTEXT_DPAD_RIGHT,	joysticks[controller].control[MAP_D_RIGHT]);	

	GetButton(hDlg, IDTEXT_MX_L,			joysticks[controller].control[MAP_A_XL]);
	GetButton(hDlg, IDTEXT_MX_R,			joysticks[controller].control[MAP_A_XR]);
	GetButton(hDlg, IDTEXT_MY_U,			joysticks[controller].control[MAP_A_YU]);
	GetButton(hDlg, IDTEXT_MY_D,			joysticks[controller].control[MAP_A_YD]);
	
	joysticks[controller].controllertype = (int)SendMessage(GetDlgItem(hDlg, IDC_CONTROLTYPE), CB_GETCURSEL, 0, 0); 
	joysticks[controller].deadzone = (int)SendMessage(GetDlgItem(hDlg, IDC_DEADZONE), CB_GETCURSEL, 0, 0);
	
}

// Get text from static text item
// ������������������������������
void GetButton(HWND hDlg, int item, wchar* Receiver)
{	
	GetDlgItemText(hDlg, item, Receiver, 8);	
}

// Set text in static text item
// ����������������������������
void SetButton(HWND hDlg, int item, wchar* value)
{	
	SetDlgItemText(hDlg, item, value);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Config dialog functions
// �����������������������

// OpenAbout
// ���������
void OpenAbout(HINSTANCE abouthInstance, HWND _hParent)
{
	DialogBox(abouthInstance,MAKEINTRESOURCE(IDD_ABOUT), _hParent, AboutDlg);
}

// AboutDlg
// ��������
BOOL CALLBACK AboutDlg(HWND abouthWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	wchar format[512];	
	
	switch (message)
	{		
		case WM_INITDIALOG:			
			wsprintf(format, L"PuruPuru v" _T(INPUT_VERSION) L" by Falcon4ever\n"
			L"Builddate: " _T(__DATE__) L"\n"
			L"www.multigesture.net");			
			SetDlgItemText(abouthWnd, IDC_ABOUT1, format);

			wsprintf(format, _T(INPUT_STATE));
			SetDlgItemText(abouthWnd, IDC_STATEI, format);
			
			wsprintf(format, _T(THANKYOU));
			SetDlgItemText(abouthWnd, IDC_ABOUT3, format);
		break;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK) 
			{
				EndDialog(abouthWnd, LOWORD(wParam));
				return TRUE;
			}
		break;
	}
    return FALSE;
}
