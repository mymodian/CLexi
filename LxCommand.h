/**
	当前存在的问题：所有操作如果要实现undo和redo，必定不能在execute和undo的实现中直接使用cursor。
	然而当前的诸多操作都是直接使用cursor。尽早将接口改过来，将cursor和操作的实现解耦
*/

#include "stdafx.h"
#include "LxComposeDoc.h"
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
	virtual void Undo() {}
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
	virtual ~LxInsertCmd() = default;
	LxInsertCmd(TCHAR* cs, size_t len);
	LxInsertCmd(size_t ins_pos, size_t src_font, COLORREF src_color);
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo() override;
private:
	TCHAR* cs_;
	size_t len_;
	size_t ins_pos;
	size_t src_font;
	COLORREF src_color;
};

class LxInsertPhyParagraphCmd : public LxCommandBase
{
public:
	LxInsertPhyParagraphCmd() = delete;
	virtual ~LxInsertPhyParagraphCmd() = default;
	LxInsertPhyParagraphCmd(int index, int direction);
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo() override;
private:
	int index_;				//该段在哪个段(index)上下插入
	int direction_;		//在index_段之前(0)还是之后(1)插入
};

class LxSingleRemoveCmd : public LxCommandBase
{
public:
	LxSingleRemoveCmd() = default;
	virtual ~LxSingleRemoveCmd();
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo() override;
};

class LxDeleteCmd : public LxCommandBase
{
public:
	LxDeleteCmd() = default;
	virtual ~LxDeleteCmd();
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo() override;
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
	LxMergeCmd(ComposeParagraph* paragraph1, ComposeParagraph* paragraph2);
	virtual ~LxMergeCmd();
public:
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo() override;
private:
	ComposeParagraph* paragraph1_;
	ComposeParagraph* paragraph2_;
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
	virtual void Undo() override;
private:
	size_t phy_paragraph_index_;
	size_t offset_inner_;
};

class LxCommand : public LxCommandBase
{
public:
	LxCommand() = default;
	virtual ~LxCommand();
	void add_child_cmd(LxCommandBase* child_cmd);
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override;
	virtual void Undo() override;
	virtual void set_dvctl(LxDcViCtl* doc_view_ctrl) override;
private:
	list<LxCommandBase*> command;
};

class LxCommandMgr
{
public:
	LxCommandMgr();
	~LxCommandMgr();

	void insert_cmd(LxCommand* lx_cmd);
	LxCommand* get_redo_cmd();
	LxCommand* get_undo_cmd();
private:
	list<LxCommand*> command_list;
	list<LxCommand*>::iterator curr_;
};

#endif