#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "stdafx.h"
using namespace std;

#ifndef __LEXI_SRC_FONT_FACTORY_H
#define __LEXI_SRC_FONT_FACTORY_H

class FontLessCmp
{
public:
	bool operator()(const LOGFONT& logfont1, const LOGFONT& logfont2)
	{
		if (logfont1.lfHeight < logfont2.lfHeight)
			return true;
		else if (logfont1.lfHeight > logfont2.lfHeight)
			return false;
		//if (logfont1.lfItalic && )
		if (_tcscmp(logfont1.lfFaceName, logfont2.lfFaceName) < 0)
			return true;
		return false;
	}
};

class SrcFontFactory
{
public:
	size_t insert_src_font(const LOGFONT& logfont)
	{
		auto it = font_map.find(logfont);
		if (it == font_map.end())
		{
			font_map.insert(std::make_pair(logfont, 1 + font_list.size()));
			CFont* font_t = new CFont();
			font_t->CreateFontIndirect(&logfont);
			font_list.push_back(font_t);
			return font_list.size();
		}
		return (*it).second;
	}
	CFont* get_src_font(size_t src_index)
	{
		return *(font_list.begin() + src_index - 1);
	}
	~SrcFontFactory()
	{
		for (auto font : font_list)
			delete font;
		font_list.clear();
	}
	void store_stream(FILE* file, std::set<size_t>& font_list_still_using);
	void build_from_stream(FILE* file);
	void clear();
public:
	static SrcFontFactory* GetFontFactInstance()
	{
		if (lpViewWindowInstance == nullptr)
			lpViewWindowInstance = new SrcFontFactory();
		return lpViewWindowInstance;
	}
	static void store_stream_logfont(FILE* file, LOGFONT& logfont);
	static void build_stream_logfont(FILE* file, LOGFONT& logfont);
private:
	static SrcFontFactory* lpViewWindowInstance;
	map<LOGFONT, size_t, FontLessCmp> font_map;
	vector<CFont*> font_list;
private:
	SrcFontFactory() = default;
	SrcFontFactory(const SrcFontFactory&) = delete;
	SrcFontFactory& operator=(const SrcFontFactory&) = delete;
};

#endif