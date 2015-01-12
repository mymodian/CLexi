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