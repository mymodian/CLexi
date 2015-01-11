#include "stdafx.h"
#include "LxContextBuf.h"

TrivalStrBuf::TrivalStrBuf() : size_(0) {}
TrivalStrBuf::~TrivalStrBuf() {}

void TrivalStrBuf::store_stream(FILE* file)
{
	if( size_ > 0 )
		fwrite(str_buf,sizeof(char),size_,file);
}

size_t TrivalStrBuf::Insert(size_t position, char ch)
{
	if( remaining_capacity() == 0 ) return 0;
	if( position > size_ ) return 0;
	if( position == size_ )
	{
		str_buf[size_++]=ch;
		return 1;
	}
	memmove(str_buf+position+1,str_buf+position,size_-position);
	str_buf[position]=ch;
	size_++;
	return 1;
}
size_t TrivalStrBuf::Insert(size_t position, char* str, size_t count)
{
	if( remaining_capacity() < count ) return 0;
	if( position > size_ ) return 0;
	if( position == size_ )
	{
		memcpy(str_buf+position,str,count);
		size_+=count;
		return count;
	}
	memmove(str_buf+position+count,str_buf+position,size_-position);
	memcpy(str_buf+position,str,count);
	size_+=count;
	return count;
}
size_t TrivalStrBuf::Delete(size_t section_begin, size_t section_end)
{
	if( section_begin > section_end ) return 0;
	if( section_begin >= size_ ) return 0;
	if( section_end >= size_-1 )
	{
		size_t deleted=size_-section_begin;
		size_-=deleted;
		return deleted;
	}
	memmove(str_buf+section_begin,str_buf+section_end+1,size_-1-section_end);
	size_-=section_end-section_begin+1;
	return section_end-section_begin+1;
}


RandomAccessStrBuf::RandomAccessStrBuf() {}
RandomAccessStrBuf::~RandomAccessStrBuf()
{
	auto it_b=str_buffer_list.begin();
	auto it_e=str_buffer_list.end();
	for(;it_b != it_e;it_b++)
		delete *it_b;
	str_buffer_list.clear();
}

void RandomAccessStrBuf::store_stream(FILE* file)
{
	auto it_b=str_buffer_list.begin();
	auto it_e=str_buffer_list.end();
	for(;it_b != it_e;it_b++)
		(*it_b)->store_stream(file);
}

void RandomAccessStrBuf::Insert(size_t position, char ch)
{
	size_t cursor=0;
	auto it_b=str_buffer_list.begin();
	auto it_e=str_buffer_list.end();
	for(;it_b != it_e;it_b++)
		cursor+=(*it_b)->size();
}
void RandomAccessStrBuf::Insert(size_t position, char* str, size_t count)
{
}
void RandomAccessStrBuf::Delete(size_t section_begin, size_t section_end)
{
}