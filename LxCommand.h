#include "stdafx.h"
#include "LxDcViCtl.h"
#include <list>
using namespace std;

#ifndef __LEXI_COMMAND_H
#define __LEXI_COMMAND_H

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

class LxInsertCmd : public LxCommandBase
{
public:
	LxInsertCmd() = delete;
	virtual ~LxInsertCmd() = default;
	LxInsertCmd(char* cs, size_t len);
	LxInsertCmd(size_t ins_pos, size_t src_font, COLORREF src_color);
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo() override;
private:
	char* cs_;
	size_t len_;
	size_t ins_pos;
	size_t src_font;
	COLORREF src_color;
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
	char* deleted_text;
};

class LxModifyViewCmd : public LxCommandBase
{
public:
	LxModifyViewCmd() = default;
	~LxModifyViewCmd() = default;
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
	LxSplitCmd(ComposeParagraph* paragraph);
	virtual ~LxSplitCmd();
public:
	virtual void Excute(CDC* pDC) override;
	virtual bool CanUndo() override { return true; }
	virtual void Undo() override;
private:
	ComposeParagraph* paragraph;
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