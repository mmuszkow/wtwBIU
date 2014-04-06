/**
 * Copyright (c) 2009 Maciej Muszkowski
 */

#pragma once

#include "stdinc.h"
#include "ggHeaders.h"

using namespace std;

#define TAGS_NO	7

static char okTags[TAGS_NO][3] = {
	"b",
	"i",
	"u",
	"/b",
	"/i",
	"/u",
	"br"
};

/// Klasa konwetujaca html w utf na format uzywany przez starsze GG (< 8.x)
class HtmlToGG 
{
#pragma pack(push, 1)
	/** 
	  * \brief gg_msg_richtext_format + gg_msg_richtext_color
	  *
	  * Ze wzgledu na to ze informacja o kolorze w wersji gg 8.x wystepuje zawsze
	  * laczymy te 2 struktury zeby nie rzutowac
	  */

	struct gg_msg_color {
		struct gg_msg_richtext_format format;
		struct gg_msg_richtext_color color;
	};
#pragma pack(pop)
	/// Struktura zawierajaca informacje o tagu html
	struct HtmlTag
	{
		/// nazwa taga (b,u,i,font,span)
		string name;
		/// Poczatek (wystapienie "<")
		unsigned int start;
		/// Koniec (wystapienie ">")
		unsigned int end;
	};
	/// html przesylany do servera razem z <span... w UTF-8
	string html;
	/// czysty tekst w cp-1250, bez tagow html
	string plain;
	/// formatowanie tekstu wg starego gg
	unsigned char* format;
	/// Rozmiar struktury formatowania
	unsigned int formatlen;

	/// czy dany tag jest na liscie dozwolonych
	bool isOkTag(const string& tag);
	/// Zamienia /r/n na </ br>
	string rnToBr(const string& str);
	/// przepuszcza tylko tagi <b> <u> <i> <font color=>
	string filter(const string& str);
public:
	/// Parsuje html podany w argumencie
	HtmlToGG(const char* str);

	/// html przesylany do servera razem z <span... w UTF-8
	inline string& getHtml() { return html; }
	/// czysty tekst w cp-1250, bez tagow html
	inline string& getPlain() { return plain; }
	/// formatowanie tekstu wg starego gg
	inline unsigned char* getFormats() { return format; }
	/// Rozmiar struktury formatowania
	inline unsigned int getFormatsSize() { return formatlen; }

	static string htmlencode(const string &c);

	~HtmlToGG()	
	{
		if(format) delete [] format;	
	}
	
};
