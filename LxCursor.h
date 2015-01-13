#include "LxComposeDoc.h"

#ifndef __LEXI_CURSOR_H
#define __LEXI_CURSOR_H

class LxCursor
{
public:
	LxCursor() :point_x(0), point_y(0), index_inner(0) {}
	~LxCursor() {}
	LxCursor& operator++();
	LxCursor operator++(int);
	size_t get_index_global()
	{
		return (*row)->get_area_begin() + index_inner;
	}
public:
	int point_x;			//carset的位置相对于页面范围
	int point_y;
	int height;				//光标的高度随字体而改变
	int index_inner;		//在当前行的第几个字符后 //need
	int width_used;		//当前行在此处达到的宽度 //don't need
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
};

//

#endif