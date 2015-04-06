#include "stdafx.h"
#include "LxDocument.h"
#include <iostream>
using namespace std;

Document::Document() {}
Document::~Document()
{
	for(auto it=paragraph_list.begin();it != paragraph_list.end();it++)
		delete *it;
	paragraph_list.clear();
}

void Document::store_stream(FILE* file)
{
	for(auto it=paragraph_list.begin();it != paragraph_list.end();it++)
		(*it)->store_stream(file);
}

void Document::add_paragraph(Paragraph* paragraph)
{
	paragraph_list.push_back(paragraph);
}

void Document::insert_paragraph(Paragraph* paragraph,int index)
{
	auto it = paragraph_list.begin();
	advance(it, index);
	paragraph_list.insert(it, paragraph);
}

size_t Document::get_offset_inner(size_t index_global, size_t pgh_index)
{
	ASSERT(pgh_index < size());
	auto it = begin();
	size_t _offset = 0;
	for (int i = 0; i < pgh_index; ++i, ++it)
	{
		_offset += (*it)->size();
	}
	return index_global - _offset;
}

Paragraph* Document::get_pgh(int index)
{
	auto it = paragraph_list.begin();
	advance(it, index);
	return *it;
}

void Document::insert_paragraph(Paragraph* paragraph, contex_pgh_iter pos)
{
	paragraph_list.insert(pos, paragraph);
}

void Document::remove_paragraph(int index)
{
	contex_pgh_iter it = paragraph_list.begin();
	advance(it, index);
	paragraph_list.erase(it);
}

void Document::remove_paragraph(contex_pgh_iter pgh_it)
{
	paragraph_list.erase(pgh_it);
}

void Document::remove_paragraphs(size_t index_b, size_t index_e)
{
	auto it = begin();
	advance(it, index_b);
	for (int i = 0; i < index_e - index_b + 1; ++i)
	{
		delete *it;
		it = paragraph_list.erase(it);
	}
}

void Document::insert(size_t pos, TCHAR* cs, size_t len)
{
	size_t index = 0;
	for (Paragraph* paragraph : paragraph_list)
	{
		index += paragraph->size() - 1;
		if (pos <= index)
		{
			return;
		}
	}
}