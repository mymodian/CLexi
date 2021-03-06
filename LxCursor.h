#include "LxComposeDoc.h"

#ifndef __LEXI_CURSOR_H
#define __LEXI_CURSOR_H

class LxCursor
{
public:
	LxCursor() : point_x(0), point_y(0), index_inner(0) {}
	LxCursor(const LxCursor& other)
	{
		this->point_x = other.point_x;
		this->point_y = other.point_y;
		this->height = other.height;
		this->index_inner = other.index_inner;
		this->page = other.page;
		this->paragraph = other.paragraph;
		this->row = other.row;
	}
	~LxCursor() {}
	LxCursor& operator=(const LxCursor& other)
	{
		this->point_x = other.point_x;
		this->point_y = other.point_y;
		this->height = other.height;
		this->index_inner = other.index_inner;
		this->page = other.page;
		this->paragraph = other.paragraph;
		this->row = other.row;
		return *this;
	}
	bool operator==(const LxCursor& other)
	{
		return point_x == other.point_x && point_y == other.point_y;
	}
	bool operator!=(const LxCursor& other)
	{
		return point_x != other.point_x || point_y != other.point_y;
	}
	bool operator<(const LxCursor& other)
	{
		if ((*this->row)->get_top_pos() == (*other.row)->get_top_pos())
			return this->point_x < other.point_x;
		return this->point_y < other.point_y;
	}
	bool operator>(const LxCursor& other)
	{
		if ((*this->row)->get_top_pos() == (*other.row)->get_top_pos())
			return this->point_x > other.point_x;
		return this->point_y > other.point_y;
	}
	/*LxCursor& operator++();
	LxCursor operator++(int);*/
	size_t get_index_global()
	{
		return (*row)->get_area_begin() + index_inner;
	}
	size_t get_index_inner_paragraph()
	{
		return get_index_global() - (*paragraph)->get_area_begin() + (*paragraph)->get_offset_inner();
	}
	Paragraph* get_phy_paragraph()
	{
		return (*paragraph)->get_phy_paragraph();
	}
	bool tail_of_paragraph()
	{
		return get_index_inner_paragraph() == (*paragraph)->get_phy_paragraph()->size();
	}
	bool head_of_paragraph()
	{
		return get_index_inner_paragraph() == 0;
	}
public:
	int point_x;			//carset的位置相对于页面范围
	int point_y;
	int height;				//光标的高度随字体而改变
	int index_inner;		//在当前行的第几个字符后,如果此处没有字符，则为0 //need
	//int width_used;		//当前行在此处达到的宽度 //don't need
	page_iter page;
	paragraph_iter paragraph;
	row_iter row;
};

/*
选择区域：表示被选中的区域信息；
在除了滚动和copy的其他一切操作后，将销毁。
滚动时将进行重绘，选中的文本添加灰色背景
*/
struct Section
{
	LxCursor cursor_begin;
	LxCursor cursor_end;
	bool trace;		//点下左键之后设置，才能跟踪鼠标移动
	bool active()
	{
		return cursor_begin != cursor_end;
	}
};

//

#endif