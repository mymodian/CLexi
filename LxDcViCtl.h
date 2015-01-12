#include "stdafx.h"
#include "LxDocument.h"
#include "LxComposeDoc.h"
#include "LxTreeBase.h"
#include "LxViewWindow.h"
#include "LxPaper.h"
#include "LxCursor.h"
#include "LxComposeAlgom.h"
#include "LxRender.h"

#ifndef __LX_DOCUMENT_VIEW_CONTROL_H
#define __LX_DOCUMENT_VIEW_CONTROL_H

class LxDcViCtl
{
public:
	LxDcViCtl();
	~LxDcViCtl();
	void init(CDC* pDC);
public:
	void insert(size_t position, size_t  count);
	void insert(size_t position, size_t  count, size_t font_src_index);
	void remove(size_t position);
	void remove(size_t position_begin, size_t position_end);
	void modify_font(size_t position_begin, size_t position_end, size_t font_src_index);
	void modify_color(size_t position_begin, size_t position_end, size_t color_src_index);
//≈≈∞Êœ‡πÿ
public:
	void modify_layout();
	void compose_complete(CDC* pDC);
	void draw_complete(CDC* pDC);
private:
	Document document;
	ComposeDoc compose_doc;
	TreeBase font_tree;
	TreeBase color_tree;
	LxCursor cursor;
	GD_proxy_base gd_proxy;
	LxRender* render;
};

#endif