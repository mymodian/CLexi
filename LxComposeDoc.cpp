#include "stdafx.h"
#include "LxPaper.h"
#include "LxViewWindow.h"
#include "LxComposeDoc.h"
#include "LxComposeAlgom.h"
#include "LxSrcFontFactory.h"
#include "LxCursor.h"
#include <iostream>
using namespace std;

ComposeDoc::~ComposeDoc()
{
	for (auto page : pages)
		delete page;
	pages.clear();
}
void ComposeDoc::add_page(ComposePage* page)
{
	pages.push_back(page);
}
void ComposeDoc::add_page(ComposePage* page, int index)
{
	auto it = pages.begin();
	advance(it, index);
	pages.push_back(page);
}

LxParagraphInDocIter ComposeDoc::pargraph_begin()
{
	return LxParagraphInDocIter(this, begin(), (*begin())->begin());
}
LxParagraphInDocIter ComposeDoc::pargraph_end()
{
	return LxParagraphInDocIter(this, end(), (*(--end()))->end());
}

void ComposeDoc::calc_cursor(LxCursor& cursor, size_t cur_gbl_index, Paragraph* phy_pgh, CDC* pDC)
{
	for (page_iter page = begin(); page != end(); page++)
	{
		if ((*page)->get_area_end() + 1 >= cur_gbl_index)
		{
			for (paragraph_iter paragraph = (*page)->begin(); paragraph != (*page)->end(); paragraph++)
			{
				if ((*paragraph)->get_area_end() + 1 >= cur_gbl_index)
				{
					if ((*paragraph)->get_phy_paragraph() == phy_pgh)
					{
						for (row_iter row = (*paragraph)->begin(); row != (*paragraph)->end(); row++)
						{
							if ((*row)->get_area_end() + 1 >= cur_gbl_index)
							{
								cursor.page = page;
								cursor.paragraph = paragraph;
								cursor.row = row;
								cursor.index_inner = cur_gbl_index - (*row)->get_area_begin();
								//cursor在row的第index_inner个字符后
								int x = LxPaper::left_margin;
								int y;
								size_t index = (*row)->get_area_begin();
								size_t font_index, same_font_cnt;
								for (int i = 0; i < cursor.index_inner; )
								{
									font_tree->get_src_index(index, font_index, same_font_cnt);
									size_t count = min(cursor.index_inner - i, same_font_cnt);
									CFont* font = SrcFontFactory::GetFontFactInstance()->get_src_font(font_index);
									pDC->SelectObject(font);
									TEXTMETRIC trx;
									pDC->GetTextMetrics(&trx);
									cursor.height = trx.tmHeight;
									y = (*row)->get_top_pos() + (*row)->get_base_line() - trx.tmAscent;
									CSize size;
									for (int j = 0; j < count; j++)
									{
										size = pDC->GetTextExtent(phy_pgh->get_context_ptr() + index - (*paragraph)->get_area_begin(), 1);
										index++;
										x += size.cx + (*row)->get_words_space();
									}
									i += count;
								}
								cursor.point_x = x;
								cursor.point_y = y;
								return;
							}
						}
					}
				}
			}
		}
	}
}

void ComposeDoc::Draw(CDC* pDC)
{
	for (auto page_ : pages)
	{
		if (page_->get_top_pos() + LxPaper::pixel_height <= ViewWindow::GetViewWindowInstance()->offset_y)
			continue;
		if (page_->get_top_pos() >= ViewWindow::GetViewWindowInstance()->get_bottom_pos())
			break;
		page_->FlushOwnArea(pDC);
		page_->Draw(pDC, font_tree, color_tree);
	}
}

void ComposeDoc::clear()
{
	for (auto page : pages)
		delete page;
	pages.clear();
}

void ComposeDoc::compose_complete(CDC* pDC)
{
	clear();
	size_t index_global = 0;
	int y_offset = ViewWindow::GetViewWindowInstance()->border_height + LxPaper::top_margin;
	ComposePage* page = new ComposePage();
	page->set_area(0, -1);
	page->set_top_pos(0 + ViewWindow::GetViewWindowInstance()->border_height);
	add_page(page);

	for (contex_pgh_iter phy_pagph = phy_document->begin(); phy_pagph != phy_document->end(); phy_pagph++)
	{
		size_t index_inner = 0;
		ComposeParagraph* paragraph = new ComposeParagraph();
		paragraph->set_area(index_global, index_global - 1);
		paragraph->set_offset_inner(index_inner);
		paragraph->set_phy_paragraph(*phy_pagph);
		paragraph->set_pos(y_offset, 0);

		if ((*phy_pagph)->size() == 0)
		{
			ComposeRow* row = new ComposeRow();
			row->set_area(index_global, index_global - 1);
			paragraph->set_area(index_global, index_global - 1);

			CFont* font = SrcFontFactory::GetFontFactInstance()->get_src_font(font_tree->get_src_index(index_global));
			pDC->SelectObject(font);
			TEXTMETRIC text_metric;
			GetTextMetrics(*pDC, &text_metric);

			row->set_height(text_metric.tmHeight);
			row->set_base_line(text_metric.tmAscent);
			row->set_external_leading(text_metric.tmExternalLeading);
			row->set_line_space(LxPaper::pixel_width - LxPaper::left_margin - LxPaper::right_margin);

			paragraph->add_row(row);


			if (y_offset + row->get_height() > page->get_top_pos() + LxPaper::pixel_height - LxPaper::bottom_margin)
			{
				page->set_area(page->get_area_begin(), index_global - 1);
				int new_page_pos = page->get_top_pos() + LxPaper::pixel_height +
					ViewWindow::GetViewWindowInstance()->border_height;
				page = new ComposePage();
				page->set_area(index_global, index_global - 1);
				page->set_top_pos(new_page_pos);
				add_page(page);
				y_offset = page->get_top_pos() + LxPaper::top_margin;
			}
			row->set_top_pos(y_offset);
			paragraph->set_parent_page(page);
			paragraph->set_pos(y_offset, row->get_bottom_pos());
			page->add_paragraph(paragraph);
			y_offset = row->get_next_allowed_pos();
		}
		else
		{
			for (; index_inner < (*phy_pagph)->size();)
			{
				ComposeRow* row_to_compose = new ComposeRow();
				(*phy_pagph)->GetComposeAlgom()->compose(row_to_compose, *phy_pagph, index_global, index_inner, font_tree, pDC);
				if (y_offset + row_to_compose->get_height() > page->get_top_pos() + LxPaper::pixel_height - LxPaper::bottom_margin)
				{
					//当前页无法容纳新行
					//1.修改当前页的属性
					page->set_area(page->get_area_begin(), index_global - row_to_compose->size() - 1);

					//当前段已有内容，保存
					if (paragraph->row_size() != 0)
					{
						paragraph->set_area(paragraph->get_area_begin(), index_global - row_to_compose->size() - 1);
						paragraph->set_pos(paragraph->get_top_pos(), (*(--paragraph->end()))->get_bottom_pos());
						paragraph->set_parent_page(page);
						page->add_paragraph(paragraph);
						paragraph = new ComposeParagraph();
					}

					int old_top_pos = page->get_top_pos();
					page = new ComposePage();
					page->set_area(index_global - row_to_compose->size(), -1);
					page->set_top_pos(old_top_pos + LxPaper::pixel_height + ViewWindow::GetViewWindowInstance()->border_height);
					add_page(page);
					y_offset = page->get_top_pos() + LxPaper::top_margin;

					paragraph->set_phy_paragraph(*phy_pagph);
					paragraph->set_area(index_global - row_to_compose->size(), -1);
					paragraph->set_offset_inner(index_inner - row_to_compose->size());
					paragraph->set_pos(y_offset, -1);
				}
				row_to_compose->set_top_pos(y_offset);
				paragraph->add_row(row_to_compose);
				y_offset = row_to_compose->get_next_allowed_pos();
			}
			paragraph->set_area(paragraph->get_area_begin(), index_global - 1);
			paragraph->set_pos(paragraph->get_top_pos(), (*(--paragraph->end()))->get_bottom_pos());
			paragraph->set_parent_page(page);
			page->add_paragraph(paragraph);
		}
	}
}

void ComposeDoc::modify_index(LxParagraphInDocIter pagraph_iter, int count)
{
	paragraph_iter pgraph_cusr = pagraph_iter.get_paragraph();
	pgraph_cusr++;
	page_iter page_cusr = pagraph_iter.get_page();
	(*page_cusr)->set_area((*page_cusr)->get_area_begin(), (*page_cusr)->get_area_end() + count);

	for (;;)
	{
		for (; pgraph_cusr != (*page_cusr)->end(); pgraph_cusr++)
		{
			(*pgraph_cusr)->set_area((*pgraph_cusr)->get_area_begin() + count, (*pgraph_cusr)->get_area_end() + count);
			for (row_iter row = (*pgraph_cusr)->begin(); row != (*pgraph_cusr)->end(); row++)
				(*row)->set_area((*row)->get_area_begin() + count, (*row)->get_area_end() + count);
		}
		page_cusr++;
		if (page_cusr == end())
			break;
		(*page_cusr)->set_area((*page_cusr)->get_area_begin() + count, (*page_cusr)->get_area_end() + count);
		pgraph_cusr = (*page_cusr)->begin();
	}
}

LxParagraphInDocIter ComposeDoc::modify(LxParagraphInDocIter pagraph_iter, row_iter pos, CDC* pDC)
{
	paragraph_iter pgraph_cusr = pagraph_iter.get_paragraph();
	page_iter page_cusr = pagraph_iter.get_page();
	//先删除当前物理段对应的所有逻辑段
	/*
	每行添加一个字间距
	每次command操作需要重新计算Cursor
	该行和段之后的行删除
	*/
	//注意迭代器失效的情况

	//		1.删除当前逻辑段pos和pos之后的逻辑行,先记住row的一些属性（offset_y, index）
	int row_max_height_ = (*pos)->get_height();
	int row_baseline_offset_inner_ = (*pos)->get_base_line();
	int row_ri_external_leading_ = (*pos)->get_external_leading();

	int top_offset_current = (*pos)->get_top_pos();
	size_t index_begin = (*pos)->get_area_begin();
	for (row_iter row_it = pos; row_it != (*pgraph_cusr)->end();)
	{
		row_iter temp_it = row_it;
		row_it++;
		delete *temp_it;
		(*pgraph_cusr)->remove_row(temp_it);
	}

	//		2.删除当前物理段对应的所有逻辑段
	for (LxParagraphInDocIter pgph_it = pagraph_iter; pgph_it != pargraph_end();)
	{
		pgph_it++;
		if (pgph_it == pargraph_end()) break;
		LxParagraphInDocIter temp_it = pgph_it;
		pgph_it++;
		//如果 temp_it 所指逻辑段为部分段则删除，否则退出
		if ((*temp_it)->get_offset_inner() == 0) break;
		//删除该逻辑段
		ComposeParagraph* paragraph_to_delete = *temp_it;
		(*(temp_it.get_page()))->remove_paragraph(temp_it.get_paragraph());
		delete paragraph_to_delete;
	}

	//		3.重排当前物理段
	ComposeParagraph* composed_paragraph = *pgraph_cusr;
	int y_offset = top_offset_current;
	size_t index_global = index_begin;
	size_t index_inner = index_begin - composed_paragraph->get_area_begin() + composed_paragraph->get_offset_inner();
	Paragraph* paragraph = composed_paragraph->get_phy_paragraph();
	LxComposeAlgom* compose_algom = paragraph->GetComposeAlgom();
	if (paragraph->size() == 0)
	{
		ComposeRow* row = new ComposeRow();
		composed_paragraph->add_row(row);
		row->set_area(index_global, index_global - 1);
		row->set_base_line(row_baseline_offset_inner_);
		row->set_external_leading(row_ri_external_leading_);
		row->set_height(row_max_height_);
		row->set_top_pos(y_offset);
		row->set_line_space(LxPaper::pixel_width - LxPaper::left_margin - LxPaper::right_margin);
		composed_paragraph->set_area(index_global, index_global - 1);
		composed_paragraph->set_pos(y_offset, row->get_bottom_pos());
		y_offset = row->get_next_allowed_pos();
		LxParagraphInDocIter last_PD(this, page_cusr, pgraph_cusr);
		return last_PD;
	}
	if (index_inner == paragraph->size())
	{
		if (composed_paragraph->row_size() != 0)
		{
			composed_paragraph->set_area(composed_paragraph->get_area_begin(), index_begin - 1);
			composed_paragraph->set_pos(composed_paragraph->get_top_pos(), (*(--composed_paragraph->end()))->get_bottom_pos());
			LxParagraphInDocIter last_PD(this, page_cusr, pgraph_cusr);
			return last_PD;
		}
		delete *pgraph_cusr;
		(*page_cusr)->remove_paragraph(pgraph_cusr);
		page_cusr--;
		pgraph_cusr = --((*page_cusr)->end());
		LxParagraphInDocIter last_PD(this, page_cusr, pgraph_cusr);
		return last_PD;
	}
	for (; index_inner < paragraph->size();)
	{
		ComposeRow* row_to_compose = new ComposeRow();
		compose_algom->compose(row_to_compose, paragraph, index_global, index_inner, font_tree, pDC);
		if (y_offset + row_to_compose->get_height() >(*page_cusr)->get_top_pos() + LxPaper::pixel_height - LxPaper::bottom_margin)
		{
			//新加入行后会导致超越当前的页范围
			//1.1如果当前页为最后一页，新建一个页
			page_iter next_page = page_cusr;
			next_page++;
			if (next_page == end())
			{
				ComposePage* new_page = new ComposePage();
				add_page(new_page);
				new_page->set_top_pos((*page_cusr)->get_top_pos() + LxPaper::pixel_height +
					ViewWindow::GetViewWindowInstance()->border_height);
			}
			//1.2将之后的逻辑段都移入下一页(++page_cusr)
			page_iter prev_page = page_cusr;
			page_cusr++;

			for (paragraph_iter reverse_it = --((*prev_page)->end()); reverse_it != pgraph_cusr;)
			{
				paragraph_iter deleted = reverse_it;
				reverse_it--;
				(*deleted)->set_parent_page(*page_cusr);
				(*page_cusr)->add_paragraph(*deleted, 0);
				(*prev_page)->remove_paragraph(deleted);
			}
			//1.3如果当前为物理段对应的第一个行，则删除该逻辑段
			if (composed_paragraph->row_size() == 0)
			{
				delete *pgraph_cusr;
				(*prev_page)->remove_paragraph(pgraph_cusr);
			}
			else
			{
				composed_paragraph->set_area(composed_paragraph->get_area_begin(), index_global - row_to_compose->size() - 1);
				composed_paragraph->set_pos(composed_paragraph->get_top_pos(), (*(--composed_paragraph->end()))->get_bottom_pos());
			}
			//1.4先更新当前页和逻辑段的属性
			(*prev_page)->set_area((*prev_page)->get_area_begin(), index_global - row_to_compose->size() - 1);

			//1.5开始跨页，先新建一个逻辑段并设置部分属性
			composed_paragraph = new ComposeParagraph();
			composed_paragraph->set_offset_inner(index_inner - row_to_compose->size());
			composed_paragraph->set_area(index_global - row_to_compose->size(), 0);
			composed_paragraph->set_phy_paragraph(paragraph);

			//1.6将新建的逻辑段加入新页
			(*page_cusr)->add_paragraph(composed_paragraph, 0);
			composed_paragraph->set_parent_page(*page_cusr);
			pgraph_cusr = (*page_cusr)->begin();
			//1.7修改下一页相应属性
			(*page_cusr)->set_area(composed_paragraph->get_area_begin(), (*page_cusr)->get_area_end());
			//1.8将新行加入到新段中
			y_offset = (*page_cusr)->get_top_pos() + LxPaper::top_margin;
			composed_paragraph->set_pos(y_offset, composed_paragraph->get_bottom_pos());
			row_to_compose->set_top_pos(y_offset);
			composed_paragraph->add_row(row_to_compose);
			y_offset += row_to_compose->get_height() + row_to_compose->get_external_leading();
		}
		else
		{
			row_to_compose->set_top_pos(y_offset);
			composed_paragraph->add_row(row_to_compose);
			y_offset += row_to_compose->get_height() + row_to_compose->get_external_leading();
		}
	}
	//最后更新段的末尾属性
	composed_paragraph->set_area(composed_paragraph->get_area_begin(), index_global - 1);
	composed_paragraph->set_pos(composed_paragraph->get_top_pos(), (*(--composed_paragraph->end()))->get_bottom_pos());
	LxParagraphInDocIter last_PD(this, page_cusr, pgraph_cusr);
	return last_PD;
}

paragraph_iter ComposeDoc::do_logic_combine(ComposePage* page, paragraph_iter paragraph_it)
{
	/*if( paragraph_it == page->begin() )
	return;
	if( (*paragraph_it)->get_offset_inner() == 0 )
	return;*/
	paragraph_iter prev_pragraph = paragraph_it;
	prev_pragraph--;

	(*prev_pragraph)->set_area((*prev_pragraph)->get_area_begin(), (*paragraph_it)->get_area_end());
	(*prev_pragraph)->set_pos((*prev_pragraph)->get_top_pos(), (*paragraph_it)->get_bottom_pos());
	for (row_iter row_it = (*paragraph_it)->begin(); row_it != (*paragraph_it)->end();)
	{
		row_iter to_move = row_it;
		row_it++;
		(*prev_pragraph)->add_row(*to_move);
		(*paragraph_it)->remove_row(to_move);
	}
	ComposeParagraph* to_delete = *paragraph_it;
	page->remove_paragraph(paragraph_it);
	delete to_delete;

	return prev_pragraph;
}

//pagraph_iter为指向modify所生成的最后一个段
void ComposeDoc::relayout(LxParagraphInDocIter pagraph_iter)
{
	page_iter page_cusr = pagraph_iter.get_page();
	page_iter container_page = page_cusr;
	paragraph_iter pgraph_cusr = pagraph_iter.get_paragraph();
	int y_offset = (*(--(*pgraph_cusr)->end()))->get_next_allowed_pos();
	pgraph_cusr++;

	for (;;)
	{
		for (; pgraph_cusr != (*page_cusr)->end();)
		{
			if ((*pgraph_cusr)->get_top_pos() == y_offset)		//如果段的原始位置不变，则调整结束
				return;
			//1.当前页还能容下整个段
			if ((*pgraph_cusr)->get_height() <= (*container_page)->get_top_pos() + LxPaper::pixel_height -
				LxPaper::bottom_margin - y_offset)
			{
				//平移该段
				int offset_ = y_offset - (*pgraph_cusr)->get_top_pos();
				(*pgraph_cusr)->set_pos(y_offset, y_offset + (*pgraph_cusr)->get_height());
				//平移该段的每一行
				for (row_iter row_it = (*pgraph_cusr)->begin(); row_it != (*pgraph_cusr)->end(); row_it++)
				{
					(*row_it)->set_top_pos((*row_it)->get_top_pos() + offset_);
					y_offset = (*row_it)->get_next_allowed_pos();
				}

				paragraph_iter to_check_combine = pgraph_cusr;
				if (page_cusr != container_page)
				{
					(*container_page)->add_paragraph(*pgraph_cusr);
					(*pgraph_cusr)->set_parent_page(*container_page);
					(*page_cusr)->remove_paragraph(pgraph_cusr);
					pgraph_cusr = (*page_cusr)->begin();
					to_check_combine = --((*container_page)->end());
				}
				else
					pgraph_cusr++;
				//检查是否需要将该段与上一个段融合
				if ((*to_check_combine)->get_offset_inner() != 0 && to_check_combine != (*container_page)->begin())
					do_logic_combine(*container_page, to_check_combine);
			}
			else
			{
				//1.3如果当前页为最后一页，新建一个页
				page_iter next_page = container_page;
				next_page++;
				if (next_page == end())
				{
					ComposePage* new_page = new ComposePage();
					add_page(new_page);
					new_page->set_top_pos((*container_page)->get_top_pos() + LxPaper::pixel_height +
						ViewWindow::GetViewWindowInstance()->border_height);
					next_page--;
				}
				//2.当前页无法容纳该段的任意行
				if ((*(*pgraph_cusr)->begin())->get_height() > (*container_page)->get_top_pos() + LxPaper::pixel_height -
					LxPaper::bottom_margin - y_offset)
				{
					//完成当前页的属性修改
					(*container_page)->set_area((*container_page)->get_area_begin(), (*pgraph_cusr)->get_area_begin() - 1);
					//修改下一页部分属性
					(*next_page)->set_area((*pgraph_cusr)->get_area_begin(), (*next_page)->get_area_end());
					//修改当前 y_offset
					y_offset = (*next_page)->get_top_pos() + LxPaper::top_margin;
					//如果该段在排版页内，则将该段和之后同页的段从当前页移到下一页并修改相应的属性
					if (page_cusr == container_page)
					{
						for (paragraph_iter reverse_it = --((*page_cusr)->end());;)
						{
							paragraph_iter to_move = reverse_it;
							bool bEnd = true;
							if (to_move != pgraph_cusr)
							{
								reverse_it--;
								bEnd = false;
							}
							(*to_move)->set_parent_page(*next_page);
							(*next_page)->add_paragraph(*to_move, 0);
							(*page_cusr)->remove_paragraph(to_move);
							if (bEnd) break;
						}
						container_page = next_page;
						page_cusr = next_page;
						pgraph_cusr = (*page_cusr)->begin();
					}
					else
						container_page = next_page;
				}
				//3.当前页能容下部分段
				else
				{
					//将当前段之后的所有段都移入下一页
					if (page_cusr == container_page)
					{
						for (paragraph_iter reverse_it = --((*page_cusr)->end()); reverse_it != pgraph_cusr;)
						{
							paragraph_iter deleted = reverse_it;
							reverse_it--;
							(*deleted)->set_parent_page(*next_page);
							(*next_page)->add_paragraph(*deleted, 0);
							(*page_cusr)->remove_paragraph(deleted);
						}
					}
					//计算当前段的哪些行被移除
					int height_allowed_ = (*container_page)->get_top_pos() + LxPaper::pixel_height -
						LxPaper::bottom_margin - y_offset;
					int height_ = 0;
					row_iter row_it_ = (*pgraph_cusr)->begin();
					for (; row_it_ != (*pgraph_cusr)->end(); row_it_++)
					{
						height_ += (*row_it_)->get_height();
						if (height_ > height_allowed_)
						{
							break;
						}
						height_ += (*row_it_)->get_external_leading();
					}

					//新建一个段并将剩余的行加入其中
					ComposeParagraph* new_paragraph = new ComposeParagraph();
					new_paragraph->set_area((*row_it_)->get_area_begin(), (*pgraph_cusr)->get_area_end());
					new_paragraph->set_offset_inner((*row_it_)->get_area_begin() - (*pgraph_cusr)->get_area_begin());
					if (page_cusr == container_page)
						new_paragraph->set_parent_page(*next_page);
					else
						new_paragraph->set_parent_page(*page_cusr);
					new_paragraph->set_phy_paragraph((*pgraph_cusr)->get_phy_paragraph());
					new_paragraph->set_pos((*row_it_)->get_top_pos(), (*pgraph_cusr)->get_bottom_pos());

					//调整当前页和段的属性
					//(*container_page)->set_area( (*container_page)->get_area_begin(), (*row_it_)->get_area_begin() - 1 );
					(*pgraph_cusr)->set_area((*pgraph_cusr)->get_area_begin(), (*row_it_)->get_area_begin() - 1);
					row_it_--;
					(*pgraph_cusr)->set_pos((*pgraph_cusr)->get_top_pos(), (*row_it_)->get_bottom_pos());
					row_it_++;

					for (; row_it_ != (*pgraph_cusr)->end();)
					{
						row_iter to_move = row_it_;
						row_it_++;
						new_paragraph->add_row(*to_move);
						(*pgraph_cusr)->remove_row(to_move);
					}
					//如果段不属于container_page，则移入
					//将新段加入下一页
					if (page_cusr == container_page)
						(*next_page)->add_paragraph(new_paragraph, 0);
					else
						(*page_cusr)->add_paragraph(new_paragraph, 1);
					//在下一次段的循环中会自动平移段
				}
			}
		}
		page_cusr++;
		if (page_cusr == end())
			break;
		pgraph_cusr = (*page_cusr)->begin();
	}
	//处理完所有页后，删除container_page之后的所有页
	container_page++;
	for (; container_page != end();)
	{
		page_iter to_delete = container_page;
		container_page++;
		delete *to_delete;
		remove_page(to_delete);
	}
}

ComposePage::~ComposePage()
{
	for (auto pagraph : paragraphs)
		delete pagraph;
	paragraphs.clear();
}
void ComposePage::add_paragraph(ComposeParagraph* paragraph)
{
	paragraphs.push_back(paragraph);
}
void ComposePage::add_paragraph(ComposeParagraph* paragraph, int index)
{
	auto it = paragraphs.begin();
	advance(it, index);
	paragraphs.insert(it, paragraph);
}
ComposePage::inner_row_iter ComposePage::rowiter_begin()
{
	auto itb = (*paragraphs.begin())->begin();
	auto ite = (*paragraphs.begin())->end();
	for (; itb != ite; itb++)
	{
		if ((*itb)->get_top_pos() > this->top_pos_global)	//找出该页的第一个行。如果首个段是跨页的段
			break;
	}
	ComposePage::inner_row_iter it(this, paragraphs.begin(), itb);
	return it;
}
ComposePage::inner_row_iter ComposePage::rowiter_end()
{
	if (is_crossed_paragraph(paragraphs.back()))
	{
		auto itb = paragraphs.back()->begin();
		auto ite = paragraphs.back()->end();
		for (; itb != ite; itb++)
		{
			if ((*itb)->get_top_pos() > this->top_pos_global + LxPaper::pixel_height)	//找出该页的最后一个行。如果最后一个段是跨页的段
				break;
		}
		itb--;
		ComposePage::inner_row_iter it(this, --paragraphs.end(), itb);
		return it;
	}
	ComposePage::inner_row_iter it(this, paragraphs.end(), (*(--paragraphs.end()))->end());
	return it;
}

void ComposePage::Draw(CDC* pDC, TreeBase* font_tree, TreeBase* color_tree)
{
	for (auto paragraph_ : paragraphs)
	{
		if (paragraph_->get_bottom_pos() <= ViewWindow::GetViewWindowInstance()->offset_y)
			continue;
		if (paragraph_->get_top_pos() >= ViewWindow::GetViewWindowInstance()->get_bottom_pos())
			break;
		paragraph_->Draw(pDC, font_tree, color_tree);
	}
}

void ComposePage::FlushOwnArea(CDC* pDC)
{
	CRect rect(
		ViewWindow::GetViewWindowInstance()->border_width_left -
		ViewWindow::GetViewWindowInstance()->offset_x,
		this->get_top_pos() - ViewWindow::GetViewWindowInstance()->offset_y,
		ViewWindow::GetViewWindowInstance()->border_width_left + LxPaper::pixel_width -
		ViewWindow::GetViewWindowInstance()->offset_x,
		this->get_top_pos() + LxPaper::pixel_height - ViewWindow::GetViewWindowInstance()->offset_y);
	FlushRect(pDC, &rect, LxPaper::paper_back_color);
	//绘制四个角的定位标识
	pDC->SetDCPenColor(RGB(255, 0, 0));
	pDC->MoveTo(rect.left + LxPaper::left_margin / 2, rect.top + LxPaper::top_margin);
	pDC->LineTo(rect.left + LxPaper::left_margin, rect.top + LxPaper::top_margin);
	pDC->LineTo(rect.left + LxPaper::left_margin, rect.top + LxPaper::top_margin / 2);

	pDC->MoveTo(rect.left + LxPaper::left_margin / 2, rect.bottom - LxPaper::bottom_margin);
	pDC->LineTo(rect.left + LxPaper::left_margin, rect.bottom - LxPaper::bottom_margin);
	pDC->LineTo(rect.left + LxPaper::left_margin, rect.bottom - LxPaper::bottom_margin / 2);

	pDC->MoveTo(rect.right - LxPaper::right_margin / 2, rect.top + LxPaper::top_margin);
	pDC->LineTo(rect.right - LxPaper::right_margin, rect.top + LxPaper::top_margin);
	pDC->LineTo(rect.right - LxPaper::right_margin, rect.top + LxPaper::top_margin / 2);

	pDC->MoveTo(rect.right - LxPaper::right_margin / 2, rect.bottom - LxPaper::bottom_margin);
	pDC->LineTo(rect.right - LxPaper::right_margin, rect.bottom - LxPaper::bottom_margin);
	pDC->LineTo(rect.right - LxPaper::right_margin, rect.bottom - LxPaper::bottom_margin / 2);
}

ComposeParagraph::~ComposeParagraph()
{
	for (auto row : rows)
		delete row;
	rows.clear();
}
void ComposeParagraph::add_row(ComposeRow* row)
{
	rows.push_back(row);
}
void ComposeParagraph::add_row(ComposeRow* row, int index)
{
	auto it = rows.begin();
	advance(it, index);
	rows.insert(it, row);
}
void ComposeParagraph::Draw(CDC* pDC, TreeBase* font_tree, TreeBase* color_tree)
{
	for (auto row_ : rows)
	{
		if (row_->get_bottom_pos() <= ViewWindow::GetViewWindowInstance()->offset_y)
			continue;
		if (row_->get_top_pos() >= ViewWindow::GetViewWindowInstance()->get_bottom_pos())
			break;
		row_->Draw(pDC, font_tree, color_tree, get_phy_paragraph(), index_begin);
	}
}

ComposeRow::ComposeRow() {}

void ComposeRow::FlushOwnArea(CDC* pDC)
{
	CRect rect(
		ViewWindow::GetViewWindowInstance()->border_width_left -
		ViewWindow::GetViewWindowInstance()->offset_x + LxPaper::left_margin,
		this->get_top_pos() - ViewWindow::GetViewWindowInstance()->offset_y,
		ViewWindow::GetViewWindowInstance()->border_width_left + LxPaper::pixel_width -
		ViewWindow::GetViewWindowInstance()->offset_x - LxPaper::right_margin,
		this->get_bottom_pos() - ViewWindow::GetViewWindowInstance()->offset_y);
	FlushRect(pDC, &rect, LxPaper::paper_back_color);
}

void ComposeRow::Draw(CDC* pDC, TreeBase* font_tree, TreeBase* color_tree, Paragraph* pagraph, size_t base_index)
{
	if (this->size() == 0)
		return;
	size_t inner_index = this->index_begin - base_index;
	size_t font_index, same_font_cnt;
	size_t color_index, same_color_cnt;
	size_t index = this->index_begin;
	int x = ViewWindow::GetViewWindowInstance()->border_width_left -
		ViewWindow::GetViewWindowInstance()->offset_x + LxPaper::left_margin;
	for (; index <= this->index_end;)
	{
		font_tree->get_src_index(index, font_index, same_font_cnt);
		color_tree->get_src_index(index, color_index, same_color_cnt);
		size_t count = min(this->index_end - index + 1, min(same_font_cnt, same_color_cnt));
		CFont* font = SrcFontFactory::GetFontFactInstance()->get_src_font(font_index);
		pDC->SelectObject(font);
		pDC->SetTextColor(color_index);
		TEXTMETRIC trx;
		pDC->GetTextMetrics(&trx);
		CSize size;
		for (int i = 0; i < count; i++)
		{
			size = pDC->GetTextExtent(pagraph->get_context_ptr() + inner_index, 1);
			pDC->TextOutA(x, top_offset_session + baseline_offset_inner - trx.tmAscent,
				pagraph->get_context_ptr() + inner_index, 1);
			inner_index++;
			x += size.cx + words_space;
		}
		index += count;
	}
}
//do not need again
bool is_crossed_paragraph(ComposeParagraph* paragraph)
{
	return paragraph->get_bottom_pos() > paragraph->get_parent_page()->get_top_pos() + LxPaper::pixel_height;
}
void FlushRect(CDC* pDC, CRect* rect, COLORREF back_color)
{
	CBrush brush;
	brush.CreateSolidBrush(back_color);
	pDC->FillRect(rect, &brush);
	brush.DeleteObject();
}