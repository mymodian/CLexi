#include "stdafx.h"
#include <list>
using std::list;

#ifndef __LEXI_COMPOSE_DOC_H
#define __LEXI_COMPOSE_DOC_H

#include "LxDocument.h"
#include "LxTreeBase.h"

/*
用来表示排版结构，当各种操作后都需要重新计算排版的变更，更改变化了的结构信息。包括行、段、页属性的变化。

*/
class ComposeBase;
class ComposeParagraph;
class ComposePage;
class ComposeDoc;
class ComposeRow;

typedef list<ComposeRow*> RowList;

typedef list<ComposePage*>::iterator page_iter;
typedef list<ComposeParagraph*>::iterator paragraph_iter;
typedef list<ComposeRow*>::iterator row_iter;

class LxRowInPageIter;
class LxParagraphInDocIter;

class ComposeBase
{
public:
	virtual void modify() = 0;
};

class ComposeRow
{
public:
	ComposeRow();
	~ComposeRow() {}
	void init();
	void set_area(size_t index_begin, size_t index_end)
	{
		this->index_begin = index_begin;
		this->index_end = index_end;
	}
	size_t get_area_begin() { return index_begin; }
	size_t get_area_end() { return index_end; }
	size_t size() { return index_end - index_begin + 1; }
public:
	int get_line_space() { return last_line_space; }
	void set_line_space(int space) { last_line_space = space; }
	int get_height() { return max_height; }
	void set_height(int height) { max_height = height; }
	int get_base_line() { return baseline_offset_inner; }
	void set_base_line(int base_line) { baseline_offset_inner = base_line; }
	int get_bottom_pos() { return top_offset_session + max_height; }
	int get_next_allowed_pos() { return top_offset_session + max_height + ri_external_leading; }
	int get_top_pos() { return top_offset_session; }
	void set_top_pos(int offset) { top_offset_session = offset; }
	int get_external_leading() { return ri_external_leading; }
	void set_external_leading(int leading) { ri_external_leading = leading; }
	void set_words_space(int words_space_) { words_space = words_space_; }
	int get_words_space() { return words_space; }
public:
	void FlushOwnArea(CDC* pDC);
	void Draw(CDC* pDC, TreeBase* font_tree, TreeBase* color_tree, Paragraph* pagraph, size_t base_index);
private:
	size_t index_begin;
	size_t index_end;
private:
	int words_space;						//字间距
	int top_offset_session;
	int max_height;							//行的高度
	int baseline_offset_inner;		//基线
	int ri_external_leading;			//字体建议的行距
	int last_line_space;					//行末剩余的空白空间(不包含任何字符)宽度
};

class ComposeParagraph
{
public:
	ComposeParagraph() {}
	~ComposeParagraph();
public:
	void add_row(ComposeRow* row);
	void add_row(ComposeRow* row,int index);
	void remove_row(row_iter row_it) { rows.erase(row_it); }
	void set_phy_paragraph(Paragraph* paragraph) { this->paragraph = paragraph; }
	Paragraph* get_phy_paragraph() { return paragraph; }
	void set_parent_page(ComposePage* parent) { parent_page = parent; }
	ComposePage* get_parent_page() { return parent_page; }
	void set_area(size_t index_begin, size_t index_end)
	{
		this->index_begin = index_begin;
		this->index_end = index_end;
	}
	size_t get_area_begin() { return index_begin; }
	size_t get_area_end() { return index_end; }
	int get_height() { return bottom_offset_session - top_offset_session; }
	void set_pos(int top_offset, int bottom_offset)
	{
		this->top_offset_session = top_offset;
		this->bottom_offset_session = bottom_offset;
	}
	int get_top_pos() { return top_offset_session; }
	int get_bottom_pos() { return bottom_offset_session; }
	void set_offset_inner(size_t offset_inner) { this->offset_inner = offset_inner; }
	size_t get_offset_inner() { return offset_inner; }
	size_t size() { return index_end - index_begin + 1; }
	size_t row_size() { return rows.size(); }
public:
	row_iter begin() { return rows.begin(); }
	row_iter end() { return rows.end(); }
public:
	//将跨页的段的处理隐藏在操作中
	void FlushOwnArea(CDC* pDC);
	void Draw(CDC* pDC, TreeBase* font_tree, TreeBase* color_tree);
private:
	// offset_inner -> 逻辑段起始位置在物理段的偏移，偏移为0代表对应的第一个逻辑段
	size_t offset_inner;
	size_t index_begin;
	size_t index_end;
private:
	int top_offset_session;
	int bottom_offset_session;
private:
	Paragraph* paragraph;
	ComposePage* parent_page;
	list<ComposeRow*> rows;
};

class ComposePage
{
public:
	ComposePage() {}
	~ComposePage();
	void add_paragraph(ComposeParagraph* paragraph);
	void add_paragraph(ComposeParagraph* paragraph,int index);
	void remove_paragraph(paragraph_iter it) { paragraphs.erase(it); }
	void set_area(size_t index_begin, size_t index_end)
	{
		this->index_begin = index_begin;
		this->index_end = index_end;
	}
	void set_top_pos(int top_pos) { top_pos_global = top_pos; }
	int get_area_begin() { return index_begin; }
	int get_area_end() { return index_end; }
	int get_top_pos() { return top_pos_global; }
	int get_last_space() { return last_space; }
	size_t size() { return index_end - index_begin + 1; }
public:
	paragraph_iter begin() { return paragraphs.begin(); }
	paragraph_iter end() { return paragraphs.end(); }
	typedef LxRowInPageIter inner_row_iter;
	inner_row_iter rowiter_begin();
	inner_row_iter rowiter_end();
public:
	//将跨页的段的处理隐藏在操作中
	void FlushOwnArea(CDC* pDC);
	void Draw(CDC* pDC, TreeBase* font_tree, TreeBase* color_tree);
private:
	//绝对位置
	size_t index_begin;
	size_t index_end;
private:
	int top_pos_global;		//页纸张顶部的坐标，页的起始有效位置还应该加上 LxPaper::top_margin
	int last_space;			//页最后剩下的空间高度
	list<ComposeParagraph*> paragraphs;
};

class ComposeDoc
{
public:
	ComposeDoc() = default;
	~ComposeDoc();
	void add_page(ComposePage* page);
	void add_page(ComposePage* page, int index);
	void remove_page(page_iter it) { pages.erase(it); }
public:
	void clear();
	//全部重新排版
	void compose_complete(CDC* pDC);
	//插入或删除文本时修改和重布局
	LxParagraphInDocIter modify(LxParagraphInDocIter pagraph_iter, row_iter pos, CDC* pDC);
	//插入或删除文本时修改相应结构的index
	void modify_index(LxParagraphInDocIter pagraph_iter, int count);
	//relayout作用为将剩下的区域集体上下移动，段中每行的排版无需改动
	void relayout(LxParagraphInDocIter pagraph_iter);
	paragraph_iter do_logic_combine(ComposePage* page, paragraph_iter paragraph_it);
public:
	void Draw(CDC* pDC);
	void AttachFontInfo(TreeBase* font_tree) { this->font_tree = font_tree; }
	void AttachColorInfo(TreeBase* color_tree) { this->color_tree = color_tree; }
	void AttachPhyDoc(Document* phy_document) { this->phy_document = phy_document; }
public:
	page_iter begin() { return pages.begin(); }
	page_iter end() { return pages.end(); }
	LxParagraphInDocIter pargraph_begin();
	LxParagraphInDocIter pargraph_end();
private:
	list<ComposePage*> pages;
	TreeBase* font_tree;
	TreeBase* color_tree;
	Document* phy_document;
};

/*
实现Iterator模式来访问其中的每一个行、段、页。
list的iterator可以保持有效。
*/

//do not need again
bool is_crossed_paragraph(ComposeParagraph* paragraph);
void FlushRect(CDC* pDC, CRect* rect, COLORREF back_color);

class LxRowInPageIter
{
public:
	LxRowInPageIter() = default;
	LxRowInPageIter(ComposePage* page, paragraph_iter pagraph, row_iter irow) :
		this_page(page), paragraph(pagraph), row(irow)
	{
	}
	bool operator==(const LxRowInPageIter& other) const
	{
		return row == other.row;
	}
	bool operator!=(const LxRowInPageIter& other) const
	{
		return row != other.row;
	}
	LxRowInPageIter& operator=(LxRowInPageIter& other)
	{
		this_page = other.this_page;
		paragraph = other.paragraph;
		row = other.row;
		return *this;
	}
	ComposeRow* operator*()
	{
		return *row;
	}
	LxRowInPageIter& operator++()
	{
		row++;
		if (row == (*paragraph)->end())
		{
			paragraph++;
			if (paragraph != this_page->end())
				row = (*paragraph)->begin();
		}
		return *this;
	}
	LxRowInPageIter operator++(int)
	{
		LxRowInPageIter tmp = *this;
		row++;
		if (row == (*paragraph)->end())
		{
			paragraph++;
			if (paragraph != this_page->end())
				row = (*paragraph)->begin();
		}
		return tmp;
	}
private:
	ComposePage* this_page;
	paragraph_iter paragraph;
	row_iter row;
};

class LxParagraphInDocIter
{
public:
	LxParagraphInDocIter() = default;
	LxParagraphInDocIter(ComposeDoc* the_doc, page_iter page_it, paragraph_iter pagraph_it) :
		doc(the_doc), page(page_it), paragraph(pagraph_it)
	{
	}
	inline page_iter get_page() { return page; }
	inline paragraph_iter get_paragraph() { return paragraph; }
	bool operator==(const LxParagraphInDocIter& other) const
	{
		return paragraph == other.paragraph;
	}
	bool operator!=(const LxParagraphInDocIter& other) const
	{
		return paragraph != other.paragraph;
	}
	LxParagraphInDocIter& operator=(LxParagraphInDocIter& other)
	{
		paragraph = other.paragraph;
		page = other.page;
		doc = other.doc;
		return *this;
	}
	LxParagraphInDocIter& operator++()
	{
		paragraph++;
		if (paragraph == (*page)->end())
		{
			page++;
			if (page != doc->end())
				paragraph = (*page)->begin();
		}
		return *this;
	}
	LxParagraphInDocIter operator++(int)
	{
		LxParagraphInDocIter tmp = *this;
		paragraph++;
		if (paragraph == (*page)->end())
		{
			page++;
			if (page != doc->end())
				paragraph = (*page)->begin();
		}
		return tmp;
	}
	ComposeParagraph* operator*()
	{
		return *paragraph;
	}
private:
	ComposeDoc* doc;
	paragraph_iter paragraph;
	page_iter page;
};

#endif