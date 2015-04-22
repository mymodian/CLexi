#include "stdafx.h"
#include "LxCommon.h"
#include "LxSrcFontFactory.h"

SrcFontFactory* SrcFontFactory::lpViewWindowInstance = nullptr;

void SrcFontFactory::store_stream_logfont(FILE* file, LOGFONT& logfont)
{
	fwrite(&logfont, 1, sizeof(LOGFONT), file);
}

void SrcFontFactory::build_stream_logfont(FILE* file, LOGFONT& logfont)
{
	fread(&logfont, 1, sizeof(LOGFONT), file);
}

void SrcFontFactory::store_stream(FILE* file, std::set<size_t>& font_list_still_using)
{
	store_stream_int(file, font_list_still_using.size());
	LOGFONT logfont;
	for (auto _index : font_list_still_using)
	{
		store_stream_int(file, _index);
		get_src_font(_index)->GetLogFont(&logfont);
		SrcFontFactory::store_stream_logfont(file, logfont);
	}
}
void SrcFontFactory::build_from_stream(FILE* file)
{
	int font_cnt = read_stream_int(file);
	int index;
	LOGFONT logfont;
	int max_index = 0;
	for (int i = 0; i < font_cnt; i++)
	{
		index = read_stream_int(file);
		if (index > max_index)
			max_index = index;
		SrcFontFactory::build_stream_logfont(file, logfont);
		CFont* font_t = new CFont();
		font_t->CreateFontIndirect(&logfont);
		if (index > font_list.size())
			font_list.resize(index + index / 2);
		font_list[index - 1] = font_t;
		font_map.insert(std::make_pair(logfont, index));
	}
	if (max_index > 0)
		font_list.resize(max_index);
}

void SrcFontFactory::clear()
{
	for (auto font : font_list)
		delete font;
	font_list.clear();
	font_map.clear();
}