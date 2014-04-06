#pragma once

#include "stdinc.h"
#include "SettingsPage.h"

namespace wtwBIU {

static const wchar_t BIU_BOLD[] =		L"wtwBIU/bold";
static const wchar_t BIU_ITALIC[] =		L"wtwBIU/italic";
static const wchar_t BIU_UNDERLINE[] =	L"wtwBIU/underline";

static const wchar_t BIU_GG_SEND_FORMATTED[] = L"wtwBIU/GGSendFormatted";

/** Singleton */
class PluginController
{
	// basic
	WTWFUNCTIONS*	wtw;
	HINSTANCE		hInst;

	HANDLE			ggSendFmtFunc;

	HANDLE			chatWndCreateHook;
	HANDLE			msgProcHook;
	HANDLE			msgSentHook;

	SettingsPage*	settPage;

	PluginController() : wtw(NULL), hInst(NULL), ggSendFmtFunc(NULL), 
		chatWndCreateHook(NULL), msgProcHook(NULL), msgSentHook(NULL),
		settPage(NULL) {}
	PluginController(const PluginController&);

	static WTW_PTR ggSendFormatted(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);

	static WTW_PTR onChatWndCreate(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);
	static WTW_PTR onMessage(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);
	static WTW_PTR onMessageSent(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);
	static WTW_PTR onClick(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);
	
	static WTW_PTR onSettingsShow(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);
public:

	static PluginController& getInstance()
	{
		static PluginController instance;
		return instance;
	}

	int onLoad(WTWFUNCTIONS *fn);

	int onUnload();

	inline const WTWPLUGINFO* getPlugInfo()
	{
		static WTWPLUGINFO _plugInfo = {
			sizeof(WTWPLUGINFO),						// struct size
			L"wtwBIU",									// plugin name
			L"Pogrubienie, pochylenie, podkreœlenie tekstu", // plugin description
			L"© 2011 Maciej Muszkowski",				// copyright
			L"Maciej Muszkowski",						// author
			L"maciek.muszkowski@gmail.com",				// authors contact
			L"http://www.alset.pl",						// authors webpage
			L"",										// url to xml with autoupdate data
			PLUGIN_API_VERSION,							// api version
			MAKE_QWORD(0, 1, 0, 0),						// plugin version
			WTW_CLASS_UTILITY,							// plugin class
			NULL,										// function called after "O wtyczce..." pressed
			L"{2713fdf8-9299-49db-a717-49db1b89e9ba}",	// guid
			NULL,										// dependencies (list of guids)
			0,											// options
			0, 0, 0										// reserved
		};
		return &_plugInfo;
	}

	inline void setDllHINSTANCE(const HINSTANCE h)
	{
		hInst = h;
	}

	inline HINSTANCE getDllHINSTANCE() const
	{
		return hInst;
	}

	inline WTWFUNCTIONS* getWTWFUNCTIONS() const
	{
		return wtw;
	}
};

}; // namespace utlFTP
