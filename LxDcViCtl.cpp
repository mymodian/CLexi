#include "stdafx.h"
#include "LxDcViCtl.h"
#include "LxSrcFontFactory.h"

LxDcViCtl::LxDcViCtl() {}
LxDcViCtl::~LxDcViCtl() {}

void LxDcViCtl::init(CDC* pDC)
{
	CFont* font = new CFont;
	font->CreateFont(-16, 0, 0, 0, 100, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, L"微软雅黑");
	LOGFONT logfont;
	font->GetLogFont(&logfont);
	size_t font_index = SrcFontFactory::GetFontFactInstance()->insert_src_font(logfont);
	font_tree.insert(0, 0, font_index);

	pDC->SelectObject(font);
	TEXTMETRIC trx;
	pDC->GetTextMetrics(&trx);
	cursor.height = trx.tmHeight;

	delete font;
	color_tree.insert(0, 0, RGB(0, 0, 0));

	Paragraph* paragraph = new Paragraph();
	//设置默认的排版算法，对象应从排版算法管理结构中获取
	paragraph->SetComposeAlgom(new LxSimpleComposeAlgo());
	document.add_paragraph(paragraph);
	compose_doc.AttachColorInfo(&color_tree);
	compose_doc.AttachFontInfo(&font_tree);
	compose_doc.AttachPhyDoc(&document);

	compose_doc.compose_complete(pDC);

	cursor.point_x = LxPaper::left_margin;
	cursor.point_y = ViewWindow::GetViewWindowInstance()->border_height + LxPaper::top_margin;
	cursor.page = compose_doc.begin();
	cursor.paragraph = (*cursor.page)->begin();
	cursor.row = (*cursor.paragraph)->begin();
	cursor.index_inner = 0;
	cursor.width_used = 0;

	gd_proxy.init();
	render = new LxBorderRender(new LxContexRender(&compose_doc, &gd_proxy));
}

void LxDcViCtl::move_cursor(CDC* pDC, unsigned direction)
{
	switch (direction)
	{
	case VK_LEFT:
	{
		LxParagraphInDocIter pgh_doc_it(&compose_doc, cursor.page, cursor.paragraph);
		size_t cur_gbl_index_old = cursor.get_index_global() - 1;
		Paragraph* phy_pgh = cursor.get_phy_paragraph();
		if (cursor.get_index_inner_paragraph() == 0)
		{
			if (pgh_doc_it == compose_doc.pargraph_begin()) return;
			--pgh_doc_it;
			cur_gbl_index_old++;
			phy_pgh = (*(pgh_doc_it.get_paragraph()))->get_phy_paragraph();
		}
		compose_doc.calc_cursor(cursor, cur_gbl_index_old, phy_pgh, pDC);
	}
		break;
	case VK_RIGHT:
	{
		LxParagraphInDocIter pgh_doc_it(&compose_doc, cursor.page, cursor.paragraph);
		size_t cur_gbl_index_old = cursor.get_index_global() + 1;
		Paragraph* phy_pgh = cursor.get_phy_paragraph();
		if (cursor.get_index_inner_paragraph() == (*(cursor.paragraph))->get_phy_paragraph()->size())
		{
			++pgh_doc_it;
			if (pgh_doc_it == compose_doc.pargraph_end()) return;
			cur_gbl_index_old--;
			phy_pgh = (*(pgh_doc_it.get_paragraph()))->get_phy_paragraph();
		}
		compose_doc.calc_cursor(cursor, cur_gbl_index_old, phy_pgh, pDC);
	}
		break;
	case VK_UP:
		compose_doc.locate(cursor, pDC, cursor.point_x, cursor.point_y - 3);
		break;
	case VK_DOWN:
		compose_doc.locate(cursor, pDC, cursor.point_x, cursor.point_y + cursor.height + 3);
		break;
	default:
		return;
	}
	if (cursor.point_y + cursor.height > ViewWindow::GetViewWindowInstance()->get_bottom_pos())
	{
		ViewWindow::GetViewWindowInstance()->offset_y +=
			cursor.point_y + cursor.height - ViewWindow::GetViewWindowInstance()->get_bottom_pos();
		draw_complete(pDC);
	}
	else if (cursor.point_y < ViewWindow::GetViewWindowInstance()->get_top_pos())
	{
		ViewWindow::GetViewWindowInstance()->offset_y -=
			ViewWindow::GetViewWindowInstance()->get_top_pos() - cursor.point_y;
		draw_complete(pDC);
	}
	else
	{
		render->hide_caret();
		render->create_caret(cursor.height, cursor.height / 8);
		render->show_caret(&cursor);
	}
}

void LxDcViCtl::backspace()
{

}

bool LxDcViCtl::single_remove()
{
	Paragraph* pgh = (*(cursor.paragraph))->get_phy_paragraph();
	if (pgh->size() == 0 || cursor.get_index_inner_paragraph() == 0)
		return false;
	pgh->Delete(cursor.get_index_inner_paragraph() - 1);

	font_tree.remove(cursor.get_index_inner_paragraph() - 1, cursor.get_index_inner_paragraph());
	color_tree.remove(cursor.get_index_inner_paragraph() - 1, cursor.get_index_inner_paragraph());
	return true;
}

void LxDcViCtl::insert(TCHAR* src, size_t  count)
{
	//在cursor处执行插入操作
	Paragraph* pgh = (*(cursor.paragraph))->get_phy_paragraph();
	pgh->Insert(cursor.get_index_inner_paragraph(), src, count);

	font_tree.insert(cursor.get_index_global(), count);
	color_tree.insert(cursor.get_index_global(), count);
}
void LxDcViCtl::insert(TCHAR* src, size_t  count, size_t src_index)
{
}
void LxDcViCtl::remove(size_t position)
{
	if( position != 0 )
		remove(position-1, position);
}
void LxDcViCtl::remove(size_t position_begin, size_t position_end)
{
	font_tree.remove(position_begin, position_end);
	color_tree.remove(position_begin, position_end);
}
void LxDcViCtl::modify_font(size_t position_begin, size_t position_end, size_t font_src_index)
{
	font_tree.modify(position_begin,position_end,font_src_index);
}
void LxDcViCtl::modify_color(size_t position_begin, size_t position_end, size_t color_src_index)
{
	color_tree.modify(position_begin,position_end,color_src_index);
}

//partial
void LxDcViCtl::modify_layout(CDC* pDC, int count)
{
	assert(count != 0);
	size_t cur_gbl_index_old = cursor.get_index_global() + count;
	Paragraph* phy_pgh = cursor.get_phy_paragraph();

	//删除时计算新的cursor
	if (count < 0)
		compose_doc.calc_cursor(cursor, cur_gbl_index_old, phy_pgh, pDC);

	LxParagraphInDocIter pgh_doc_it(&compose_doc, cursor.page, cursor.paragraph);
	compose_doc.modify_index(pgh_doc_it, count);
	pgh_doc_it = compose_doc.modify(pgh_doc_it, cursor.row, pDC);
	compose_doc.relayout(pgh_doc_it);

	//插入时计算新的cursor
	if (count > 0)
		compose_doc.calc_cursor(cursor, cur_gbl_index_old, phy_pgh, pDC);
	//删除时由于先计算的cursor的row会被清除，所以需要再次计算cursor
	if (count < 0)
		compose_doc.calc_cursor(cursor, cur_gbl_index_old, phy_pgh, pDC);

	if (cursor.point_y + cursor.height > ViewWindow::GetViewWindowInstance()->get_bottom_pos())
	{
		ViewWindow::GetViewWindowInstance()->offset_y += 
			cursor.point_y + cursor.height - ViewWindow::GetViewWindowInstance()->get_bottom_pos();
	}
	else if (cursor.point_y < ViewWindow::GetViewWindowInstance()->get_top_pos())
	{
		ViewWindow::GetViewWindowInstance()->offset_y -=
			ViewWindow::GetViewWindowInstance()->get_top_pos() - cursor.point_y;
	}
}

void LxDcViCtl::locate(CDC* pDC, int doc_x, int doc_y)
{
	render->hide_caret();
	compose_doc.locate(cursor, pDC, doc_x, doc_y);
	render->create_caret(cursor.height, cursor.height / 8);
	render->show_caret(&cursor);
}

//full text
void LxDcViCtl::compose_complete(CDC* pDC)
{
	compose_doc.compose_complete(pDC);
}
void LxDcViCtl::draw_complete(CDC* pDC)
{
	render->hide_caret();
	pDC->SetBkMode(TRANSPARENT);
	render->DrawDocument(pDC);
	render->create_caret(cursor.height, cursor.height/8);
	render->show_caret(&cursor);
}