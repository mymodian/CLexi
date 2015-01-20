#include <list>
#include <iostream>
#include "LxContextBuf.h"

#ifndef __LEXI_DOCUMENT_H
#define __LEXI_DOCUMENT_H

class LxComposeAlgom;
class Paragraph;
typedef std::list<Paragraph*>::iterator contex_pgh_iter;

class Document
{
public:
	Document();
	virtual ~Document();
	contex_pgh_iter begin() { return paragraph_list.begin(); }
	contex_pgh_iter end() { return paragraph_list.end(); }
public:
	void store_stream(FILE* file);
	void add_paragraph(Paragraph* paragraph);
	void insert_paragraph(Paragraph* paragraph,int index);
public:
	void insert(size_t pos, TCHAR* cs, size_t len);		//insert中不包括换行符
private:
	std::list<Paragraph*> paragraph_list;
};

class Paragraph
{
public:
	Paragraph() : compose_algom(nullptr) {}
	virtual ~Paragraph() {}
	void SetComposeAlgom(LxComposeAlgom* compose_algom)
	{
		this->compose_algom = compose_algom;
	}
	LxComposeAlgom* GetComposeAlgom() { return compose_algom; }
public:
	inline void store_stream(FILE* file)
	{
		context.store_stream(file);
	}
public:
	const TCHAR* get_context_ptr() const { return context.get_context_ptr(); }
	inline TCHAR Get(int index) { return context.Get(index); }
	inline size_t size() { return context.size(); }
	inline void Insert(size_t position, TCHAR ch)
	{
		context.Insert(position,ch);
	}
	inline void Insert(size_t position, TCHAR* str, size_t count)
	{
		context.Insert(position,str,count);
	}
	inline void Delete(size_t position)
	{
		context.Delete(position);
	}
	inline void Delete(size_t section_begin, size_t section_end)
	{
		context.Delete(section_begin,section_end);
	}
private:
	//how to store the paragraph context string
	//RandomAccessStrBuf context;
	GeneralStrBuf context;
	LxComposeAlgom* compose_algom;
};

#endif