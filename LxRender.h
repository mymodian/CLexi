#include "stdafx.h"
#include "LxComposeDoc.h"
#include "LxCursor.h"
#include "GD_proxy_base.h"

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
	LxRender() = default;
	virtual ~LxRender() = default;
public:
	virtual void create_caret(int height, int width) = 0;
	virtual void show_caret(LxCursor* cursor) = 0;
	virtual void hide_caret() = 0;
	virtual void DrawSection(CDC* pDC, Section* section) = 0;
	virtual void ClearSection(CDC* pDC, Section* section) = 0;
	virtual void DrawDocument(CDC* pDC, Section* section) = 0;
	virtual ComposeDoc* get_compose_doc() = 0;
	virtual GD_proxy_base* get_gd_proxy() = 0;
};

class LxContexRender : public LxRender
{
public:
	LxContexRender() = delete;
	LxContexRender(const LxContexRender&) = delete;
	LxContexRender(ComposeDoc* compose_doc, GD_proxy_base* gd_proxy);
	virtual ~LxContexRender();
public:
	virtual void create_caret(int height, int width) override;
	virtual void show_caret(LxCursor* cursor) override;
	virtual void hide_caret() override;
	virtual void DrawSection(CDC* pDC, Section* section) override;
	virtual void ClearSection(CDC* pDC, Section* section) override;
	virtual void DrawDocument(CDC* pDC, Section* section) override;
	virtual ComposeDoc* get_compose_doc() override;
	virtual GD_proxy_base* get_gd_proxy() override;
private:
	ComposeDoc* compose_doc_;
	GD_proxy_base* gd_proxy_;
};

class LxBorderRender : public LxRender
{
public:
	LxBorderRender() = delete;
	LxBorderRender(const LxBorderRender&) = delete;
	LxBorderRender(LxRender* base_render);
	virtual ~LxBorderRender();
public:
	virtual void create_caret(int height, int width) override;
	virtual void show_caret(LxCursor* cursor) override;
	virtual void hide_caret() override;
	virtual void DrawSection(CDC* pDC, Section* section) override;
	virtual void ClearSection(CDC* pDC, Section* section) override;
	virtual void DrawDocument(CDC* pDC, Section* section) override;
	virtual ComposeDoc* get_compose_doc() override;
	virtual GD_proxy_base* get_gd_proxy() override;
private:
	void DrawBorder(CDC* pDC);
	void DrawPageSpace(CDC* pDC, int bottom_pos, int pages_space);
private:
	LxRender* base_render_;
};

class LxScrollRender : public LxRender
{
public:
	LxScrollRender() = delete;
	LxScrollRender(const LxScrollRender&) = delete;
	LxScrollRender(LxRender* base_render);
	virtual ~LxScrollRender();
public:
	virtual void create_caret(int height, int width) override;
	virtual void show_caret(LxCursor* cursor) override;
	virtual void hide_caret() override;
	virtual void DrawSection(CDC* pDC, Section* section) override;
	virtual void ClearSection(CDC* pDC, Section* section) override;
	virtual void DrawDocument(CDC* pDC, Section* section) override;
	virtual ComposeDoc* get_compose_doc() override;
	virtual GD_proxy_base* get_gd_proxy() override;
public:
	void set_scroll_size_total(int width_total, int height_total);
	void set_scroll_pos(int hscroll_pos, int vscroll_pos);
private:
	LxRender* base_render_;
};

#endif