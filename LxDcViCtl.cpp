#include "stdafx.h"
#include "LxDcViCtl.h"

LxDcViCtl::LxDcViCtl()
{
	ComposePage* first_page=new ComposePage();
	ComposeParagraph* first_paragraph=new ComposeParagraph();
	ComposeRow* first_row=new ComposeRow();
	first_row->set_area(0, 0);
	first_paragraph->set_area(0, 0);
	first_page->set_area(0, 0);

	compose_doc.add_page(first_page);
	first_page->add_paragraph(first_paragraph);
	first_paragraph->add_row(first_row);
	
	cursor.point_x = ViewWindow::GetViewWindowInstance()->border_width_left + LxPaper::left_margin;
	cursor.point_y = ViewWindow::GetViewWindowInstance()->border_height + LxPaper::top_margin;
	cursor.page = compose_doc.begin();
	cursor.paragraph = first_page->begin();
	cursor.row = first_paragraph->begin();
	cursor.index_inner = 0;
	cursor.width_used = 0;

	Paragraph* paragraph = new Paragraph();
	//设置默认的排版算法，对象应从排版算法管理结构中获取
	paragraph->SetComposeAlgom(new LxSimpleComposeAlgo());
	document.add_paragraph(paragraph);
	first_paragraph->set_phy_paragraph(paragraph);
	compose_doc.AttachColorInfo(&color_tree);
	compose_doc.AttachFontInfo(&font_tree);
	compose_doc.AttachPhyDoc(&document);
}
LxDcViCtl::~LxDcViCtl()
{
}
void LxDcViCtl::insert(size_t position, size_t  count)
{
}
void LxDcViCtl::insert(size_t position, size_t  count, size_t src_index)
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