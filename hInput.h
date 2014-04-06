#pragma once

namespace wtwBIU
{
	static const int BIU_BOLD_FLAG =		0x1;
	static const int BIU_ITALIC_FLAG =		0x2;
	static const int BIU_UNDERLINE_FLAG =	0x4;
	static const int BIU_ALL_MODS_FLAG =	0x7;

	void addFormat(HWND hInput, int flag);
};
