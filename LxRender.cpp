#include "stdafx.h"
#include "LxPaper.h"
#include "LxRender.h"
#include "LxViewWindow.h"

//context render,only draw text.
LxContexRender::LxContexRender(ComposeDoc* compose_doc, GD_proxy_base* gd_proxy) 
	: compose_doc_(compose_doc), 
	gd_proxy_(gd_proxy) {}
LxContexRender::~LxContexRender() {}

void LxContexRender::DrawDocument(CDC* pDC)
{
	compose_doc_->Draw(pDC);
}

void LxContexRender::show_caret(LxCursor* cursor)
{
	gd_proxy_->show_caret(cursor->point_x, cursor->point_y);
}
ComposeDoc* LxContexRender::get_compose_doc()
{
	return compose_doc_;
}
GD_proxy_base* LxContexRender::get_gd_proxy()
{
	return gd_proxy_;
}

//border render,draw border and the base render which is decorated.
LxBorderRender::LxBorderRender(LxRender* base_render)
	: base_render_(base_render) {}
LxBorderRender::~LxBorderRender() {}

void LxBorderRender::DrawPageSpace(CDC* pDC, int bottom_pos, int pages_space)
{
	CRect rect(ViewWindow::GetViewWindowInstance()->border_width_left -
		ViewWindow::GetViewWindowInstance()->offset_x,
		bottom_pos - pages_space,
		ViewWindow::GetViewWindowInstance()->border_width_left + LxPaper::pixel_width -
		ViewWindow::GetViewWindowInstance()->offset_x,
		bottom_pos);
	FlushRect(pDC, &rect, ViewWindow::GetViewWindowInstance()->get_view_back_color());
}

void LxBorderRender::DrawBorder(CDC* pDC)
{
	// draw spaces between pages
	ComposeDoc* compose_doc_ = base_render_->get_compose_doc();
	int view_top = ViewWindow::GetViewWindowInstance()->get_top_pos();
	int view_bottom = ViewWindow::GetViewWindowInstance()->get_bottom_pos();
	int pages_space = ViewWindow::GetViewWindowInstance()->border_height;
	for (page_iter page_ = compose_doc_->begin(); page_ != compose_doc_->end(); page_++)
	{
		if ((*page_)->get_top_pos() > view_top && (*page_)->get_top_pos() < view_bottom)
		{
			DrawPageSpace(pDC, (*page_)->get_top_pos(), pages_space);
		}
		if ((*page_)->get_bottom_pos() > view_top && (*page_)->get_bottom_pos() < view_bottom)
		{
			DrawPageSpace(pDC, (*page_)->get_bottom_pos() + pages_space, pages_space);
			page_++;
			if (page_ == compose_doc_->end())
				break;
			continue;
		}
		if ((*page_)->get_top_pos() >= view_bottom)
			break;
	}
	// draw left and right border
	CRect rect_l(0, 0, ViewWindow::GetViewWindowInstance()->border_width_left, 
		ViewWindow::GetViewWindowInstance()->height);
	CRect rect_r(ViewWindow::GetViewWindowInstance()->width - ViewWindow::GetViewWindowInstance()->border_width_right,
		0, ViewWindow::GetViewWindowInstance()->width, ViewWindow::GetViewWindowInstance()->height);
	FlushRect(pDC, &rect_l, ViewWindow::GetViewWindowInstance()->get_view_back_color());
	FlushRect(pDC, &rect_r, ViewWindow::GetViewWindowInstance()->get_view_back_color());
}

void LxBorderRender::DrawDocument(CDC* pDC)
{
	base_render_->DrawDocument(pDC);
	DrawBorder(pDC);
}

void LxBorderRender::show_caret(LxCursor* cursor)
{
	base_render_->show_caret(cursor);
}
ComposeDoc* LxBorderRender::get_compose_doc()
{
	return base_render_->get_compose_doc();
}
GD_proxy_base* LxBorderRender::get_gd_proxy()
{
	return base_render_->get_gd_proxy();
}
