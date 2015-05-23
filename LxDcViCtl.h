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
#include "LxStructuredContext.h"

#ifndef __LX_DOCUMENT_VIEW_CONTROL_H
#define __LX_DOCUMENT_VIEW_CONTROL_H

class LxDcViCtl
{
public:
	LxDcViCtl();
	~LxDcViCtl();
	void init(CDC* pDC);
	void init(CDC* pDC, FILE* file);
	void clear();
	void store_stream(FILE* file);
	void build_from_stream(FILE* file);
	//用户操作
public:
	void usr_undo(CDC* pDC);
	void usr_redo(CDC* pDC);
	void usr_mouse_lbutton_down(CDC* pDC, int x, int y);
	void usr_mouse_move(CDC* pDC, int x, int y);
	void usr_mouse_lbutton_up(CDC* pDC, int x, int y);
	void usr_mouse_rbutton_down(CDC* pDC, int x, int y);
	void usr_mouse_rbutton_up(CDC* pDC, int x, int y);
	void usr_insert(CDC* pDC, TCHAR* cs, int len, size_t src_font, COLORREF src_color);
	void usr_wrap(CDC* pDC);
	void usr_backspace(CDC* pDC);
	void usr_move_cursor(CDC* pDC, unsigned int direction);
	void usr_font_change(CDC* pDC, LOGFONT log_font);
	void usr_color_change(CDC* pDC, COLORREF src_color);
	void usr_select_all(CDC* pDC);
	void usr_copy();
	void usr_cut(CDC* pDC);
	void usr_paste(CDC* pDC);
public:
	bool section_active() { return section.active(); }
public:
	void backspace();
	void insert(TCHAR* src, size_t  count, size_t src_font, COLORREF src_color, size_t phy_pgh_index, size_t pos_global, size_t pos_inner);
	void insert(size_t pos_global, size_t pos_pgh_index, TCHAR* src, size_t  count);
	//void insert(TCHAR* src, size_t  count, size_t font_src_index);
	void remove(size_t position);
	void remove(size_t position_begin, size_t position_end);
	void modify_font(size_t position_begin, size_t position_end, size_t font_src_index);
	void modify_color(size_t position_begin, size_t position_end, size_t color_src_index);
	//操作物理文档
public:
	Paragraph* insert_null_phy_paragraph(int index);
	Paragraph* split_phy_paragraph(size_t phy_paragraph_index, size_t offset_inner);
	size_t merge_phy_paragraph(size_t index_para2);
	void remove_phy_section(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, 
		size_t section_end_pgh, StructuredSectionContext* structured_section_context);
	//排版相关
public:
	void add_phy_paragraph(CDC* pDC, Paragraph* pgh, int index, int direction);
	void compose_splited_paragraph(CDC* pDC, size_t phy_paragraph_index, size_t offset_inner, Paragraph* seprated_phy_pgh);
	void compose_merged_paragraph(CDC* pDC, size_t index_para1, size_t offset_para1);
	void modify_layout(CDC* pDC, int count, size_t cur_gbl_index, size_t pgh_index);
	void compose_complete(CDC* pDC);
	void draw_complete(CDC* pDC);
	void draw_section(CDC* pDC, Section* _section);
	void clear_section(CDC* pDC, Section* _section);
	int modify_cursor_offset();
	size_t get_current_cur_index();
	void locate(CDC* pDC, int doc_x, int doc_y);
	void locate(CDC* pDC, Paragraph* pgh, int global_index);
	bool single_remove(size_t phy_pgh_index_, size_t pos_global_, size_t pos_inner_, TCHAR& ch, size_t& font_index, size_t& color_index);
	void move_cursor(CDC* pDC, unsigned direction);
	void modify_view_size(int width, int height);
	void modify_mouse_hscroll(CDC* pDC, int hdistanse);
	void modify_mouse_vscroll(CDC* pDC, int vdistanse);
	void calc_font_color();

	//排版相关-------在 section != NULL 下的操作
public:
	void modify_section_color(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh, 
		size_t section_end_index, size_t section_end_pgh, COLORREF src_color);
	void modify_section_font(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh,
		size_t section_end_index, size_t section_end_pgh, size_t src_font);
	void modify_section_font(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh,
		size_t section_end_index, size_t section_end_pgh, StructuredSrcContext* font_context);
	void remove_section(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh,
		size_t section_end_index, size_t section_end_pgh, StructuredSectionContext* structured_section_context);
	void replace_section(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index,
		size_t section_end_pgh, TCHAR* cs, size_t len, size_t src_font, COLORREF src_color);
	void section_wrap(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh,
		size_t section_end_index, size_t section_end_pgh, StructuredSectionContext* structured_section_context);

	//信息记录
public:
	void record_section_src_info(TreeBase* src_tree, StructuredSrcContext* src_contex, size_t section_begin, size_t section_end);
	void record_section_color_info(StructuredSrcContext* color_contex, size_t section_begin, size_t section_end);
	void record_section_font_info(StructuredSrcContext* font_contex, size_t section_begin, size_t section_end);
	void record_section_context(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index,
		size_t section_end_pgh, StructuredSectionContext* structured_section_context);

	bool doc_changed();
	//信息恢复
public:
	void modify_structured_color_context(StructuredSrcContext* color_contex);
	void insert_structured_context(CDC* pDC, StructuredSectionContext* structured_section_context, 
		size_t section_begin_index, size_t section_begin_pgh);
	void reset_selection(CDC* pDC, size_t section_begin_index, size_t section_begin_pgh, 
		size_t section_end_index, size_t section_end_pgh);

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
	LxScrollRender* render;
	Section section;
	StructuredSectionContext copy_context;
private:
	LxCommandMgr lx_command_mgr;		//命令管理
private:
	size_t default_font_index;
	size_t default_color_index;
};

#endif