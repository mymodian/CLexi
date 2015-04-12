#include "stdafx.h"
#include "LxDocument.h"
#include "LxComposeAlgom.h"
#include <iostream>
using namespace std;

Document::Document() {}
Document::~Document()
{
	for (auto pgh : paragraph_list)
		delete pgh;
	paragraph_list.clear();
}

void Document::clear()
{
	for (auto pgh : paragraph_list)
		delete pgh;
	paragraph_list.clear();
}

void Document::store_stream(FILE* file)
{
	store_stream_int(file, paragraph_list.size());
	for (auto pgh : paragraph_list)
		pgh->store_stream(file);
}

void Document::build_from_stream(FILE* file)
{
	int pgh_cnt = read_stream_int(file);
	for (int i = 0; i < pgh_cnt; i++)
	{
		Paragraph* pgh = new Paragraph();
		add_paragraph(pgh);
		pgh->build_from_stream(file);
	}
}

size_t Document::size()
{
	size_t size_total = 0;
	for (auto pgh : paragraph_list)
		size_total += pgh->size();
	return size_total;
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
	ASSERT(pgh_index < pgh_size());
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

LxComposeAlgom* Paragraph::GetComposeAlgom()
{
	return ComposeAlgoFactory::GetComposeAlgoFactInstance()->get_compose_algom(compose_algom_type_);
}
