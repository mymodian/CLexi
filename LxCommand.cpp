#include "stdafx.h"
#include "LxCommand.h"

LxMoveCmd::LxMoveCmd(unsigned int direction)
	: direction_(direction)
{}
void LxMoveCmd::Excute(CDC* pDC)
{
	doc_view_ctrl_->move_cursor(pDC, direction_);
}

LxLocateCmd::LxLocateCmd(int point_x, int point_y)
	: point_x_(point_x), point_y_(point_y)
{}

void LxLocateCmd::Excute(CDC* pDC)
{
	int doc_inner_point_x = point_x_ - ViewWindow::GetViewWindowInstance()->border_width_left + 
		ViewWindow::GetViewWindowInstance()->offset_x;
	int doc_inner_point_y = point_y_ + ViewWindow::GetViewWindowInstance()->offset_y;

	doc_view_ctrl_->locate(pDC, doc_inner_point_x, doc_inner_point_y);
}

LxInsertCmd::LxInsertCmd(size_t ins_pos, size_t src_font, COLORREF src_color)
	: ins_pos(ins_pos), src_font(src_font), src_color(src_color)
{
}
LxInsertCmd::LxInsertCmd(TCHAR* cs, size_t len) : cs_(cs), len_(len) {}
void LxInsertCmd::Excute(CDC* pDC)
{
	//获取插入前的状态信息 only for test and debugger
	/*LxCursor cursor;
	doc_view_ctrl_->get_cursor(cursor);
	std::cout << cursor.index_inner <<endl;*/
	///////////////////////////////////////////
	doc_view_ctrl_->insert(cs_, len_);
	doc_view_ctrl_->modify_layout(pDC, len_);
	doc_view_ctrl_->draw_complete(pDC);
	// !-- caution this
	//每次插入排版前记录下一些调试需要知道的先前信息
	//排版后安排ComposeDoc和cursor的自检程序来辅助调试(页产生空段和页偏移不连续的现象)
	//document自检 only for test and debugger
	assert(doc_view_ctrl_->self_check());
	///////////////////////////////////////////
}
void LxInsertCmd::Undo()
{

}

LxSingleRemoveCmd::~LxSingleRemoveCmd() {}
void LxSingleRemoveCmd::Undo()
{

}
void LxSingleRemoveCmd::Excute(CDC* pDC)
{
	if (!doc_view_ctrl_->single_remove())
	{
		doc_view_ctrl_->draw_complete(pDC);
		return;
	}
	doc_view_ctrl_->modify_layout(pDC, -1);
	doc_view_ctrl_->draw_complete(pDC);
	//document自检 only for test and debugger
	assert(doc_view_ctrl_->self_check());
	///////////////////////////////////////////
}

LxDeleteCmd::~LxDeleteCmd()
{

}
void LxDeleteCmd::Excute(CDC* pDC)
{

}
void LxDeleteCmd::Undo()
{

}

void LxModifyViewCmd::Excute(CDC* pDC)
{

}

LxMergeCmd::LxMergeCmd(ComposeParagraph* paragraph1, ComposeParagraph* paragraph2)
	: paragraph1_(paragraph1), paragraph2_(paragraph2)
{
}
LxMergeCmd::~LxMergeCmd()
{

}
void LxMergeCmd::Excute(CDC* pDC)
{

}
void LxMergeCmd::Undo()
{

}

LxSplitCmd::LxSplitCmd(ComposeParagraph* paragraph)
	: paragraph(paragraph)
{
}
LxSplitCmd::~LxSplitCmd()
{

}
void LxSplitCmd::Excute(CDC* pDC)
{

}
void LxSplitCmd::Undo()
{

}

LxCommand::~LxCommand()
{
	for (LxCommandBase* it : command)
		delete it;
}
void LxCommand::add_child_cmd(LxCommandBase* child_cmd)
{
	command.push_back(child_cmd);
}
void LxCommand::Excute(CDC* pDC)
{
	for (LxCommandBase* it : command)
		it->Excute(pDC);
}
bool LxCommand::CanUndo()
{
	for (LxCommandBase* it : command)
		if (!it->CanUndo()) return false;
	return true;
}
void LxCommand::Undo()
{
	if (CanUndo())
	{
		auto rit = command.rbegin();
		auto rite = command.rend();
		for (; rit != rite; ++rit)
		{
			(*(rit))->Undo();
		}
	}
}
void LxCommand::set_dvctl(LxDcViCtl* doc_view_ctrl)
{
	for (LxCommandBase* it : command)
		it->set_dvctl(doc_view_ctrl);
}

LxCommandMgr::LxCommandMgr()
{
	LxCommand* empty_cmd = new LxCommand();
	empty_cmd->add_child_cmd(new LxEmptyCmd());
	command_list.push_back(empty_cmd);
	curr_ = command_list.begin();
}
LxCommandMgr::~LxCommandMgr()
{
	for (LxCommand* it : command_list)
		delete it;
}
void LxCommandMgr::insert_cmd(LxCommand* lx_cmd)
{
	if (!lx_cmd->CanUndo())
	{
		delete lx_cmd;
		return;
	}
	if (curr_ == command_list.begin())
	{
		command_list.push_back(lx_cmd);
		++curr_;
		return;
	}
	auto it = curr_;
	++it;
	for (; it != command_list.end();)
	{
		delete *it;
		it = command_list.erase(it);
	}
	command_list.push_back(lx_cmd);
	++curr_;
}
LxCommand* LxCommandMgr::get_redo_cmd()
{
	if (curr_ == --command_list.end()) return nullptr;
	++curr_;
	return *curr_;
}
LxCommand* LxCommandMgr::get_undo_cmd()
{
	if (curr_ == command_list.begin()) return nullptr;
	LxCommand* cmd = *curr_;
	--curr_;
	return cmd;
}