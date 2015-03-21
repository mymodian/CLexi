#include "stdafx.h"
#include "GD_proxy_base.h"
#include "CLexiDoc.h"
#include "CLexiView.h"
#include "MainFrm.h"

void GD_proxy_base::init()
{
	CMainFrame* p = (CMainFrame*)AfxGetMainWnd();
	the_view = (CCLexiView*)(p->GetActiveView());
}

void GD_proxy_base::set_font_index(size_t font_index)
{
	the_view->set_font_index(font_index);
}
void GD_proxy_base::set_color(COLORREF color)
{
	the_view->set_color(color);
}
void GD_proxy_base::create_caret(int height, int width)
{
	the_view->create_caret(height, width);
}
void GD_proxy_base::hide_caret()
{
	the_view->hide_caret();
}
void GD_proxy_base::show_caret(int x, int y)
{
	the_view->show_caret(x, y);
}
void GD_proxy_base::set_scroll_size_total(int width_total, int height_total)
{
	the_view->set_scroll_size_total(width_total, height_total);
}
void GD_proxy_base::set_scroll_pos(int hscroll_pos, int vscroll_pos)
{
	the_view->set_scroll_pos(hscroll_pos, vscroll_pos);
}