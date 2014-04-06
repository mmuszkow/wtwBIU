#include "stdinc.h"
#include "SettingsController.h"
#include "PluginController.h"

namespace wtwBIU
{
	void SettingsController::init()
	{
		if(_config)
			return;

		WTWFUNCTIONS *wtw = PluginController::getInstance().getWTWFUNCTIONS();
		HINSTANCE hInst = PluginController::getInstance().getDllHINSTANCE();

		wtwMyConfigFile configName;
		initStruct(configName);
		configName.bufferSize = MAX_PATH + 1;
		configName.pBuffer = new wchar_t[MAX_PATH + 1];

		wtw->fnCall(WTW_SETTINGS_GET_MY_CONFIG_FILE, reinterpret_cast<WTW_PARAM>(&configName), reinterpret_cast<WTW_PARAM>(hInst));
		_config = reinterpret_cast<void*>(wtw->fnCall(WTW_SETTINGS_INIT, reinterpret_cast<WTW_PARAM>(configName.pBuffer), reinterpret_cast<WTW_PARAM>(hInst)));
		delete [] configName.pBuffer;
	}

	wstring SettingsController::getWStr(const wchar_t *name, const wchar_t* def)
	{
		if (_config)
		{
			WTWFUNCTIONS *wtw = PluginController::getInstance().getWTWFUNCTIONS();
			wtw->fnCall(WTW_SETTINGS_READ, reinterpret_cast<WTW_PARAM>(_config), 0);
			wchar_t* tmp = NULL;
			wtwGetStr(wtw, _config, name, def, &tmp);
			wstring ret(tmp);
			delete [] tmp;
			return ret;
		}
		return wstring(L"");
	}

	string SettingsController::getStr(const wchar_t *name, const wchar_t* def)
	{
		if (_config)
		{
			WTWFUNCTIONS *wtw = PluginController::getInstance().getWTWFUNCTIONS();
			wtw->fnCall(WTW_SETTINGS_READ, reinterpret_cast<WTW_PARAM>(_config), 0);
			wchar_t* tmp = NULL;
			wtwGetStr(wtw, _config, name, def, &tmp);

			char val[1024];
			wcstombs(val,tmp,1024);

			return string(val);
		}
		return string("");
	}

	void SettingsController::deinit()
	{
		if (_config)
		{
			WTWFUNCTIONS *wtw = PluginController::getInstance().getWTWFUNCTIONS();
			HINSTANCE hInst = PluginController::getInstance().getDllHINSTANCE();
			wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(_config), reinterpret_cast<WTW_PARAM>(hInst));
		}
	}

	void SettingsController::setStr(wchar_t const*name, wchar_t const*val)
	{
		if (_config)
		{
			WTWFUNCTIONS *wtw = PluginController::getInstance().getWTWFUNCTIONS();
			HINSTANCE hInst = PluginController::getInstance().getDllHINSTANCE();
			wtwSetStr(wtw,_config,name,val);
			wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(_config), 0);
		}
	}

	void SettingsController::setInt(wchar_t const*name, int const val)
	{
		if (_config)
		{
			WTWFUNCTIONS *wtw = PluginController::getInstance().getWTWFUNCTIONS();
			HINSTANCE hInst = PluginController::getInstance().getDllHINSTANCE();
			wtwSetInt(wtw,_config,name,val);
			wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(_config), 0);
		}
	}

	int SettingsController::getInt(wchar_t const*name, int def)
	{
		if (_config)
		{
			WTWFUNCTIONS *wtw = PluginController::getInstance().getWTWFUNCTIONS();
			wtw->fnCall(WTW_SETTINGS_READ, reinterpret_cast<WTW_PARAM>(_config), 0);

			return wtwGetInt(wtw, _config, name, def);
		}
		return -1;
	}
}
