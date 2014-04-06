#include "stdinc.h"

#include "hInput.h"

namespace wtwBIU 
{
	void addFormat(HWND hInput, int flag)
	{
		int modStatus = reinterpret_cast<int>(GetPropW(hInput, L"mod_status"));
		if(flag & BIU_BOLD_FLAG)
		{
			SendMessage(hInput, WM_CHAR, L'<', 0);
			if(modStatus & BIU_BOLD_FLAG)
			{
				SendMessage(hInput, WM_CHAR, L'/', 0);
				modStatus &= ~BIU_BOLD_FLAG;
			}
			else
				modStatus |= BIU_BOLD_FLAG;
			SendMessage(hInput, WM_CHAR, L'b', 0);
			SendMessage(hInput, WM_CHAR, L'>', 0);
		}
		if(flag & BIU_ITALIC_FLAG)
		{
			SendMessage(hInput, WM_CHAR, L'<', 0);
			if(modStatus & BIU_ITALIC_FLAG)
			{
				SendMessage(hInput, WM_CHAR, L'/', 0);
				modStatus &= ~BIU_ITALIC_FLAG;
			}
			else
				modStatus |= BIU_ITALIC_FLAG;
			SendMessage(hInput, WM_CHAR, L'i', 0);
			SendMessage(hInput, WM_CHAR, L'>', 0);
		}
		if(flag & BIU_UNDERLINE_FLAG)
		{
			SendMessage(hInput, WM_CHAR, L'<', 0);
			if(modStatus & BIU_UNDERLINE_FLAG)
			{
				SendMessage(hInput, WM_CHAR, L'/', 0);
				modStatus &= ~BIU_UNDERLINE_FLAG;
			}
			else
				modStatus |= BIU_UNDERLINE_FLAG;
			SendMessage(hInput, WM_CHAR, L'u', 0);
			SendMessage(hInput, WM_CHAR, L'>', 0);
		}
		SetPropW(hInput, L"mod_status", (HANDLE)modStatus);
	}
};
