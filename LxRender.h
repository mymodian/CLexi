#include "stdafx.h"
#include "LxComposeDoc.h"
#include "LxCursor.h"

#ifndef __LX_RENDER_H
#define __LX_RENDER_H

/*
在render中绘制文档、页、段、行
擦出页、行
整页的绘制和部分绘制在不同函数中实现，同整段排版和部分排版一样
*/

class LxRender
{
public:
	LxRender() {}
	virtual ~LxRender() {}
	void ShowCousor(CDC* pDC, LxCursor* cursor);
public:
	virtual void DrawDocument(CDC* pDC) = 0;
};

class LxContexRender : public LxRender
{
public:
	LxContexRender(ComposeDoc* compose_doc);
	virtual ~LxContexRender();
public:
	virtual void DrawDocument(CDC* pDC) override;
private:
	ComposeDoc* compose_doc_;
};

class LxBorderRender : public LxRender
{
public:
	LxBorderRender(LxRender* base_render);
	virtual ~LxBorderRender();
public:
	virtual void DrawDocument(CDC* pDC) override;
private:
	void DrawBorder(CDC* pDC);
	void DrawPageSpace(CDC* pDC, int bottom_pos, int pages_space);
private:
	LxRender* base_render_;
};

#endif