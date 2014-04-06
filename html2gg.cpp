#include "stdinc.h"
#include "html2gg.h"

string HtmlToGG::rnToBr(const string& str)
{
	string ret;
	unsigned int i=0;
	unsigned int max=str.size()-1;
	while(i<max)
	{
		if(str[i]!='\r' && str[i+1]!='\n')
		{
			ret.append(&str[i], 1);
			i++;
		}
		else
		{
			ret += "<br>";
			i+=2;
		}
	}
	if(i<str.size())
		ret.append(&str[i], 1);
	return ret;
}

bool HtmlToGG::isOkTag(const string& tag)
{
	for(unsigned int i=0;i<TAGS_NO;i++)
		if(strcmp(okTags[i],tag.c_str())==0)
			return true;
	return false;
}

string HtmlToGG::filter(const string& str)
{
	vector<HtmlTag> tagsList;
	unsigned int i=0, max=str.size();
	string ret, name, inside;
	bool tagName=false; // wciaz trwa nazwa tagu (nie bylo spacji)
 	HtmlTag temp;
	
	while(i<max)
	{
		if(str[i]=='<')
		{
			temp.start = i;
			name.clear();
			tagName=true;
		}
		else
		if(str[i]==' ')
		{
			tagName=false;
		}
		else
		if(str[i]=='>' && name.size()>2)
		{
			tagName=false;
			temp.end = i;
			name.assign(name.substr(1,name.size()-1));
			temp.name.assign(name);
			tagsList.push_back(temp);
			temp.start=0;
			temp.end=0;
			temp.name.clear();
		}

		if(tagName)
		{
			name+=str[i];
		}

		i++;
	}

	unsigned int last = 0;
	if(tagsList.empty())
		return str;
	for(i=0;i<tagsList.size();i++)
	{
		ret += str.substr(last,tagsList[i].start-last);
		last = tagsList[i].end+1;
		if(isOkTag(tagsList[i].name))
		{
			ret += str.substr(tagsList[i].start,tagsList[i].end-tagsList[i].start+1);
		}
		else
		{
			ret += htmlencode(str.substr(tagsList[i].start,tagsList[i].end-tagsList[i].start+1));
		}
	}
	ret += str.substr(last,str.size()-last);

	return ret;
}

HtmlToGG::HtmlToGG(const char* str) : format(NULL)
{			
	vector<gg_msg_color> formats;

	gg_msg_richtext header;
	gg_msg_color def;

	header.flag = 0x02;
	header.length = 6;

	def.format.position = 0;
	def.format.font = GG_FONT_COLOR;
	def.color.red = 0;
	def.color.green = 0;
	def.color.blue = 0;

	//cpy(&formats,(char*)&header,sizeof(gg_msg_richtext));
	formats.push_back(def);

	char* utf8 = gg_cp_to_utf8(str);
	string utf(utf8);
	free(utf8);

	// dodajemy span do html-a
	string temp = "<span style=\"color:#000000; font-family:'MS Shell Dlg 2'; font-size:9pt; \">";
	temp += filter(rnToBr(utf));
	temp += "</span>";
	temp += '\0';
	html.assign(temp);
	
	// obrobka tagow <b> <u> <i>
	bool bold=false,italic=false,underline=false;
	unsigned int pos; // pozycja razem z formatowaniem
	unsigned int text_pos = 1; // pozycja w tekscie (po usunieciu formatowania)
	unsigned int str_len=strlen(str);
	bool*tag = new bool[str_len]; // tutaj -1 jesli czysty tekst, 1 jesli znacznik
	for(pos=0;pos<str_len;pos++) tag[pos]=false;
	if(str_len>2)
	{
		pos=2;
		text_pos = 2;
		while(pos<str_len)
		{
			if(bold||italic||underline) // moga wystapic dopiero przy pierwszym przejsciu petli
			{
				if(str[pos]=='>' && (str[pos-1]=='b'||str[pos-1]=='B') && str[pos-2]=='/' && str[pos-3]=='<') // </b> </B>
				{
					def.format.font &= ~GG_FONT_BOLD;
					text_pos -= 4;
					def.format.position = text_pos+1;
					formats.push_back(def);
					bold=false;
					tag[pos]=true; tag[pos-1]=true; tag[pos-2]=true; tag[pos-3]=true;// oznaczamy ze 4 poprzednie znaki sa znacznikiem a nie plain text-em
				}
				else if(str[pos]=='>' && (str[pos-1]=='i'||str[pos-1]=='I') && str[pos-2]=='/' && str[pos-3]=='<') // </i> </I>
				{
					def.format.font &= ~GG_FONT_ITALIC;
					text_pos -= 4;
					def.format.position = text_pos+1;
					formats.push_back(def);
					italic=false;
					tag[pos]=true; tag[pos-1]=true; tag[pos-2]=true; tag[pos-3]=true;
				}
				else if(str[pos]=='>' && (str[pos-1]=='u'||str[pos-1]=='U') && str[pos-2]=='/' && str[pos-3]=='<') // </u> </U>
				{
					def.format.font &= ~GG_FONT_UNDERLINE;
					text_pos -= 4;
					def.format.position = text_pos+1;
					formats.push_back(def);
					underline=false;
					tag[pos]=true; tag[pos-1]=true; tag[pos-2]=true; tag[pos-3]=true;
				}
			}
			if(str[pos]=='>' && (str[pos-1]=='b'||str[pos-1]=='B') && str[pos-2]=='<') // <b> <B>
			{
				def.format.font |= GG_FONT_BOLD;
				text_pos -= 3;
				def.format.position = text_pos+1;
				formats.push_back(def);
				bold=true;
				tag[pos]=true; tag[pos-1]=true; tag[pos-2]=true; // oznaczamy ze 3 poprzednie znaki sa znacznikiem a nie plain text-em

			}
			else if(str[pos]=='>' && (str[pos-1]=='i'||str[pos-1]=='I') && str[pos-2]=='<') // <i> <I>
			{
				def.format.font |= GG_FONT_ITALIC;
				text_pos -= 3;
				def.format.position = text_pos+1;
				formats.push_back(def);
				italic=true;
				tag[pos]=true; tag[pos-1]=true; tag[pos-2]=true;
			}
			else if(str[pos]=='>' && (str[pos-1]=='u'||str[pos-1]=='U') && str[pos-2]=='<') // <u> <U>
			{
				def.format.font |= GG_FONT_UNDERLINE;
				text_pos -= 3;
				def.format.position = text_pos+1;
				formats.push_back(def);
				underline=true;
				tag[pos]=true; tag[pos-1]=true; tag[pos-2]=true;
			}
			pos++; 
			text_pos++;
		}
	}

	// tworzenie plain-textu
	for(pos=0;pos<str_len;pos++)
	{
		if(!tag[pos])
			plain+=str[pos];
	}
	plain += '\0';
	delete [] tag; 

	//usuwanie duplikatow (w sytuacji gdy jeden tag sie konczy a zaczyna nastepny)
	vector<gg_msg_color> cleaned;
	for(pos=0;pos<formats.size()-1;pos++)
		if(formats[pos].format.position!=formats[pos+1].format.position)
			cleaned.push_back(formats[pos]);
	if(formats[pos].format.position!=text_pos) // po co dodawac informacje o konczacym sie tagu skoro za nim i tak nic nie ma..
		cleaned.push_back(formats[pos]);

	this->formatlen = cleaned.size()*sizeof(gg_msg_color)+sizeof(gg_msg_richtext);
	header.length = formatlen-sizeof(gg_msg_richtext);
	this->format = new unsigned char[formatlen];
	memcpy(format,&header,sizeof(gg_msg_richtext));
	for(pos=0;pos<cleaned.size();pos++)
		memcpy(format+sizeof(gg_msg_richtext)+pos*sizeof(gg_msg_color),&cleaned[pos],sizeof(gg_msg_color));
}

string HtmlToGG::htmlencode(const string &c)
{  
	string ok;
	int max = c.length();
	for(int i=0; i<max; i++)
	{
	switch(c[i])
		{
		case '<': ok.append("&lt;"); break;
		case '>': ok.append("&gt;"); break;
		case '&': ok.append("&amp;"); break;
		case '"': ok.append("&quot;"); break;
		default: ok.append( &c[i], 1);
		}
	}
	return ok;
}
