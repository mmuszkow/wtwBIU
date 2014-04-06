#pragma once

#include "stdinc.h"
#include "SettingsController.h"

namespace wtwBIU
{
	class SettingsPage
	{
		HWND hButtonsVisible;
		HWND hFontStatic;
		HWND hFontName;
		HWND hFontSize;

		static const int ROW_HEIGHT = 24;
	public:
		SettingsPage(HWND hParent, HINSTANCE hInst, int x, int y, int cx, int cy);
		~SettingsPage();

		void show();
		void hide();
		inline void move(int x, int y, int cx, int cy) {}
		void apply();
		void cancel() {}
	};
};