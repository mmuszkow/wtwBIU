#pragma once

#include "stdinc.h"

namespace wtwBIU {

	namespace config
	{
		static const wchar_t INPUT_FONT[] = L"wtwBIU/inputFont";
		static const wchar_t INPUT_SIZE[] = L"wtwBIU/inputFontSize";
		static const wchar_t BUTTONS_VISIBLE[] = L"wtwBIU/showButtons";
	};

	class SettingsController
	{
		void* _config;

		SettingsController() : _config(NULL) {}
		SettingsController(const SettingsController&);
	public:
		static SettingsController& getInstance()
		{
			static SettingsController instance;
			return instance;
		}

		void init();
		wstring getWStr(const wchar_t* name, const wchar_t* def);
		string getStr(const wchar_t* name, const wchar_t* def);
		int getInt(wchar_t const* name, int def);
		void setStr(const wchar_t* name, const wchar_t* val);
		void setInt(wchar_t const*name, int const val);
		void deinit();
	};
}; // namespace utlFTP
