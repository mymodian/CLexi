#include <list>
using std::list;

#ifndef __LEXI_COMPOSE_ITER_H
#define __LEXI_COMPOSE_ITER_H

class ComposeParagraph;
class ComposePage;
class ComposeDoc;
class ComposeRow;

typedef list<ComposeRow*> RowList;

typedef list<ComposePage*>::iterator page_iter;
typedef list<ComposeParagraph*>::iterator paragraph_iter;
typedef list<ComposeRow*>::iterator row_iter;

class LxRowInPageIter;

#include "LxComposeDoc.h"

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



#endif