#include "stdafx.h"
#include "LxDocument.h"
#include <list>

#ifndef _LX_STRUCTURED_CONTEXT_H_
#define _LX_STRUCTURED_CONTEXT_H_

//ÑÕÉ«ºÍ×ÖÌå
struct StructuredSrcContext
{
	int pos_begin_global;
	std::list<std::pair<int, int>> srcinfo_list;		//cnt & src_index
	inline void clear()
	{
		srcinfo_list.clear();
	}
};

struct StructuredSectionContext
{
	StructuredSrcContext color_info_list;
	StructuredSrcContext font_info_list;
	std::list<std::vector<TCHAR>*> doc_context;
	~StructuredSectionContext()
	{
		clear();
	}
	inline bool empty()
	{
		return doc_context.empty();
	}
	inline void clear()
	{
		for (auto pgh : doc_context)
		{
			delete pgh;
		}
		doc_context.clear();
		color_info_list.clear();
		font_info_list.clear();
	}
	inline size_t size()
	{
		size_t _size = 0;
		for (auto pgh : doc_context)
		{
			_size += pgh->size();
		}
		return _size;
	}
};

#endif