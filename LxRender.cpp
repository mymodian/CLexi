#include "stdafx.h"
#include "LxRender.h"
#include "LxViewWindow.h"

//show cousor.
void LxRender::ShowCousor(CDC* pDC, LxCursor* cursor)
{
	SetCaretPos(cursor->point_x, cursor->point_y);
	//ShowCaret(HWND hWnd);
}

//context render,only draw text.
LxContexRender::LxContexRender(ComposeDoc* compose_doc) : compose_doc_(compose_doc) {}
LxContexRender::~LxContexRender() {}

void LxContexRender::DrawDocument(CDC* pDC)
{
	compose_doc_->Draw(pDC);
}
//border render,draw border and the base render which is decorated.
LxBorderRender::LxBorderRender(LxRender* base_render) : base_render_(base_render) {}
LxBorderRender::~LxBorderRender() {}
void LxBorderRender::DrawBorder()
{
	ComposeDoc* compose_doc_;
	int view_top = ViewWindow::GetViewWindowInstance()->get_top_pos();
	int view_bottom = ViewWindow::GetViewWindowInstance()->get_bottom_pos();
	for (page_iter page_ = compose_doc_->begin(); page_ != compose_doc_->end(); page_++)
	{
		if ((*page_)->get_top_pos() > )
	}
}
void LxBorderRender::DrawDocument(CDC* pDC)
{
	base_render_->DrawDocument(pDC);
}
