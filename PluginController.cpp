#include "stdinc.h"
#include "PluginController.h"
#include "SettingsController.h"
#include "hInput.h"
#include "html2gg.h"

namespace wtwBIU
{
	int PluginController::onLoad(WTWFUNCTIONS *fn)
	{
		wtw = fn;

		SettingsController::getInstance().init();

		wtwOptionPageDef pg;
		pg.id = getPlugInfo()->pluginGUID;
		pg.parentID = WTW_OPTIONS_GROUP_PLUGINS;
		pg.caption = L"Formatowanie";
		pg.callback = onSettingsShow; 
		pg.cbData = this;
		wtw->fnCall(WTW_OPTION_PAGE_ADD, 
			reinterpret_cast<WTW_PARAM>(hInst), 
			reinterpret_cast<WTW_PARAM>(&pg));

		ggSendFmtFunc = wtw->fnCreate(BIU_GG_SEND_FORMATTED, ggSendFormatted, NULL);

		chatWndCreateHook = wtw->evHook(WTW_EVENT_ON_CHATWND_CREATE, onChatWndCreate, this);
		msgProcHook = wtw->evHook(WTW_EVENT_CHATWND_BEFORE_MSG_PROC, onMessage, this);
		msgSentHook = wtw->evHook(WTW_EVENT_ON_CHATWND_MSG_SENT, onMessageSent, this);

		wtwGraphics wg;
		wg.hInst = hInst;

		wg.graphID = BIU_BOLD;
		wg.resourceId = 0;
		wg.filePath = L"iconBold.png";
		wg.flags = WTW_GRAPH_FLAG_RELATIVE_DEF_PATH;
		if(wtw->fnCall(WTW_GRAPH_LOAD,reinterpret_cast<WTW_PARAM>(&wg), 0) != 1)
		{
			wg.filePath = NULL;
			wg.flags = 0;
			wg.resourceId = MAKEINTRESOURCE(106);
			wtw->fnCall(WTW_GRAPH_LOAD,reinterpret_cast<WTW_PARAM>(&wg), 0);
		}
		

		wg.graphID = BIU_ITALIC;
		wg.resourceId = 0;
		wg.filePath = L"iconItalic.png";
		wg.flags = WTW_GRAPH_FLAG_RELATIVE_DEF_PATH;
		if(wtw->fnCall(WTW_GRAPH_LOAD,reinterpret_cast<WTW_PARAM>(&wg), 0) != 1)
		{
			wg.filePath = NULL;
			wg.flags = 0;
			wg.resourceId = MAKEINTRESOURCE(105);
			wtw->fnCall(WTW_GRAPH_LOAD,reinterpret_cast<WTW_PARAM>(&wg), 0);
		}

		wg.graphID = BIU_UNDERLINE;
		wg.resourceId = 0;
		wg.filePath = L"iconUnderline.png";
		wg.flags = WTW_GRAPH_FLAG_RELATIVE_DEF_PATH;
		if(wtw->fnCall(WTW_GRAPH_LOAD,reinterpret_cast<WTW_PARAM>(&wg), 0) != 1)
		{
			wg.filePath = NULL;
			wg.flags = 0;
			wg.resourceId = MAKEINTRESOURCE(104);
			wtw->fnCall(WTW_GRAPH_LOAD,reinterpret_cast<WTW_PARAM>(&wg), 0);
		}

		srand(static_cast<unsigned int>(time(NULL)));

		return 0;
	}

	int PluginController::onUnload()
	{
		wtw->fnCall(WTW_OPTION_PAGE_REMOVE, 
			reinterpret_cast<WTW_PARAM>(hInst), 
			reinterpret_cast<WTW_PARAM>(getPlugInfo()->pluginGUID));
		SettingsController::getInstance().deinit();
		if(settPage)
			delete settPage;

		if(chatWndCreateHook)
			wtw->evUnhook(chatWndCreateHook);

		if(msgProcHook)
			wtw->evUnhook(msgProcHook);

		if(msgSentHook)
			wtw->evUnhook(msgSentHook);

		wtw->fnCall(WTW_CCB_FUNCT_CLEAR, reinterpret_cast<WTW_PARAM>(hInst), 0);

		if(ggSendFmtFunc)
			wtw->fnDestroy(ggSendFmtFunc);

		return 0;
	}

	WTW_PTR PluginController::ggSendFormatted(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		wtwMessageDef* msg = reinterpret_cast<wtwMessageDef*>(wParam);
	
		int cp1250len = WideCharToMultiByte(1250, 0, msg->msgMessage, -1, NULL, 0, 0, 0);
		if(cp1250len<=0) // empty message
			return 1; 

		char* cp1250 = new char[cp1250len+1];
		WideCharToMultiByte(1250, 0, msg->msgMessage, -1, cp1250, cp1250len, 0, 0);

		HtmlToGG* parser = new HtmlToGG(cp1250);

		int htmllen = parser->getHtml().size();
		int plainlen = parser->getPlain().size();
		int attlen = parser->getFormatsSize();

		if(	parser->getHtml().size() == 0 || 
			parser->getPlain().size() == 0 || 
			parser->getFormatsSize() == 0)
		{
			delete parser;
			delete [] cp1250;
			return 1; // smth went wrong
		}

		int	len = sizeof(gg_send_msg80) + htmllen +	plainlen + attlen;
		int plain_off = sizeof(gg_send_msg80) - 8 + htmllen;
		int attr_off = sizeof(gg_send_msg80) - 8 + htmllen + plainlen;
		
		char* raw_packet = new char[len];
		gg_send_msg80* packet = reinterpret_cast<gg_send_msg80*>(raw_packet);
		packet->type = GG_SEND_MSG80;
		packet->length = len-8;
		packet->recipient = _wtoi(msg->contactData.id);
		packet->seq = rand() | (rand() << 16);
		packet->msgclass = GG_CLASS_CHAT;
		packet->offset_plain = plain_off;
		packet->offset_attributes = attr_off;
		
		char* html = raw_packet + sizeof(gg_send_msg80);
		char* plain = raw_packet + 8 + plain_off;
		char* attr = raw_packet + 8 + attr_off;
		
		memcpy(html, parser->getHtml().c_str(), parser->getHtml().size());
		memcpy(plain, parser->getPlain().c_str(), parser->getPlain().size());
		memcpy(attr, parser->getFormats(), parser->getFormatsSize());

		wtwRawDataDef raw;
		raw.pData = raw_packet;
		raw.pDataLen = len;
		raw.flags = WTW_RAW_FLAG_BIN|WTW_RAW_FLAG_EXT;

		wchar_t fn[512] = {0};
		wsprintf(fn, L"%s/%d/%s", 
			msg->contactData.netClass, 
			msg->contactData.netId, 
			WTW_PF_RAW_DATA_SEND);

		WTWFUNCTIONS* wtw = PluginController::getInstance().getWTWFUNCTIONS();
		WTW_PTR res = wtw->fnCall(fn, reinterpret_cast<WTW_PARAM>(&raw), 0);

		delete [] raw_packet;
		delete parser;
		delete [] cp1250;

		if(res != len)
			return 1; // do nothing if we are offline or msg wasn't send

		msg->msgFlags |= WTW_MESSAGE_FLAG_NOHTMLESC;
		wtw->fnCall(WTW_CHATWND_SHOW_MESSAGE, reinterpret_cast<WTW_PARAM>(msg), 0);

		return 0;
	}

	WTW_PTR PluginController::onChatWndCreate(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		wtwChatWindowInfo*	info = reinterpret_cast<wtwChatWindowInfo*>(lParam);
		SettingsController& sc = SettingsController::getInstance();

		HWND hInput = FindWindowExW(info->hWindow, NULL, L"A7A0B242-211B-424c-96E0-73D31C1A2436", NULL);
		if(!hInput)
			return 0;

		wstring fontName = sc.getWStr(config::INPUT_FONT, L"");
		int fontSize = sc.getInt(config::INPUT_SIZE, 0);
		if(fontName != L"" && fontSize > 0)
		{
			HFONT hFont = CreateFontW(fontSize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, fontName.c_str());
			if(hFont)
			{
				SendMessage(hInput, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), MAKELPARAM(TRUE, 0));
				DeleteObject(hFont);
			}
		}

		if(sc.getInt(config::BUTTONS_VISIBLE, 1) == 1)
		{
			wtwContactDef*		cnts = reinterpret_cast<wtwContactDef*>(wParam);

			if(info->iContacts == 1 && wcscmp(cnts[0].netClass, L"TLEN") == 0) // in Tlen there's no formatting
				return 0;

			PluginController& pc = PluginController::getInstance();

			wtwCommandEntry	entry;
			entry.pWnd = info->pWnd;
			entry.hInstance = pc.getDllHINSTANCE();

			entry.itemType = CCB_TYPE_SEPARATOR;
			entry.itemID = L"wtwBIU/sep";
			pc.getWTWFUNCTIONS()->fnCall(WTW_CCB_FUNCT_ADD, reinterpret_cast<WTW_PARAM>(&entry), 0);

			entry.itemFlags = CCB_FLAG_CHANGECAPTION|CCB_FLAG_CHANGEICON|CCB_FLAG_CHANGETIP;
			entry.itemType = CCB_TYPE_STANDARD;
			entry.callback = onClick;
			entry.cbData = hInput;

			entry.itemID = BIU_BOLD;
			entry.graphID = BIU_BOLD;
			entry.toolTip = L"Pogrubienie";
			entry.itemData = reinterpret_cast<void*>(BIU_BOLD_FLAG);
			pc.getWTWFUNCTIONS()->fnCall(WTW_CCB_FUNCT_ADD, reinterpret_cast<WTW_PARAM>(&entry), 0);

			entry.itemID = BIU_ITALIC;
			entry.graphID = BIU_ITALIC;
			entry.toolTip = L"Pochylenie";
			entry.itemData = reinterpret_cast<void*>(BIU_ITALIC_FLAG);
			pc.getWTWFUNCTIONS()->fnCall(WTW_CCB_FUNCT_ADD, reinterpret_cast<WTW_PARAM>(&entry), 0);

			entry.itemID = BIU_UNDERLINE;
			entry.graphID = BIU_UNDERLINE;
			entry.toolTip = L"Podkreœlenie";
			entry.itemData = reinterpret_cast<void*>(BIU_UNDERLINE_FLAG);
			pc.getWTWFUNCTIONS()->fnCall(WTW_CCB_FUNCT_ADD, reinterpret_cast<WTW_PARAM>(&entry), 0);
		}

		return 0;
	}

	bool equal(const wchar_t* str1, const wchar_t* str2, size_t len)
	{
		for(size_t i=0; i<len; i++)
			if(str1[i] != str2[i])
				return false;
		return true;
	}

	wstring endProperly(const wstring& html)
	{
		wstring		ret;
		stack<int>	tags;
		wchar_t		tag[5];
		int			i, j, len;

		ret.reserve(html.size() + 16);
		i = 0;
		len = html.size();
		memset(tag, 0, sizeof(tag));

		while(i < len)
		{
			if(len - i >= 3)
			{
				for(j=0;j<4;j++)
					tag[j] = html[i+j];
			}
			else
				tag[0] = 0;

			if(i <= len-3 && equal(tag, L"<b>", 3))
			{
				tags.push(BIU_BOLD_FLAG);
				ret += L"<b>";
				i+=3;
			}
			else if(i <= len-3 && equal(tag, L"<i>", 3))
			{
				tags.push(BIU_ITALIC_FLAG);
				ret += L"<i>";
				i+=3;
			}
			else if(i <= len-3 && equal(tag, L"<u>", 3))
			{
				tags.push(BIU_UNDERLINE_FLAG);
				ret += L"<u>";
				i+=3;
			}
			else if(i <= len-4 && (equal(tag, L"</b>", 4) || equal(tag, L"</i>", 4) || equal(tag, L"</u>", 4)))
			{
				if(!tags.empty())
				{
					switch(tags.top())
					{
					case BIU_BOLD_FLAG:
						ret += L"</b>";
						break;
					case BIU_ITALIC_FLAG:
						ret += L"</i>";
						break;
					case BIU_UNDERLINE_FLAG:
						ret += L"</u>";
						break;
					}
					tags.pop();
				}
				i+=4;
			}
			else
			{
				switch(html[i])
				{
				case L'<': ret += L"&lt;"; break;
				case L'>': ret += L"&gt;"; break;
				case L'&': ret += L"&amp;"; break;
				case L'"': ret += L"&quot;"; break;
				default: ret += html[i];
				}
				i++;
			}
		}

		while(!tags.empty())
		{
			switch(tags.top())
			{
			case BIU_BOLD_FLAG:
				ret += L"</b>";
				break;
			case BIU_ITALIC_FLAG:
				ret += L"</i>";
				break;
			case BIU_UNDERLINE_FLAG:
				ret += L"</u>";
				break;
			}
			tags.pop();
		}

		return ret;
	}

	WTW_PTR PluginController::onMessage(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		wtwBmpStruct *pBmp = reinterpret_cast<wtwBmpStruct*>(wParam);

		if (!pBmp || !pBmp->message.msgMessage)
			return BMP_OK;

		if(wcscmp(pBmp->message.contactData.netClass, L"TLEN") == 0) // in Tlen there's no formatting
			return BMP_OK;

		if(!(pBmp->message.msgFlags & WTW_MESSAGE_FLAG_OUTGOING)) // only outgoing
			return BMP_OK;

		wstring text(pBmp->message.msgMessage);

		if(	!(	text.find(L"<b>") != wstring::npos || 
				text.find(L"<i>") != wstring::npos || 
				text.find(L"<u>") != wstring::npos))
			return BMP_OK; // no b,i or u tags

		const wchar_t* prev_content = NULL;
		wstring propHtml = endProperly(text);
		if(text != propHtml)
		{
			prev_content = pBmp->message.msgMessage;
			pBmp->message.msgMessage = propHtml.c_str();
		}
		
		WTWFUNCTIONS* wtw = PluginController::getInstance().getWTWFUNCTIONS();
		wchar_t fn[512] = {0};
		
		if(wcscmp(pBmp->message.contactData.netClass, L"GG") == 0) // if GG
		{		
			WTW_PTR res = wtw->fnCall(BIU_GG_SEND_FORMATTED, reinterpret_cast<WTW_PARAM>(&pBmp->message), 0);

			if(prev_content)
				pBmp->message.msgMessage = prev_content;

			if(res != 0)
				return BMP_OK; // if smth went wrong

			return BMP_NO_PROCESS;
		}

		// if not GG
		pBmp->message.msgFlags |= WTW_MESSAGE_FLAG_NOHTMLESC;

		wsprintf(fn, L"%s/%d/%s", 
			pBmp->message.contactData.netClass, 
			pBmp->message.contactData.netId, 
			WTW_PF_MESSAGE_SEND);
		
		if(wtw->fnCall(fn, reinterpret_cast<WTW_PARAM>(&pBmp->message), 0)  != 0)
		{
			if(prev_content)
				pBmp->message.msgMessage = prev_content;

			return BMP_OK; // do nothing if we are offline or msg wasn't send
		}

		wtw->fnCall(WTW_CHATWND_SHOW_MESSAGE, reinterpret_cast<WTW_PARAM>(&pBmp->message), 0);

		if(prev_content)
			pBmp->message.msgMessage = prev_content;

		return BMP_NO_PROCESS;
	}

	WTW_PTR PluginController::onMessageSent(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		wtwChatWindowInfo* info = reinterpret_cast<wtwChatWindowInfo*>(lParam);
		HWND hInput = FindWindowExW(info->hWindow, NULL, L"A7A0B242-211B-424c-96E0-73D31C1A2436", NULL);
		if(!hInput)
			return 0;

		int modStatus = reinterpret_cast<int>(GetPropW(hInput, L"mod_status"));
		if(modStatus & BIU_BOLD_FLAG)
		{
			SendMessage(hInput, WM_CHAR, L'<', 0);
			SendMessage(hInput, WM_CHAR, L'b', 0);
			SendMessage(hInput, WM_CHAR, L'>', 0);
		}
		if(modStatus & BIU_ITALIC_FLAG)
		{
			SendMessage(hInput, WM_CHAR, L'<', 0);
			SendMessage(hInput, WM_CHAR, L'i', 0);
			SendMessage(hInput, WM_CHAR, L'>', 0);
		}
		if(modStatus & BIU_UNDERLINE_FLAG)
		{
			SendMessage(hInput, WM_CHAR, L'<', 0);
			SendMessage(hInput, WM_CHAR, L'u', 0);
			SendMessage(hInput, WM_CHAR, L'>', 0);
		}
		return 0;
	}

	WTW_PTR PluginController::onClick(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		wtwCommandCallback* info = reinterpret_cast<wtwCommandCallback*>(wParam);

		if(info->action == CCB_ACTION_LCLICK)
		{	
			DWORD start, end;
			SendMessage(static_cast<HWND>(ptr), EM_GETSEL, (WPARAM)&start, (WPARAM)&end);

			if(start == end) // no selection
			{
				addFormat(static_cast<HWND>(ptr), reinterpret_cast<int>(info->itemData));
			}
			else // selection
			{
				SendMessage(static_cast<HWND>(ptr), EM_SETSEL, start, start);
				addFormat(static_cast<HWND>(ptr), reinterpret_cast<int>(info->itemData));
				SendMessage(static_cast<HWND>(ptr), EM_SETSEL, end+3, end+3);
				addFormat(static_cast<HWND>(ptr), reinterpret_cast<int>(info->itemData));
			}
		}

		return 0;
	}

	WTW_PTR PluginController::onSettingsShow(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		PluginController* plugInst = static_cast<PluginController*>(ptr);
		wtwOptionPageShowInfo* info = reinterpret_cast<wtwOptionPageShowInfo*>(wParam);

		wcscpy(info->windowCaption, L"Formatowanie wysy³anego tekstu");
		wcscpy(info->windowDescrip, L"Pogrubienie, pochylenie, podkreœlenie");

		if (!plugInst->settPage)
			plugInst->settPage = new SettingsPage(info->handle,plugInst->hInst,info->x,info->y,info->cx,info->cy);

		switch (info->action)
		{
		case WTW_OPTIONS_PAGE_ACTION_SHOW:
			plugInst->settPage->show();
			return 0;
		case WTW_OPTIONS_PAGE_ACTION_MOVE:
			plugInst->settPage->move(info->x, info->y, info->cx, info->cy);
			return 0;
		case WTW_OPTIONS_PAGE_ACTION_HIDE:
			plugInst->settPage->hide();
			return 0;
		case WTW_OPTIONS_PAGE_ACTION_CANCEL:
			plugInst->settPage->cancel();
			delete plugInst->settPage;
			plugInst->settPage = NULL;
			return 0;
		case WTW_OPTIONS_PAGE_ACTION_APPLY:
			plugInst->settPage->apply();
			return 0;
		case WTW_OPTIONS_PAGE_ACTION_OK:
			plugInst->settPage->apply();
			delete plugInst->settPage;
			plugInst->settPage = NULL;
			return 0;
		}

		return 0;
	}
};
