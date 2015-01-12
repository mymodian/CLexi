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
private:
	CCLexiView* the_view;
};

#endif