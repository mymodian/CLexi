#include "stdafx.h"
#include "LxDocument.h"
#include "LxComposeDoc.h"
#include "LxTreeBase.h"
#include "LxViewWindow.h"
#include "LxPaper.h"
#include "LxCursor.h"
#include "LxComposeAlgom.h"
#include "LxRender.h"
#include "LxCommand.h"

#ifndef __LX_DOCUMENT_VIEW_CONTROL_H
#define __LX_DOCUMENT_VIEW_CONTROL_H

class LxDcViCtl
{
public:
	LxDcViCtl();
	~LxDcViCtl();
	void init(CDC* pDC);
	//用户操作
public:
	void usr_mouse_lbutton_down(CDC* pDC, int x, int y);
	void usr_mouse_move(CDC* pDC, int x, int y);
	void usr_mouse_lbutton_up(CDC* pDC, int x, int y);
	void usr_mouse_rbutton_down(CDC* pDC, int x, int y);
	void usr_mouse_rbutton_up(CDC* pDC, int x, int y);
	void usr_insert(CDC* pDC, TCHAR* cs, int len);
	void usr_wrap(CDC* pDC);
	void usr_backspace(CDC* pDC);
	void usr_move_cursor(CDC* pDC, unsigned int direction);
public:
	void backspace();
	void insert(TCHAR* src, size_t  count);
	//void insert(TCHAR* src, size_t  count, size_t font_src_index);
	void remove(size_t position);
	void remove(size_t position_begin, size_t position_end);
	void modify_font(size_t position_begin, size_t position_end, size_t font_src_index);
	void modify_color(size_t position_begin, size_t position_end, size_t color_src_index);
//排版相关
public:
	void modify_layout(CDC* pDC, int count);
	void compose_complete(CDC* pDC);
	void draw_complete(CDC* pDC);
	void locate(CDC* pDC, int doc_x, int doc_y);
	bool single_remove();
	void move_cursor(CDC* pDC, unsigned direction);
//member for test and debugger.
public:
	ComposeDoc* get_com_doc() { return &compose_doc; }
	void get_cursor(LxCursor& _cursor)
	{
		_cursor.index_inner = cursor.index_inner;
		_cursor.page = cursor.page;
		_cursor.paragraph = cursor.paragraph;
		_cursor.row = cursor.row;
	}
	bool self_check() { return compose_doc.self_check(); }
private:
	Document document;
	ComposeDoc compose_doc;
	TreeBase font_tree;
	TreeBase color_tree;
	LxCursor cursor;
	GD_proxy_base gd_proxy;
	LxRender* render;
	Section section;
private:
	LxCommandMgr lx_command_mgr;		//命令管理
private:
	size_t default_font_index;
	size_t default_color_index;
};

#endif