#include "stdafx.h"

#ifndef __GD_PROXY_BASE_H
#define __GD_PROXY_BASE_H

class CCLexiView;

class GD_proxy_base
{
public:
	GD_proxy_base() = default;
	~GD_proxy_base() = default;
	void init();
public:
	void create_caret(int height, int width);
	void hide_caret();
	void show_caret(int x, int y);
	void set_scroll_size_total(int width_total, int height_total);
	void set_scroll_pos(int hscroll_pos, int vscroll_pos);
	void set_font_index(size_t font_index);
	void set_color(COLORREF color);
private:
	CCLexiView* the_view;
};

#endif