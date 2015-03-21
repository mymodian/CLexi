#include "stdafx.h"
#include "LxComposeAlgom.h"
#include "LxSrcFontFactory.h"
#include "LxPaper.h"

LxSimpleComposeAlgo::LxSimpleComposeAlgo()
{

}
LxSimpleComposeAlgo::~LxSimpleComposeAlgo()
{

}
void LxSimpleComposeAlgo::compose()
{

}
void LxSimpleComposeAlgo::layout()
{

}
ComposeParagraph* LxSimpleComposeAlgo::compose(int offset_y, size_t pagra_index_begin, Paragraph* pagraph, TreeBase* font_tree, CDC* pDC)
{
	ComposeParagraph* com_pagraph = new ComposeParagraph();
	com_pagraph->set_phy_paragraph(pagraph);
	com_pagraph->set_area(pagra_index_begin, pagra_index_begin + pagraph->size() - 1);
	int max_height = 0;
	int base_pos = 0;
	int external_leading = 0;
	ComposeRow* com_row = new ComposeRow();
	com_pagraph->add_row(com_row);
	com_row->set_line_space(LxPaper::pixel_width - LxPaper::left_margin - LxPaper::right_margin);
	com_row->set_top_pos(offset_y);
	size_t row_pos_begin = pagra_index_begin;
	for (size_t pos = pagra_index_begin; pos < pagra_index_begin + pagraph->size();)
	{
		size_t src_index, count;
		font_tree->get_src_index(pos, src_index, count);
		CFont* font = SrcFontFactory::GetFontFactInstance()->get_src_font(src_index);
		pDC->SelectObject(font);
		TEXTMETRIC text_metric;
		GetTextMetrics(*pDC, &text_metric);
		if (text_metric.tmHeight > max_height)
		{
			max_height = text_metric.tmHeight; com_row->set_height(max_height);
			base_pos = text_metric.tmAscent; com_row->set_base_line(base_pos);
			external_leading = text_metric.tmExternalLeading; com_row->set_external_leading(external_leading);
		}
		int desired_char_cnt = com_row->get_line_space() / text_metric.tmAveCharWidth;
		int measure_cnt = min(desired_char_cnt, count);
		int char_length = pDC->GetTextExtent(pagraph->get_context_ptr() + pos - pagra_index_begin, measure_cnt).cx;
		if (char_length > com_row->get_line_space())
		{
			while (char_length > com_row->get_line_space())
			{
				char_length -= pDC->GetTextExtent(pagraph->get_context_ptr() + pos - pagra_index_begin + (--measure_cnt), 1).cx;
			}
			com_row->set_line_space(com_row->get_line_space() - char_length);
			com_row->set_area(row_pos_begin, pos + measure_cnt - 1);
			int next_pos_y = com_row->get_top_pos() + max_height + external_leading;
			com_row = new ComposeRow();
			com_pagraph->add_row(com_row);
			com_row->set_line_space(LxPaper::pixel_width - LxPaper::left_margin - LxPaper::right_margin);
			com_row->set_top_pos(next_pos_y);
			row_pos_begin = pos + measure_cnt;
			max_height = 0;
			base_pos = 0;
			external_leading = 0;
		}
		else
		{
			com_row->set_line_space(com_row->get_line_space() - char_length);
		}
		pos += measure_cnt;
	}
	com_pagraph->set_pos(offset_y, com_row->get_top_pos() + com_row->get_height());
	return com_pagraph;
}

void LxSimpleComposeAlgo::compose(LxCursor cursor)
{
	row_iter row_it = cursor.row;
	page_iter page_it = cursor.page;
	paragraph_iter pagraph_it = cursor.paragraph;


}

/*
	index_begin		该行的起始全局索引
	index_inner		该行起始位置在物理段内的偏移
	*/
void LxSimpleComposeAlgo::compose(ComposeRow* row_to_compose, Paragraph* pagraph, size_t& index_begin, size_t& index_inner, TreeBase* font_tree, CDC* pDC)
{
	row_to_compose->set_line_space(LxPaper::pixel_width - LxPaper::left_margin - LxPaper::right_margin);

	int max_height = 0;
	int base_pos = 0;
	int external_leading = 0;
	int index_inner_orig = index_inner;
	for (; index_inner < pagraph->size();)
	{
		size_t src_index, count;
		font_tree->get_src_index(index_begin + index_inner - index_inner_orig, src_index, count);
		CFont* font = SrcFontFactory::GetFontFactInstance()->get_src_font(src_index);
		pDC->SelectObject(font);
		TEXTMETRIC text_metric;
		GetTextMetrics(*pDC, &text_metric);
		int desired_char_cnt = row_to_compose->get_line_space() / text_metric.tmAveCharWidth;
		desired_char_cnt = desired_char_cnt ? desired_char_cnt : 1;
		int measure_cnt = min(desired_char_cnt, min(count, pagraph->size() - index_inner));
		int char_length = pDC->GetTextExtent(pagraph->get_context_ptr() + index_inner, measure_cnt).cx;
		if (char_length > row_to_compose->get_line_space())
		{
			while (char_length > row_to_compose->get_line_space())
			{
				char_length -= pDC->GetTextExtent(pagraph->get_context_ptr() + index_inner + (--measure_cnt), 1).cx;
			}
			index_inner += measure_cnt;
			row_to_compose->set_line_space(row_to_compose->get_line_space() - char_length);
			row_to_compose->set_area(index_begin, index_begin + index_inner - index_inner_orig - 1);
			row_to_compose->set_words_space(row_to_compose->get_line_space() / (row_to_compose->size() - 1));
			index_begin += index_inner - index_inner_orig;
			if (measure_cnt > 0 && text_metric.tmHeight > max_height)
			{
				max_height = text_metric.tmHeight; row_to_compose->set_height(max_height);
				base_pos = text_metric.tmAscent; row_to_compose->set_base_line(base_pos);
				external_leading = text_metric.tmExternalLeading; row_to_compose->set_external_leading(external_leading);
			}
			return;
		}
		else
		{
			row_to_compose->set_line_space(row_to_compose->get_line_space() - char_length);
			if (text_metric.tmHeight > max_height && measure_cnt > 0)
			{
				max_height = text_metric.tmHeight; row_to_compose->set_height(max_height);
				base_pos = text_metric.tmAscent; row_to_compose->set_base_line(base_pos);
				external_leading = text_metric.tmExternalLeading; row_to_compose->set_external_leading(external_leading);
			}
		}
		index_inner += measure_cnt;
	}
	if (index_inner == pagraph->size())
	{
		row_to_compose->set_area(index_begin, index_begin + index_inner - index_inner_orig - 1);
		row_to_compose->set_words_space(0);
		index_begin += index_inner - index_inner_orig;
	}
}