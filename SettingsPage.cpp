#include "stdinc.h"

#include "SettingsPage.h"

namespace wtwBIU
{
	SettingsPage::SettingsPage(HWND hParent, HINSTANCE hInst, int x, int y, int cx, int cy)
	{
		SettingsController& settCtrl = SettingsController::getInstance();

		hButtonsVisible = CreateWindow(L"BUTTON",L"Pokazuj przyciski formatowania w oknie czatu",WS_CHILD|BS_AUTOCHECKBOX,
			x,y,cx,ROW_HEIGHT,hParent,0,hInst,0);
		hFontStatic = CreateWindow(L"STATIC",L"Czcionka okna wpisywania:",WS_CHILD,
			x,y+(ROW_HEIGHT<<1)-(ROW_HEIGHT>>1),cx,ROW_HEIGHT,hParent,0,hInst,0);
		hFontName = CreateWindow(L"EDIT",settCtrl.getWStr(config::INPUT_FONT, L"").c_str(),WS_CHILD|WS_BORDER|ES_CENTER,
			x+140,y+(ROW_HEIGHT<<1)-(ROW_HEIGHT>>1)-2,150,18,hParent,0,hInst,0);
		hFontSize = CreateWindow(L"EDIT",settCtrl.getWStr(config::INPUT_SIZE, L"0").c_str(),WS_CHILD|WS_BORDER|ES_NUMBER|ES_CENTER,
			x+292,y+(ROW_HEIGHT<<1)-(ROW_HEIGHT>>1)-2,30,18,hParent,0,hInst,0);
	
		HFONT hDefaultFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		SendMessage(hButtonsVisible, WM_SETFONT, reinterpret_cast<WPARAM>(hDefaultFont), MAKELPARAM(TRUE, 0));
		SendMessage(hFontStatic, WM_SETFONT, reinterpret_cast<WPARAM>(hDefaultFont), MAKELPARAM(TRUE, 0));
		SendMessage(hFontName, WM_SETFONT, reinterpret_cast<WPARAM>(hDefaultFont), MAKELPARAM(TRUE, 0));
		SendMessage(hFontSize, WM_SETFONT, reinterpret_cast<WPARAM>(hDefaultFont), MAKELPARAM(TRUE, 0));

		if(settCtrl.getInt(config::BUTTONS_VISIBLE, 1))
			SendMessage(hButtonsVisible, BM_SETCHECK, 1, 0);
	}

	SettingsPage::~SettingsPage()
	{
		DestroyWindow(hButtonsVisible);
		DestroyWindow(hFontStatic);
		DestroyWindow(hFontName);
		DestroyWindow(hFontSize);
	}

	void SettingsPage::show() 
	{
		ShowWindow(hButtonsVisible,SW_SHOW);
		ShowWindow(hFontStatic,SW_SHOW);
		ShowWindow(hFontName,SW_SHOW);
		ShowWindow(hFontSize,SW_SHOW);
	}

	void SettingsPage::hide() 
	{
		ShowWindow(hButtonsVisible,SW_HIDE);
		ShowWindow(hFontStatic,SW_HIDE);
		ShowWindow(hFontName,SW_HIDE);
		ShowWindow(hFontSize,SW_HIDE);
	}

	void SettingsPage::apply() 
	{
		wchar_t buff[256];
		SettingsController& settCtrl = SettingsController::getInstance();

		GetWindowText(hFontName,buff,256);
		settCtrl.setStr(config::INPUT_FONT,buff);
		GetWindowText(hFontSize,buff,256);
		settCtrl.setStr(config::INPUT_SIZE,buff);

		if(SendMessage(hButtonsVisible, BM_GETCHECK, 0, 0))
			settCtrl.setInt(config::BUTTONS_VISIBLE,1);
		else
			settCtrl.setInt(config::BUTTONS_VISIBLE,0);
	}
}
