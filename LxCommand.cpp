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
	: src_font_(src_font), src_color_(src_color), phy_pgh_index_(phy_pgh_index), pos_global_(pos_global), pos_inner_(pos_inner)
{
	len_ = len;
	cs_ = new TCHAR[len];
	memcpy(cs_, cs, len*sizeof(TCHAR));
}
LxInsertCmd::~LxInsertCmd()
{
	delete[] cs_;
}
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
void LxInsertCmd::Undo(CDC* pDC)
{
	LxSectionRemoveCmd section_rm_cmd(pos_global_, phy_pgh_index_, pos_global_ + len_, phy_pgh_index_);
	section_rm_cmd.set_dvctl(doc_view_ctrl_);
	section_rm_cmd.Excute(pDC);
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
void LxInsertPhyParagraphCmd::Undo(CDC* pDC)
{
	LxMergeCmd merge_cmd(index_ + 1);
	merge_cmd.set_dvctl(doc_view_ctrl_);
	merge_cmd.Excute(pDC);
}

LxSingleRemoveCmd::LxSingleRemoveCmd(size_t phy_pgh_index, size_t pos_global, size_t pos_inner)
	: phy_pgh_index_(phy_pgh_index), pos_global_(pos_global), pos_inner_(pos_inner), ch_('\0')
{}
LxSingleRemoveCmd::~LxSingleRemoveCmd() {}
void LxSingleRemoveCmd::Undo(CDC* pDC)
{
	if (ch_ != '\0')
	{
		LxInsertCmd insert_cmd(&ch_, 1, font_index_, color_index_, phy_pgh_index_, pos_global_ - 1, pos_inner_ - 1);
		insert_cmd.set_dvctl(doc_view_ctrl_);
		insert_cmd.Excute(pDC);
	}
	else
	{
		doc_view_ctrl_->draw_complete(pDC);
	}
}
void LxSingleRemoveCmd::Excute(CDC* pDC)
{
	if (!doc_view_ctrl_->single_remove(phy_pgh_index_, pos_global_, pos_inner_, ch_, font_index_, color_index_))
	{
		ch_ = '\0';
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
void LxDeleteCmd::Undo(CDC* pDC)
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
void LxMergeCmd::Undo(CDC* pDC)
{
	LxSplitCmd split_command(index_para2_ - 1, para1_size_);
	split_command.set_dvctl(doc_view_ctrl_);
	split_command.Excute(pDC);
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
void LxSplitCmd::Undo(CDC* pDC)
{
	LxMergeCmd merge_command(phy_paragraph_index_ + 1);
	merge_command.set_dvctl(doc_view_ctrl_);
	merge_command.Excute(pDC);
}

LxStructuredContextInsertCmd::LxStructuredContextInsertCmd(size_t index_gbl, size_t pgh_index, StructuredSectionContext* structured_context)
	: index_gbl_(index_gbl), pgh_index_(pgh_index), structured_context_(structured_context)
{
	bOwnContext = false;
	context_size_ = structured_context->size();
	phy_pgh_cnt_ = structured_context->doc_context.size();
}
LxStructuredContextInsertCmd::~LxStructuredContextInsertCmd()
{
	if (structured_context_ && bOwnContext)
		delete structured_context_;
}
void LxStructuredContextInsertCmd::Excute(CDC* pDC)
{
	doc_view_ctrl_->insert_structured_context(pDC, structured_context_, index_gbl_, pgh_index_);
	doc_view_ctrl_->draw_complete(pDC);

	if (bOwnContext)
	{
		delete structured_context_;
		structured_context_ = nullptr;
	}
	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxStructuredContextInsertCmd::Undo(CDC* pDC)
{
	LxSectionRemoveCmd section_rm_cmd(index_gbl_, pgh_index_, index_gbl_ + context_size_, pgh_index_ + phy_pgh_cnt_ - 1);
	section_rm_cmd.set_dvctl(doc_view_ctrl_);
	section_rm_cmd.Excute(pDC);
	section_rm_cmd.move_structured_context(structured_context_);
	bOwnContext = true;
}

LxSectionRemoveCmd::LxSectionRemoveCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh)
	: section_begin_index_(section_begin_index), section_end_index_(section_end_index),
	section_begin_pgh_(section_begin_pgh), section_end_pgh_(section_end_pgh), 
	structured_section_context_(nullptr)
{
}
LxSectionRemoveCmd::~LxSectionRemoveCmd()
{
	if (structured_section_context_)
	{
		delete structured_section_context_;
	}
}
void LxSectionRemoveCmd::move_structured_context(StructuredSectionContext* &structured_section_context_to)
{
	structured_section_context_to = structured_section_context_;
	structured_section_context_ = nullptr;
}
void LxSectionRemoveCmd::Excute(CDC* pDC)
{
	if (!structured_section_context_)
	{
		structured_section_context_ = new StructuredSectionContext();
	}
	doc_view_ctrl_->remove_section(pDC, section_begin_index_, section_begin_pgh_, 
		section_end_index_, section_end_pgh_, structured_section_context_);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxSectionRemoveCmd::Undo(CDC* pDC)
{
	size_t _section_begin_pgh = section_begin_pgh_ < section_end_pgh_ ? section_begin_pgh_ : section_end_pgh_;
	size_t _section_begin_index = section_begin_index_ < section_end_index_ ? section_begin_index_ : section_end_index_;
	doc_view_ctrl_->insert_structured_context(pDC, structured_section_context_, _section_begin_index, _section_begin_pgh);
	doc_view_ctrl_->reset_selection(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_);
	doc_view_ctrl_->draw_complete(pDC);

	delete structured_section_context_;
	structured_section_context_ = nullptr;
	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}

LxSectionWrapCmd::LxSectionWrapCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh)
	: section_begin_index_(section_begin_index), section_end_index_(section_end_index),
	section_begin_pgh_(section_begin_pgh), section_end_pgh_(section_end_pgh),
	structured_section_context_(nullptr)
{
}
LxSectionWrapCmd::~LxSectionWrapCmd()
{
	if (structured_section_context_)
	{
		delete structured_section_context_;
	}
}
void LxSectionWrapCmd::Excute(CDC* pDC)
{
	if (!structured_section_context_)
	{
		structured_section_context_ = new StructuredSectionContext();
	}
	doc_view_ctrl_->section_wrap(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_, structured_section_context_);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxSectionWrapCmd::Undo(CDC* pDC)
{
	size_t _section_begin_pgh = section_begin_pgh_ < section_end_pgh_ ? section_begin_pgh_ : section_end_pgh_;
	size_t _section_begin_index = section_begin_index_ < section_end_index_ ? section_begin_index_ : section_end_index_;
	LxMergeCmd merge_cmd(_section_begin_pgh + 1);
	merge_cmd.set_dvctl(doc_view_ctrl_);
	merge_cmd.Excute(pDC);
	doc_view_ctrl_->insert_structured_context(pDC, structured_section_context_, _section_begin_index, _section_begin_pgh);
	doc_view_ctrl_->reset_selection(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_);
	doc_view_ctrl_->draw_complete(pDC);

	delete structured_section_context_;
	structured_section_context_ = nullptr;
	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
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
void LxSectionReplaceCmd::Undo(CDC* pDC)
{

}

LxModifyFontCmd::LxModifyFontCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh, size_t src_font)
	: section_begin_index_(section_begin_index), section_end_index_(section_end_index), src_font_(src_font),
	section_begin_pgh_(section_begin_pgh), section_end_pgh_(section_end_pgh), font_contex_(nullptr)
{
}
LxModifyFontCmd::~LxModifyFontCmd()
{
	if (font_contex_)
		delete font_contex_;
}
void LxModifyFontCmd::Excute(CDC* pDC)
{
	if (!font_contex_)
	{
		font_contex_ = new StructuredSrcContext();
		doc_view_ctrl_->record_section_font_info(font_contex_, section_begin_index_, section_end_index_);
	}
	doc_view_ctrl_->modify_section_font(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_, src_font_);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxModifyFontCmd::Undo(CDC* pDC)
{
	doc_view_ctrl_->modify_section_font(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_, font_contex_);
	doc_view_ctrl_->draw_complete(pDC);
	doc_view_ctrl_->calc_font_color();
}

LxModifyColorCmd::LxModifyColorCmd(size_t section_begin_index, size_t section_begin_pgh, size_t section_end_index, size_t section_end_pgh, COLORREF src_color)
	: section_begin_index_(section_begin_index), section_begin_pgh_(section_begin_pgh), section_end_index_(section_end_index), section_end_pgh_(section_end_pgh),
	src_color_(src_color), color_contex_(nullptr)
{
}
LxModifyColorCmd::~LxModifyColorCmd()
{
	if (color_contex_)
		delete color_contex_;
}
void LxModifyColorCmd::Excute(CDC* pDC)
{
	if (!color_contex_)
	{
		color_contex_ = new StructuredSrcContext();
		doc_view_ctrl_->record_section_color_info(color_contex_, section_begin_index_, section_end_index_);
	}
	doc_view_ctrl_->modify_section_color(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_, src_color_);
	doc_view_ctrl_->draw_complete(pDC);

	ASSERT(doc_view_ctrl_->self_check());
	doc_view_ctrl_->calc_font_color();
}
void LxModifyColorCmd::Undo(CDC* pDC)
{
	doc_view_ctrl_->modify_structured_color_context(color_contex_);
	doc_view_ctrl_->reset_selection(pDC, section_begin_index_, section_begin_pgh_, section_end_index_, section_end_pgh_);
	doc_view_ctrl_->draw_complete(pDC);
	doc_view_ctrl_->calc_font_color();
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
void LxCommand::Undo(CDC* pDC)
{
	if (CanUndo())
	{
		auto rit = command.rbegin();
		auto rite = command.rend();
		for (; rit != rite; ++rit)
		{
			(*(rit))->Undo(pDC);
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
	save_point_ = empty_cmd;
}
LxCommandMgr::~LxCommandMgr()
{
	for (auto it : command_list)
		delete it;
	command_list.clear();
}
void LxCommandMgr::set_curr_as_savepoint()
{
	if (curr_ == command_list.begin())
		return;
	save_point_ = *curr_;
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
	save_point_ = empty_cmd;
}
bool LxCommandMgr::changed()
{
	return *curr_ != save_point_;
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
		if (save_point_ == *it)
			save_point_ = command_list.front();
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