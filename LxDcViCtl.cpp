#include "stdafx.h"
#include "LxDcViCtl.h"
#include "LxSrcFontFactory.h"

LxDcViCtl::LxDcViCtl() {}
LxDcViCtl::~LxDcViCtl() {}

void LxDcViCtl::init(CDC* pDC)
{
	CFont* font = new CFont;
	font->CreateFont(-18, 0, 0, 0, 100, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, L"Consolas");
	LOGFONT logfont;
	font->GetLogFont(&logfont);
	default_font_index = SrcFontFactory::GetFontFactInstance()->insert_src_font(logfont);
	font_tree.insert(0, 0, default_font_index);

	pDC->SelectObject(font);
	TEXTMETRIC trx;
	pDC->GetTextMetrics(&trx);
	cursor.height = trx.tmHeight;

	delete font;
	default_color_index = RGB(0, 50, 150);
	color_tree.insert(0, 0, default_color_index);

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
	//cursor.width_used = 0;
	section.cursor_begin = cursor;
	section.cursor_end = cursor;
	section.trace = false;

	gd_proxy.init();
	gd_proxy.set_color(default_color_index);
	gd_proxy.set_font_index(default_font_index);
	render = new LxScrollRender(new LxBorderRender(new LxContexRender(&compose_doc, &gd_proxy)));
}

void LxDcViCtl::modify_mouse_hscroll(CDC* pDC, int hdistanse)
{
	if (ViewWindow::GetViewWindowInstance()->width >= compose_doc.total_width())
		return;
	int old_offset_x = ViewWindow::GetViewWindowInstance()->offset_x;
	int new_offset_x = ViewWindow::GetViewWindowInstance()->offset_x - hdistanse;
	if (hdistanse > 0)
	{
		ViewWindow::GetViewWindowInstance()->offset_x = new_offset_x < 0 ? 0 : new_offset_x;
	}
	else
	{
		int offset_x_max = compose_doc.total_width() - ViewWindow::GetViewWindowInstance()->width;
		ViewWindow::GetViewWindowInstance()->offset_x = new_offset_x > offset_x_max ? offset_x_max : new_offset_x;
	}
	if (ViewWindow::GetViewWindowInstance()->offset_x != old_offset_x)
		draw_complete(pDC);
}

void LxDcViCtl::modify_mouse_vscroll(CDC* pDC, int vdistanse)
{
	if (ViewWindow::GetViewWindowInstance()->height >= compose_doc.total_height())
		return;
	int old_offset_y = ViewWindow::GetViewWindowInstance()->offset_y;
	int new_offset_y = ViewWindow::GetViewWindowInstance()->offset_y - vdistanse;
	if (vdistanse > 0)
	{
		ViewWindow::GetViewWindowInstance()->offset_y = new_offset_y < 0 ? 0 : new_offset_y;
	}
	else
	{
		int offset_y_max = compose_doc.total_height() - ViewWindow::GetViewWindowInstance()->height;
		ViewWindow::GetViewWindowInstance()->offset_y = new_offset_y > offset_y_max ? offset_y_max : new_offset_y;
	}
	if (ViewWindow::GetViewWindowInstance()->offset_y != old_offset_y)
		draw_complete(pDC);
}

void LxDcViCtl::remove_section(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh)
{
	size_t index_b = section_begin_index < section_end_index ? section_begin_index : section_end_index;
	size_t index_e = section_end_index > section_begin_index ? section_end_index : section_begin_index;
	size_t pgh_b = section_begin_pgh < section_end_pgh ? section_begin_pgh : section_end_pgh;
	size_t pgh_e = section_begin_pgh < section_end_pgh ? section_end_pgh : section_begin_pgh;

	remove_phy_section(index_b, pgh_b, index_e, pgh_e);

	font_tree.remove(index_b, index_e);
	color_tree.remove(index_b, index_e);
	if (font_tree.empty() && color_tree.empty())
	{
		font_tree.insert(0, 0, default_font_index);
		color_tree.insert(0, 0, default_color_index);
	}

	compose_doc.remove_section(pDC, index_b, pgh_b, index_e, pgh_e);

	compose_doc.calc_cursor(cursor, index_b, document.get_pgh(pgh_b), pDC);
	section.cursor_begin = cursor;
	section.cursor_end = cursor;
}

void LxDcViCtl::section_wrap(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh,
	size_t section_end_index, size_t section_end_pgh)
{
	remove_section(pDC, section_begin_index, section_begin_pgh, section_end_index, section_end_pgh);
	if (cursor.tail_of_paragraph() || cursor.head_of_paragraph())
	{
		int direction = 0;
		if (cursor.tail_of_paragraph())
			direction = 1;
		Paragraph* pgh = insert_null_phy_paragraph(section_begin_pgh + direction);
		add_phy_paragraph(pDC, pgh, section_begin_pgh, direction);
	}
	else
	{
		size_t _offset_inner = document.get_offset_inner(section_begin_index, section_begin_pgh);
		Paragraph* new_phy_pgh = split_phy_paragraph(section_begin_pgh, _offset_inner);
		compose_splited_paragraph(pDC, section_begin_pgh, _offset_inner, new_phy_pgh);
	}
}

void LxDcViCtl::replace_section(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index,
	size_t section_end_pgh, TCHAR* cs, size_t len, size_t src_font, COLORREF src_color)
{
	size_t index_b = section_begin_index < section_end_index ? section_begin_index : section_end_index;
	size_t index_e = section_end_index > section_begin_index ? section_end_index : section_begin_index;
	size_t pgh_b = section_begin_pgh < section_end_pgh ? section_begin_pgh : section_end_pgh;
	size_t pgh_e = section_begin_pgh < section_end_pgh ? section_end_pgh : section_begin_pgh;

	remove_phy_section(index_b, pgh_b, index_e, pgh_e);

	font_tree.remove(index_b, index_e);
	color_tree.remove(index_b, index_e);
	if (font_tree.empty() && color_tree.empty())
	{
		font_tree.insert(0, 0, default_font_index);
		color_tree.insert(0, 0, default_color_index);
	}

	insert(index_b, pgh_b, cs, len);
	font_tree.insert(index_b, len, src_font);
	color_tree.insert(index_b, len, src_color);

	compose_doc.remove_section(pDC, index_b, pgh_b, index_e - len, pgh_e);

	compose_doc.calc_cursor(cursor, index_b + len, document.get_pgh(pgh_b), pDC);
	section.cursor_begin = cursor;
	section.cursor_end = cursor;
}

void LxDcViCtl::modify_section_font(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh, size_t src_font)
{
	size_t index_b = section_begin_index < section_end_index ? section_begin_index : section_end_index;
	size_t index_e = section_end_index > section_begin_index ? section_end_index : section_begin_index;
	font_tree.modify(index_b, index_e, src_font);
	gd_proxy.set_font_index(src_font);

	size_t pgh_b = section_begin_pgh < section_end_pgh ? section_begin_pgh : section_end_pgh;
	size_t pgh_e = section_begin_pgh < section_end_pgh ? section_end_pgh : section_begin_pgh;

	compose_doc.relayout_section(pDC, index_b, pgh_b, index_e, pgh_e);
	//重新计算cursor的左边位置
	compose_doc.calc_cursor(section.cursor_begin, section_begin_index, document.get_pgh(section_begin_pgh), pDC);
	compose_doc.calc_cursor(section.cursor_end, section_end_index, document.get_pgh(section_end_pgh), pDC);
	cursor = section.cursor_end;
}

void LxDcViCtl::modify_section_color(size_t section_begin_index, size_t section_end_index, COLORREF src_color)
{
	color_tree.modify(section_begin_index, section_end_index, src_color);
}

void LxDcViCtl::calc_font_color()
{
	if (cursor.head_of_paragraph())
	{
		size_t src_index, last_cnt;
		font_tree.get_src_index(cursor.get_index_global(), src_index, last_cnt);
		gd_proxy.set_font_index(src_index);
		color_tree.get_src_index(cursor.get_index_global(), src_index, last_cnt);
		gd_proxy.set_color(src_index);
	}
	else
	{
		gd_proxy.set_font_index(font_tree.get_src_index(cursor.get_index_global()));
		gd_proxy.set_color(color_tree.get_src_index(cursor.get_index_global()));
	}
}

void LxDcViCtl::modify_view_size(int width, int height)
{
	AdjustViewWindow(width, height);
	//size改变后，文档的offset_y保持不变，但当客户区域的高度大于等于文档总高度时，将offset_y设为0
	//同样offset_x也保持不变，当客户区域的宽度大于等于文档页面宽度时，将offset_x设为0
	if (height >= compose_doc.total_height())
		ViewWindow::GetViewWindowInstance()->offset_y = 0;
	if (width >= compose_doc.total_width())
		ViewWindow::GetViewWindowInstance()->offset_x = 0;
	else if (ViewWindow::GetViewWindowInstance()->offset_x + width > compose_doc.total_width())
	{
		ViewWindow::GetViewWindowInstance()->offset_x = compose_doc.total_width() - width;
	}
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
			phy_pgh = (*pgh_doc_it)->get_phy_paragraph();
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
			phy_pgh = (*pgh_doc_it)->get_phy_paragraph();
		}
		compose_doc.calc_cursor(cursor, cur_gbl_index_old, phy_pgh, pDC);
	}
		break;
	case VK_UP:
	{
		LxRowInDocIter row_doc_it(&compose_doc, cursor.page, cursor.paragraph, cursor.row);
		if (row_doc_it == compose_doc.row_begin()) return;
		--row_doc_it;
		compose_doc.locate(cursor, pDC, cursor.point_x, (*row_doc_it)->get_top_pos() + (*row_doc_it)->get_base_line());
	}
		break;
	case VK_DOWN:
	{
		LxRowInDocIter row_doc_it(&compose_doc, cursor.page, cursor.paragraph, cursor.row);
		++row_doc_it;
		if (row_doc_it == compose_doc.row_end()) return;
		compose_doc.locate(cursor, pDC, cursor.point_x, (*row_doc_it)->get_top_pos() + (*row_doc_it)->get_base_line());
	}
		break;
	default:
		return;
	}
	if (modify_cursor_offset())
	{
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

	font_tree.remove(cursor.get_index_global() - 1, cursor.get_index_global());
	color_tree.remove(cursor.get_index_global() - 1, cursor.get_index_global());
	if (font_tree.empty() && color_tree.empty())
	{
		font_tree.insert(0, 0, default_font_index);
		color_tree.insert(0, 0, default_color_index);
	}
	return true;
}

void LxDcViCtl::insert(TCHAR* src, size_t  count, size_t src_font, COLORREF src_color)
{
	//在cursor处执行插入操作
	Paragraph* pgh = (*(cursor.paragraph))->get_phy_paragraph();
	pgh->Insert(cursor.get_index_inner_paragraph(), src, count);

	font_tree.insert(cursor.get_index_global(), count, src_font);
	color_tree.insert(cursor.get_index_global(), count, src_color);
}

void LxDcViCtl::insert(size_t pos_global, size_t pos_pgh_index, TCHAR* src, size_t  count)
{
	auto phy_pgh_iter_b = document.begin();
	advance(phy_pgh_iter_b, pos_pgh_index);
	size_t inner_index_b = document.get_offset_inner(pos_global, pos_pgh_index);
	(*phy_pgh_iter_b)->Insert(inner_index_b, src, count);
}
//void LxDcViCtl::insert(TCHAR* src, size_t  count, size_t src_index)
//{
//}
void LxDcViCtl::remove(size_t position)
{
	if (position != 0)
		remove(position - 1, position);
}
void LxDcViCtl::remove(size_t position_begin, size_t position_end)
{
	font_tree.remove(position_begin, position_end);
	color_tree.remove(position_begin, position_end);
}
void LxDcViCtl::modify_font(size_t position_begin, size_t position_end, size_t font_src_index)
{
	font_tree.modify(position_begin, position_end, font_src_index);
}
void LxDcViCtl::modify_color(size_t position_begin, size_t position_end, size_t color_src_index)
{
	color_tree.modify(position_begin, position_end, color_src_index);
}

//partial
void LxDcViCtl::modify_layout(CDC* pDC, int count)
{
	ASSERT(count != 0);
	size_t cur_gbl_index_old = cursor.get_index_global() + count;
	Paragraph* phy_pgh = cursor.get_phy_paragraph();

	//删除时计算新的cursor
	if (count < 0)
		compose_doc.calc_cursor(cursor, cur_gbl_index_old, phy_pgh, pDC);

	LxParagraphInDocIter pgh_doc_it(&compose_doc, cursor.page, cursor.paragraph);
	pgh_doc_it = compose_doc.modify(pgh_doc_it, cursor.row, pDC);
	compose_doc.modify_index(pgh_doc_it, count);
	compose_doc.relayout(pgh_doc_it);

	//插入时计算新的cursor
	if (count > 0)
		compose_doc.calc_cursor(cursor, cur_gbl_index_old, phy_pgh, pDC);
	//删除时由于先计算的cursor的row会被清除，所以需要再次计算cursor
	if (count < 0)
		compose_doc.calc_cursor(cursor, cur_gbl_index_old, phy_pgh, pDC);

	modify_cursor_offset();
}

void LxDcViCtl::locate(CDC* pDC, int doc_x, int doc_y)
{
	render->hide_caret();
	compose_doc.locate(cursor, pDC, doc_x, doc_y);
	render->create_caret(cursor.height, cursor.height / 8);
	render->show_caret(&cursor);
}

void LxDcViCtl::locate(CDC* pDC, Paragraph* pgh, int global_index)
{
	compose_doc.calc_cursor(cursor, global_index, pgh, pDC);
}

size_t LxDcViCtl::get_current_cur_index()
{
	return cursor.get_index_global();
}

void LxDcViCtl::add_phy_paragraph(CDC* pDC, Paragraph* pgh, int index, int direction)
{
	ComposePage* _page;
	ComposeParagraph* _cpgh;
	compose_doc.locate(_page, _cpgh, index, direction);
	LxParagraphInDocIter pgh_doc_it = compose_doc.compose_phy_pagph(pgh, _page, _cpgh, direction, pDC);
	compose_doc.modify_index(pgh_doc_it, pgh->size());
	compose_doc.relayout(pgh_doc_it);
	compose_doc.calc_cursor(cursor, (*pgh_doc_it)->get_area_begin() - (*pgh_doc_it)->get_offset_inner(), pgh, pDC);
	modify_cursor_offset();
}

void LxDcViCtl::compose_splited_paragraph(CDC* pDC, size_t phy_pgh_index, size_t offset_inner, Paragraph* seprated_phy_pgh)
{
	//1.先在排版文档中找到对应的逻辑段和所在逻辑行
	page_iter page_cursor;
	paragraph_iter pagraph_cursor;
	row_iter row_cursor;
	compose_doc.locate(page_cursor, pagraph_cursor, row_cursor, phy_pgh_index, offset_inner);
	LxParagraphInDocIter pgh_doc_it(&compose_doc, page_cursor, pagraph_cursor);

	pgh_doc_it = compose_doc.modify(pgh_doc_it, row_cursor, pDC);
	compose_doc.modify_index(pgh_doc_it, 0 - seprated_phy_pgh->size());
	pgh_doc_it = compose_doc.compose_phy_pagph(seprated_phy_pgh, *(pgh_doc_it.get_page()), *pgh_doc_it, 1, pDC);
	compose_doc.modify_index(pgh_doc_it, seprated_phy_pgh->size());
	compose_doc.relayout(pgh_doc_it);
	compose_doc.calc_cursor(cursor, (*pgh_doc_it)->get_area_begin() - (*pgh_doc_it)->get_offset_inner(), seprated_phy_pgh, pDC);
	modify_cursor_offset();
}

void LxDcViCtl::compose_merged_paragraph(CDC* pDC, size_t index_para1, size_t offset_para1)
{
	page_iter page_cursor;
	paragraph_iter pagraph_cursor;
	row_iter row_cursor;
	compose_doc.locate(page_cursor, pagraph_cursor, row_cursor, index_para1, offset_para1);
	LxParagraphInDocIter pgh_doc_it(&compose_doc, page_cursor, pagraph_cursor);
	LxParagraphInDocIter to_delete = pgh_doc_it;
	++to_delete;
	compose_doc.remove_group_paragraph(to_delete);
	pgh_doc_it = compose_doc.modify(pgh_doc_it, row_cursor, pDC);
	compose_doc.relayout(pgh_doc_it);
	compose_doc.calc_cursor(cursor, (*pgh_doc_it)->get_area_begin() - (*pgh_doc_it)->get_offset_inner() + offset_para1,
		(*pgh_doc_it)->get_phy_paragraph(), pDC);
	modify_cursor_offset();
}

//full text
void LxDcViCtl::compose_complete(CDC* pDC)
{
	compose_doc.compose_complete(pDC);
}
int LxDcViCtl::modify_cursor_offset()
{
	int rc = 0;
	if (cursor.point_y + cursor.height > ViewWindow::GetViewWindowInstance()->get_bottom_pos())
	{
		ViewWindow::GetViewWindowInstance()->offset_y +=
			cursor.point_y + cursor.height - ViewWindow::GetViewWindowInstance()->get_bottom_pos();
		rc = 1;
	}
	else if (cursor.point_y < ViewWindow::GetViewWindowInstance()->get_top_pos())
	{
		ViewWindow::GetViewWindowInstance()->offset_y -=
			ViewWindow::GetViewWindowInstance()->get_top_pos() - cursor.point_y;
		rc = 1;
	}
	if (cursor.point_x < ViewWindow::GetViewWindowInstance()->offset_x)
	{
		int new_offset_x = cursor.point_x - ViewWindow::GetViewWindowInstance()->width / 6;
		ViewWindow::GetViewWindowInstance()->offset_x = new_offset_x < 0 ? 0 : new_offset_x;
		rc = 1;
	}
	else if (cursor.point_x > ViewWindow::GetViewWindowInstance()->offset_x + ViewWindow::GetViewWindowInstance()->width)
	{
		int new_offset_x = cursor.point_x - ViewWindow::GetViewWindowInstance()->width + 
			ViewWindow::GetViewWindowInstance()->width / 6;
		int offset_x_max = compose_doc.total_width() - ViewWindow::GetViewWindowInstance()->width;
		ViewWindow::GetViewWindowInstance()->offset_x = new_offset_x > offset_x_max ? offset_x_max : new_offset_x;
		rc = 1;
	}
	return rc;
}

void LxDcViCtl::draw_section(CDC* pDC, Section* _section)
{
	if (!_section->active())
		return;
	render->hide_caret();
	render->set_scroll_size_total(compose_doc.total_width(), compose_doc.total_height());
	render->set_scroll_pos(ViewWindow::GetViewWindowInstance()->offset_x, ViewWindow::GetViewWindowInstance()->offset_y);
	pDC->SetBkMode(TRANSPARENT);
	render->DrawSection(pDC, _section);
	render->create_caret(cursor.height, cursor.height / 8);
	render->show_caret(&cursor);
}
void LxDcViCtl::clear_section(CDC* pDC, Section* _section)
{
	if (!_section->active())
		return;
	render->hide_caret();
	render->set_scroll_size_total(compose_doc.total_width(), compose_doc.total_height());
	render->set_scroll_pos(ViewWindow::GetViewWindowInstance()->offset_x, ViewWindow::GetViewWindowInstance()->offset_y);
	pDC->SetBkMode(TRANSPARENT);
	render->ClearSection(pDC, _section);
	render->create_caret(cursor.height, cursor.height / 8);
	render->show_caret(&cursor);
}
void LxDcViCtl::draw_complete(CDC* pDC)
{
	render->hide_caret();
	render->set_scroll_size_total(compose_doc.total_width(), compose_doc.total_height());
	render->set_scroll_pos(ViewWindow::GetViewWindowInstance()->offset_x, ViewWindow::GetViewWindowInstance()->offset_y);
	pDC->SetBkMode(TRANSPARENT);
	render->DrawDocument(pDC, &section);
	render->create_caret(cursor.height, cursor.height / 8);
	render->show_caret(&cursor);
}

//operation of phy paragraph
Paragraph* LxDcViCtl::insert_null_phy_paragraph(int index)
{
	Paragraph* new_phy_pragh = new Paragraph();
	new_phy_pragh->SetComposeAlgom(new LxSimpleComposeAlgo());
	document.insert_paragraph(new_phy_pragh, index);
	return new_phy_pragh;
}
void LxDcViCtl::remove_phy_section(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh)
{
	auto phy_pgh_iter_b = document.begin();
	advance(phy_pgh_iter_b, section_begin_pgh);
	size_t inner_index_b = document.get_offset_inner(section_begin_index, section_begin_pgh);
	if (section_begin_pgh == section_end_pgh)
	{
		(*phy_pgh_iter_b)->Delete(inner_index_b, inner_index_b + section_end_index - section_begin_index - 1);
		return;
	}
	auto phy_pgh_iter_e = document.begin();
	advance(phy_pgh_iter_e, section_end_pgh);
	size_t inner_index_e = document.get_offset_inner(section_end_index, section_end_pgh);
	if (inner_index_b < (*phy_pgh_iter_b)->size())
		(*phy_pgh_iter_b)->Delete(inner_index_b, (*phy_pgh_iter_b)->size() - 1);
	if (inner_index_e < (*phy_pgh_iter_e)->size())
		(*phy_pgh_iter_b)->Insert(inner_index_b, (*phy_pgh_iter_e)->get_context_ptr() + inner_index_e, 
		(*phy_pgh_iter_e)->size() - inner_index_e);

	document.remove_paragraphs(section_begin_pgh + 1, section_end_pgh);
}
Paragraph* LxDcViCtl::split_phy_paragraph(size_t phy_paragraph_index, size_t offset_inner)
{
	contex_pgh_iter phy_pgh_iter = document.begin();
	advance(phy_pgh_iter, phy_paragraph_index);
	Paragraph* new_phy_pgh = new Paragraph();
	new_phy_pgh->SetComposeAlgom(new LxSimpleComposeAlgo());
	new_phy_pgh->Insert(0, (*phy_pgh_iter)->get_context_ptr() + offset_inner, (*phy_pgh_iter)->size() - offset_inner);

	(*phy_pgh_iter)->Delete(offset_inner, (*phy_pgh_iter)->size() - 1);
	phy_pgh_iter++;
	document.insert_paragraph(new_phy_pgh, phy_pgh_iter);

	return new_phy_pgh;
}
size_t LxDcViCtl::merge_phy_paragraph(size_t index_para2)
{
	ASSERT(index_para2 > 0);
	contex_pgh_iter phy_pgh_iter2 = document.begin();
	advance(phy_pgh_iter2, index_para2);
	contex_pgh_iter phy_pgh_iter1 = phy_pgh_iter2;
	--phy_pgh_iter1;
	size_t para1_size = (*phy_pgh_iter1)->size();
	if ((*phy_pgh_iter2)->size())
		(*phy_pgh_iter1)->Insert((*phy_pgh_iter1)->size(), (*phy_pgh_iter2)->get_context_ptr(), (*phy_pgh_iter2)->size());
	document.remove_paragraph(phy_pgh_iter2);

	return para1_size;
}

// user operation handler
void LxDcViCtl::usr_mouse_lbutton_down(CDC* pDC, int x, int y)
{
	LxCommand* locate_cmd = new LxCommand();
	locate_cmd->add_child_cmd(new LxLocateCmd(x, y));
	locate_cmd->set_dvctl(this);
	locate_cmd->Excute(pDC);
	lx_command_mgr.insert_cmd(locate_cmd);
	if (section.active())
	{
		clear_section(pDC, &section);
	}
	section.cursor_begin = cursor;
	section.cursor_end = cursor;
	section.trace = true;
}
void LxDcViCtl::usr_mouse_move(CDC* pDC, int x, int y)
{
	if (!section.trace) return;
	LxCommand* locate_cmd = new LxCommand();
	locate_cmd->add_child_cmd(new LxLocateCmd(x, y));
	locate_cmd->set_dvctl(this);
	locate_cmd->Excute(pDC);
	lx_command_mgr.insert_cmd(locate_cmd);
	if (section.cursor_end == cursor)
		return;
	if (section.cursor_end > section.cursor_begin)
	{
		if (cursor > section.cursor_end)
		{
			Section _section;
			_section.cursor_begin = section.cursor_end;
			_section.cursor_end = cursor;
			draw_section(pDC, &_section);
			section.cursor_end = cursor;
		}
		else if (cursor < section.cursor_begin)
		{
			clear_section(pDC, &section);
			section.cursor_end = cursor;
			draw_section(pDC, &section);
		}
		else
		{
			Section _section;
			_section.cursor_begin = cursor;
			_section.cursor_end = section.cursor_end;
			clear_section(pDC, &_section);
			section.cursor_end = cursor;
		}
	}
	else if (section.cursor_end < section.cursor_begin)
	{
		if (cursor < section.cursor_end)
		{
			Section _section;
			_section.cursor_begin = cursor;
			_section.cursor_end = section.cursor_end;
			draw_section(pDC, &_section);
			section.cursor_end = cursor;
		}
		else if (cursor > section.cursor_begin)
		{
			clear_section(pDC, &section);
			section.cursor_end = cursor;
			draw_section(pDC, &section);
		}
		else
		{
			Section _section;
			_section.cursor_begin = section.cursor_end;
			_section.cursor_end = cursor;
			clear_section(pDC, &_section);
			section.cursor_end = cursor;
		}
	}
	else
	{
		section.cursor_end = cursor;
		draw_section(pDC, &section);
	}
}
void LxDcViCtl::usr_mouse_lbutton_up(CDC* pDC, int x, int y)
{
	LxCommand* locate_cmd = new LxCommand();
	locate_cmd->add_child_cmd(new LxLocateCmd(x, y));
	locate_cmd->set_dvctl(this);
	locate_cmd->Excute(pDC);
	lx_command_mgr.insert_cmd(locate_cmd);
	section.cursor_end = cursor;
	section.trace = false;
}
void LxDcViCtl::usr_mouse_rbutton_down(CDC* pDC, int x, int y)
{

}
void LxDcViCtl::usr_mouse_rbutton_up(CDC* pDC, int x, int y)
{

}

void LxDcViCtl::usr_font_change(CDC* pDC, LOGFONT log_font)
{
	ASSERT(section.active());
	size_t src_font = SrcFontFactory::GetFontFactInstance()->insert_src_font(log_font);
	////记录section的信息
	size_t index_begin_g = section.cursor_begin.get_index_global();
	size_t index_end_g = section.cursor_end.get_index_global();
	
	LxCommand* modify_section_font_cmd = new LxCommand();
	modify_section_font_cmd->add_child_cmd(
		new LxModifyFontCmd(index_begin_g, compose_doc.current_phypgh_index(section.cursor_begin), 
		index_end_g, compose_doc.current_phypgh_index(section.cursor_end), src_font));
	modify_section_font_cmd->set_dvctl(this);
	modify_section_font_cmd->Excute(pDC);
	lx_command_mgr.insert_cmd(modify_section_font_cmd);

}
void LxDcViCtl::usr_color_change(CDC* pDC, COLORREF src_color)
{
	ASSERT(section.active());
	LxCursor* _begin = &(section.cursor_begin);
	LxCursor* _end = &(section.cursor_end);
	if (section.cursor_end < section.cursor_begin)
	{
		_begin = &(section.cursor_end);
		_end = &(section.cursor_begin);
	}

	LxCommand* modify_section_color_cmd = new LxCommand();
	modify_section_color_cmd->add_child_cmd(
		new LxModifyColorCmd(_begin->get_index_global(), _end->get_index_global(), src_color));
	modify_section_color_cmd->set_dvctl(this);
	modify_section_color_cmd->Excute(pDC);
	lx_command_mgr.insert_cmd(modify_section_color_cmd);
	//此处drawcomplete可以改为部分绘制
}
void LxDcViCtl::usr_insert(CDC* pDC, TCHAR* cs, int len, size_t src_font, COLORREF src_color)
{
	if (!section.active())				//选择区域无效
	{
		LxCommand* insert_cmd = new LxCommand();
		insert_cmd->add_child_cmd(new LxInsertCmd(cs, len, src_font, src_color));
		insert_cmd->set_dvctl(this);
		insert_cmd->Excute(pDC);
		lx_command_mgr.insert_cmd(insert_cmd);
	}
	else                                       //选择区域有效
	{
		LxCommand* section_replace_cmd = new LxCommand();
		section_replace_cmd->add_child_cmd(
			new LxSectionReplaceCmd(section.cursor_begin.get_index_global(), compose_doc.current_phypgh_index(section.cursor_begin),
			section.cursor_end.get_index_global(), compose_doc.current_phypgh_index(section.cursor_end), cs, len, src_font, src_color));
		section_replace_cmd->set_dvctl(this);
		section_replace_cmd->Excute(pDC);
		lx_command_mgr.insert_cmd(section_replace_cmd);
	}
}
void LxDcViCtl::usr_wrap(CDC* pDC)
{
	if (!section.active())				//选择区域无效
	{
		if (cursor.tail_of_paragraph() || cursor.head_of_paragraph())
		{
			//now create new phy paragraph
			LxCommand* newphypragh_cmd = new LxCommand();
			if (cursor.tail_of_paragraph())		//在之后新建一个物理段
				newphypragh_cmd->add_child_cmd(new LxInsertPhyParagraphCmd(compose_doc.current_phypgh_index(cursor), 1));
			else			//在之前新建一个物理段
				newphypragh_cmd->add_child_cmd(new LxInsertPhyParagraphCmd(compose_doc.current_phypgh_index(cursor), 0));
			newphypragh_cmd->set_dvctl(this);
			newphypragh_cmd->Excute(pDC);
			lx_command_mgr.insert_cmd(newphypragh_cmd);
		}
		else
		{
			//分割当前物理段
			LxCommand* split_phypragh_cmd = new LxCommand();
			split_phypragh_cmd->add_child_cmd(new LxSplitCmd(compose_doc.current_phypgh_index(cursor),
				cursor.get_index_inner_paragraph()));
			split_phypragh_cmd->set_dvctl(this);
			split_phypragh_cmd->Excute(pDC);
			lx_command_mgr.insert_cmd(split_phypragh_cmd);
		}
	}
	else                                       //选择区域有效
	{
		LxCommand* section_wrap_cmd = new LxCommand();
		section_wrap_cmd->add_child_cmd(
			new LxSectionWrapCmd(section.cursor_begin.get_index_global(), compose_doc.current_phypgh_index(section.cursor_begin),
			section.cursor_end.get_index_global(), compose_doc.current_phypgh_index(section.cursor_end)));
		section_wrap_cmd->set_dvctl(this);
		section_wrap_cmd->Excute(pDC);
		lx_command_mgr.insert_cmd(section_wrap_cmd);
	}
}
void LxDcViCtl::usr_backspace(CDC* pDC)
{
	if (!section.active())				//选择区域无效
	{
		if (cursor.head_of_paragraph())		//物理段首
		{
			//该段为第一个物理段 什么也不做
			if (compose_doc.first_phy_paragraph(cursor))
			{
				draw_complete(pDC);
				return;
			}
			else
			{
				//当前物理段为空
				//if ((*cursor.paragraph)->get_phy_paragraph()->empty())
				//{
				//	//删除当前物理段
				//}
				//else
				{
					//和前一个物理段合并
					LxCommand* merge_phypragh_cmd = new LxCommand();
					merge_phypragh_cmd->add_child_cmd(new LxMergeCmd(compose_doc.current_phypgh_index(cursor)));
					merge_phypragh_cmd->set_dvctl(this);
					merge_phypragh_cmd->Excute(pDC);
					lx_command_mgr.insert_cmd(merge_phypragh_cmd);
				}
			}
		}
		else
		{
			LxCommand* backspace_cmd = new LxCommand();
			backspace_cmd->add_child_cmd(new LxSingleRemoveCmd());
			backspace_cmd->set_dvctl(this);
			backspace_cmd->Excute(pDC);
			lx_command_mgr.insert_cmd(backspace_cmd);
		}
	}
	else                                       //选择区域有效
	{
		LxCommand* section_remove_cmd = new LxCommand();
		section_remove_cmd->add_child_cmd(
			new LxSectionRemoveCmd(section.cursor_begin.get_index_global(), compose_doc.current_phypgh_index(section.cursor_begin),
			section.cursor_end.get_index_global(), compose_doc.current_phypgh_index(section.cursor_end)));
		section_remove_cmd->set_dvctl(this);
		section_remove_cmd->Excute(pDC);
		lx_command_mgr.insert_cmd(section_remove_cmd);
	}
}
void LxDcViCtl::usr_move_cursor(CDC* pDC, unsigned int direction)
{
	LxCommand* move_cmd = new LxCommand();
	move_cmd->add_child_cmd(new LxMoveCmd(direction));
	move_cmd->set_dvctl(this);
	move_cmd->Excute(pDC);
	lx_command_mgr.insert_cmd(move_cmd);
	if (section.active())
	{
		clear_section(pDC, &section);
		section.cursor_begin = cursor;
		section.cursor_end = cursor;
	}
}