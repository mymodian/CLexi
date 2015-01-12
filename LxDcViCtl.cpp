#include "stdafx.h"
#include "LxDcViCtl.h"
#include "LxSrcFontFactory.h"

LxDcViCtl::LxDcViCtl() {}
LxDcViCtl::~LxDcViCtl() {}

void LxDcViCtl::init(CDC* pDC)
{
	CFont* m_Font = new CFont;
	m_Font->CreateFont(-16, 0, 0, 0, 100, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, "Arial");
	LOGFONT logfont;
	m_Font->GetLogFont(&logfont);
	size_t font_index = SrcFontFactory::GetFontFactInstance()->insert_src_font(logfont);
	font_tree.insert(0, 0, font_index);
	delete m_Font;

	Paragraph* paragraph = new Paragraph();
	//设置默认的排版算法，对象应从排版算法管理结构中获取
	paragraph->SetComposeAlgom(new LxSimpleComposeAlgo());
	document.add_paragraph(paragraph);
	compose_doc.AttachColorInfo(&color_tree);
	compose_doc.AttachFontInfo(&font_tree);
	compose_doc.AttachPhyDoc(&document);

	compose_doc.compose_complete(pDC);

	cursor.point_x = ViewWindow::GetViewWindowInstance()->border_width_left + LxPaper::left_margin;
	cursor.point_y = ViewWindow::GetViewWindowInstance()->border_height + LxPaper::top_margin;
	cursor.page = compose_doc.begin();
	cursor.paragraph = (*cursor.page)->begin();
	cursor.row = (*cursor.paragraph)->begin();
	cursor.index_inner = 0;
	cursor.width_used = 0;

	gd_proxy.init();
	render = new LxBorderRender(new LxContexRender(&compose_doc, &gd_proxy));
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

//full text
void LxDcViCtl::compose_complete(CDC* pDC)
{
	compose_doc.compose_complete(pDC);
}
void LxDcViCtl::draw_complete(CDC* pDC)
{
	render->DrawDocument(pDC);
	render->hide_caret();
	render->create_caret(20, 4);
	render->show_caret(&cursor);
}