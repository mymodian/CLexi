/**
	当前存在的问题：所有操作如果要实现undo和redo，必定不能在execute和undo的实现中直接使用cursor。
	然而当前的诸多操作都是直接使用cursor。尽早将接口改过来，将cursor和操作的实现解耦
	*/

#include "stdafx.h"
#include "LxComposeDoc.h"
#include "LxStructuredContext.h"
#include <list>
using namespace std;

#ifndef __LEXI_COMMAND_H
#define __LEXI_COMMAND_H

class LxDcViCtl;

class LxCommandBase
{
public:
	LxCommandBase() = default;
	virtual ~LxCommandBase() = default;
	virtual void Excute(CDC* pDC) = 0;
	virtual bool CanUndo() = 0;
	virtual void Undo(CDC* pDC) {}
	virtual void set_dvctl(LxDcViCtl* doc_view_ctrl)
	{
		doc_view_ctrl_ = doc_view_ctrl;
	}
protected:
	LxDcViCtl* doc_view_ctrl_;
};

class LxEmptyCmd : public LxCommandBase
{
public:
	LxEmptyCmd() = default;
	virtual ~LxEmptyCmd() = default;
	virtual void Excute(CDC* pDC) override {}
	virtual bool CanUndo() override { return false; }
};

class LxMoveCmd : public LxCommandBase
{
public:
	LxMoveCmd() = delete;
	LxMoveCmd(unsigned int direction);
	virtual ~LxMoveCmd() = default;
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return false; }
private:
	unsigned int direction_;
};

class LxLocateCmd : public LxCommandBase
{
public:
	LxLocateCmd() = delete;
	LxLocateCmd(int point_x, int point_y);
	virtual ~LxLocateCmd() = default;
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return false; }
private:
	int point_x_;
	int point_y_;
};

class LxInsertCmd : public LxCommandBase
{
public:
	LxInsertCmd() = delete;
	virtual ~LxInsertCmd();
	LxInsertCmd(TCHAR* cs, size_t len, size_t src_font, COLORREF src_color, size_t phy_pgh_index, size_t pos_global, size_t pos_inner);
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	TCHAR* cs_;
	size_t len_;
	size_t src_font_;
	COLORREF src_color_;
	size_t phy_pgh_index_;
	size_t pos_global_;
	size_t pos_inner_;
};

class LxInsertPhyParagraphCmd : public LxCommandBase
{
public:
	LxInsertPhyParagraphCmd() = delete;
	virtual ~LxInsertPhyParagraphCmd() = default;
	LxInsertPhyParagraphCmd(int index, int direction);
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	int index_;				//该段在哪个段(index)上下插入
	int direction_;		//在index_段之前(0)还是之后(1)插入
};
// DO NOT use this command,use LxSectionRemove instead
class LxSingleRemoveCmd : public LxCommandBase
{
public:
	LxSingleRemoveCmd(size_t phy_pgh_index, size_t pos_global, size_t pos_inner);
	virtual ~LxSingleRemoveCmd();
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	size_t phy_pgh_index_;
	size_t pos_global_;
	size_t pos_inner_;
	TCHAR ch_;
	size_t font_index_;
	size_t color_index_;
};

class LxDeleteCmd : public LxCommandBase
{
public:
	LxDeleteCmd() = default;
	virtual ~LxDeleteCmd();
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	size_t pos_begin;
	size_t pos_end;
	size_t text_len;
	TCHAR* deleted_text;
};

class LxModifyViewCmd : public LxCommandBase
{
public:
	LxModifyViewCmd() = default;
	virtual ~LxModifyViewCmd() = default;
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return false; }
};

class LxMergeCmd : public LxCommandBase
{
public:
	LxMergeCmd() = delete;
	LxMergeCmd(size_t index_para2);
	virtual ~LxMergeCmd();
public:
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	size_t index_para2_;
	size_t para1_size_;
};

class LxSplitCmd : public LxCommandBase
{
public:
	LxSplitCmd() = delete;
	LxSplitCmd(size_t phy_paragraph_index, size_t offset_inner);
	virtual ~LxSplitCmd();
public:
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	size_t phy_paragraph_index_;
	size_t offset_inner_;
};

class LxStructuredContextInsertCmd : public LxCommandBase
{
public:
	LxStructuredContextInsertCmd() = delete;
	LxStructuredContextInsertCmd(size_t index_gbl, size_t pgh_index, StructuredSectionContext* structured_context);
	virtual ~LxStructuredContextInsertCmd();
public:
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	size_t index_gbl_;
	size_t pgh_index_;
	size_t context_size_;
	size_t phy_pgh_cnt_;
	bool bOwnContext;
	StructuredSectionContext* structured_context_;
};

class LxSectionRemoveCmd : public LxCommandBase
{
public:
	LxSectionRemoveCmd() = delete;
	LxSectionRemoveCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh);
	virtual ~LxSectionRemoveCmd();
	void move_structured_context(StructuredSectionContext* &structured_section_context_to);
public:
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	size_t section_begin_index_;
	size_t section_begin_pgh_;
	size_t section_end_index_;
	size_t section_end_pgh_;
	StructuredSectionContext* structured_section_context_;
};

class LxSectionWrapCmd : public LxCommandBase
{
public:
	LxSectionWrapCmd() = delete;
	LxSectionWrapCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh);
	virtual ~LxSectionWrapCmd();
public:
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	size_t section_begin_index_;
	size_t section_begin_pgh_;
	size_t section_end_index_;
	size_t section_end_pgh_;
	StructuredSectionContext* structured_section_context_;
};
//DO NOT USE this command,this command has been abanded
class LxSectionReplaceCmd : public LxCommandBase
{
public:
	LxSectionReplaceCmd() = delete;
	LxSectionReplaceCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh,
		TCHAR* cs, size_t len, size_t src_font, COLORREF src_color);
	virtual ~LxSectionReplaceCmd();
public:
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	size_t section_begin_index_;
	size_t section_begin_pgh_;
	size_t section_end_index_;
	size_t section_end_pgh_;

	TCHAR* cs_;
	size_t len_;
	size_t src_font_;
	COLORREF src_color_;
};

class LxModifyFontCmd : public LxCommandBase
{
public:
	LxModifyFontCmd() = delete;
	LxModifyFontCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh, size_t src_font);
	virtual ~LxModifyFontCmd();
public:
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	size_t section_begin_index_;
	size_t section_begin_pgh_;
	size_t section_end_index_;
	size_t section_end_pgh_;
	size_t src_font_;
	StructuredSrcContext* font_contex_;
};

class LxModifyColorCmd : public LxCommandBase
{
public:
	LxModifyColorCmd() = delete;
	LxModifyColorCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh, COLORREF src_color);
	virtual ~LxModifyColorCmd();
public:
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo(CDC* pDC) override;
private:
	size_t section_begin_index_;
	size_t section_begin_pgh_;
	size_t section_end_index_;
	size_t section_end_pgh_;
	COLORREF src_color_;
	StructuredSrcContext* color_contex_;
};

class LxCommand : public LxCommandBase
{
public:
	LxCommand() = default;
	virtual ~LxCommand();
	void add_child_cmd(LxCommandBase* child_cmd);
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override;
	virtual void Undo(CDC* pDC) override;
	virtual void set_dvctl(LxDcViCtl* doc_view_ctrl) override;
private:
	list<LxCommandBase*> command;
};

class LxCommandMgr
{
public:
	LxCommandMgr();
	~LxCommandMgr();

	void reset();
	void insert_cmd(LxCommand* lx_cmd);
	LxCommand* get_redo_cmd();
	LxCommand* get_undo_cmd();
private:
	list<LxCommand*> command_list;
	list<LxCommand*>::iterator curr_;
	LxCommand* save_point_;
};

#endif