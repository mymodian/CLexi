#include "stdafx.h"
#include "LxCommand.h"
#include "LxDcViCtl.h"

LxMoveCmd::LxMoveCmd(unsigned int direction)
	: direction_(direction)
{}
void LxMoveCmd::Excute(CDC* pDC)
{
	doc_view_ctrl_->move_cursor(pDC, direction_);
	doc_view_ctrl_->calc_font_color();
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
	doc_view_ctrl_->calc_font_color();
}

LxInsertCmd::LxInsertCmd(TCHAR* cs, size_t len, size_t src_font, COLORREF src_color, size_t phy_pgh_index, size_t pos_global, size_t pos_inner)
	: cs_(cs), len_(len), src_font_(src_font), src_color_(src_color), phy_pgh_index_(phy_pgh_index), pos_global_(pos_global), pos_inner_(pos_inner) {}
void LxInsertCmd::Excute(CDC* pDC)
{
	//获取插入前的状态信息 only for test and debugger
	/*LxCursor cursor;
	doc_view_ctrl_->get_cursor(cursor);
	std::cout << cursor.index_inner <<endl;*/
	///////////////////////////////////////////
	doc_view_ctrl_->insert(cs_, len_, src_font_, src_color_, phy_pgh_index_, pos_global_, pos_inner_);
	doc_view_ctrl_->modify_layout(pDC, len_, pos_global_, phy_pgh_index_);
	doc_view_ctrl_->draw_complete(pDC);
	doc_view_ctrl_->calc_font_color();
	// !-- caution this
	//每次插入排版前记录下一些调试需要知道的先前信息
	//排版后安排ComposeDoc和cursor的自检程序来辅助调试(页产生空段和页偏移不连续的现象)
	//document自检 only for test and debugger
	ASSERT(doc_view_ctrl_->self_check());
	///////////////////////////////////////////
}
void LxInsertCmd::Undo()
{

}

LxInsertPhyParagraphCmd::LxInsertPhyParagraphCmd(int index, int direction)
	: index_(index), direction_(direction)
{
}
void LxInsertPhyParagraphCmd::Excute(CDC* pDC)
{
	//1.安插物理段
	Paragraph* pgh = doc_view_ctrl_->insert_null_phy_paragraph(index_ + direction_);
	doc_view_ctrl_->add_phy_paragraph(pDC, pgh, index_, direction_);

	doc_view_ctrl_->draw_complete(pDC);
	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxInsertPhyParagraphCmd::Undo()
{

}

LxSingleRemoveCmd::LxSingleRemoveCmd(size_t phy_pgh_index, size_t pos_global, size_t pos_inner)
	: phy_pgh_index_(phy_pgh_index), pos_global_(pos_global), pos_inner_(pos_inner)
{}
LxSingleRemoveCmd::~LxSingleRemoveCmd() {}
void LxSingleRemoveCmd::Undo()
{

}
void LxSingleRemoveCmd::Excute(CDC* pDC)
{
	if (!doc_view_ctrl_->single_remove(phy_pgh_index_, pos_global_, pos_inner_))
	{
		doc_view_ctrl_->draw_complete(pDC);
		return;
	}
	doc_view_ctrl_->modify_layout(pDC, -1, pos_global_, phy_pgh_index_);
	doc_view_ctrl_->draw_complete(pDC);
	//document自检 only for test and debugger
	ASSERT(doc_view_ctrl_->self_check());
	///////////////////////////////////////////
	doc_view_ctrl_->calc_font_color();
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

LxMergeCmd::LxMergeCmd(size_t index_para2)
	: index_para2_(index_para2)
{
}
LxMergeCmd::~LxMergeCmd()
{

}
void LxMergeCmd::Excute(CDC* pDC)
{
	para1_size_ = doc_view_ctrl_->merge_phy_paragraph(index_para2_);
	doc_view_ctrl_->compose_merged_paragraph(pDC, index_para2_ - 1, para1_size_);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxMergeCmd::Undo()
{

}

LxSplitCmd::LxSplitCmd(size_t phy_paragraph_index, size_t offset_inner)
	: phy_paragraph_index_(phy_paragraph_index), offset_inner_(offset_inner)
{
}
LxSplitCmd::~LxSplitCmd()
{

}
void LxSplitCmd::Excute(CDC* pDC)
{
	Paragraph* new_phy_pgh = doc_view_ctrl_->split_phy_paragraph(phy_paragraph_index_, offset_inner_);
	doc_view_ctrl_->compose_splited_paragraph(pDC, phy_paragraph_index_, offset_inner_, new_phy_pgh);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxSplitCmd::Undo()
{

}

LxSectionRemoveCmd::LxSectionRemoveCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh)
	: section_begin_index_(section_begin_index), section_end_index_(section_end_index),
	section_begin_pgh_(section_begin_pgh), section_end_pgh_(section_end_pgh)
{
}
LxSectionRemoveCmd::~LxSectionRemoveCmd()
{

}
void LxSectionRemoveCmd::Excute(CDC* pDC)
{
	doc_view_ctrl_->remove_section(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxSectionRemoveCmd::Undo()
{

}

LxSectionWrapCmd::LxSectionWrapCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh)
	: section_begin_index_(section_begin_index), section_end_index_(section_end_index),
	section_begin_pgh_(section_begin_pgh), section_end_pgh_(section_end_pgh)
{
}
LxSectionWrapCmd::~LxSectionWrapCmd()
{

}
void LxSectionWrapCmd::Excute(CDC* pDC)
{
	doc_view_ctrl_->section_wrap(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxSectionWrapCmd::Undo()
{

}

LxSectionReplaceCmd::LxSectionReplaceCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh,
	TCHAR* cs, size_t len, size_t src_font, COLORREF src_color)
	: section_begin_index_(section_begin_index), section_end_index_(section_end_index),
	section_begin_pgh_(section_begin_pgh), section_end_pgh_(section_end_pgh),
	cs_(cs), len_(len), src_font_(src_font), src_color_(src_color)
{
}
LxSectionReplaceCmd::~LxSectionReplaceCmd()
{

}
void LxSectionReplaceCmd::Excute(CDC* pDC)
{
	doc_view_ctrl_->replace_section(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_,
		cs_, len_, src_font_, src_color_);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxSectionReplaceCmd::Undo()
{

}

LxModifyFontCmd::LxModifyFontCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh, size_t src_font)
	: section_begin_index_(section_begin_index), section_end_index_(section_end_index), src_font_(src_font),
	section_begin_pgh_(section_begin_pgh), section_end_pgh_(section_end_pgh)
{
}
LxModifyFontCmd::~LxModifyFontCmd()
{

}
void LxModifyFontCmd::Excute(CDC* pDC)
{
	doc_view_ctrl_->modify_section_font(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_, src_font_);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	//doc_view_ctrl_->calc_font_color();
}
void LxModifyFontCmd::Undo()
{

}

LxModifyColorCmd::LxModifyColorCmd(size_t section_begin_index, size_t section_end_index, COLORREF src_color)
	: section_begin_index_(section_begin_index), section_end_index_(section_end_index), src_color_(src_color)
{
}
LxModifyColorCmd::~LxModifyColorCmd()
{

}
void LxModifyColorCmd::Excute(CDC* pDC)
{
	doc_view_ctrl_->modify_section_color(section_begin_index_, section_end_index_, src_color_);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	//doc_view_ctrl_->calc_font_color();
}
void LxModifyColorCmd::Undo()
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
	for (auto it : command_list)
		delete it;
	command_list.clear();
}
void LxCommandMgr::reset()
{
	for (auto it : command_list)
		delete it;
	command_list.clear();
	LxCommand* empty_cmd = new LxCommand();
	empty_cmd->add_child_cmd(new LxEmptyCmd());
	command_list.push_back(empty_cmd);
	curr_ = command_list.begin();
}
void LxCommandMgr::insert_cmd(LxCommand* lx_cmd)
{
	if (!lx_cmd->CanUndo())
	{
		delete lx_cmd;
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