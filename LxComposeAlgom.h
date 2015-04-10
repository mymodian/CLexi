#include "stdafx.h"

#ifndef __LEXI_COMPOSE_ALGOM_H
#define __LEXI_COMPOSE_ALGOM_H

#include "LxTreeBase.h"
#include "LxComposeDoc.h"
#include "LxCursor.h"
#include "LxCommon.h"

//因为根据命令的情况可能会部分排版或全部排版，因此需要部分排版和全部排版
//  <--？？？     考虑到排版算法的独立性，那么一个通用的排版算法需要怎样的接口？      ？？？ -->

//asw: 输入：当前Page&pos_session
//       返回：排版完成的paragraph和List<Page>(表示段所跨越的页)。

/*
	整段的排版和部分排版
*/

class LxComposeAlgom
{
public:
	LxComposeAlgom() = default;
	virtual ~LxComposeAlgom() = default;
	
	//这两个函数不该在排版算法中，而应该在真正的排版实现中
	virtual void compose() = 0;
	virtual void layout() = 0;
	//整段的排版
	virtual ComposeParagraph* compose(int offset_y, size_t pagra_index_begin, Paragraph* pagraph, TreeBase* font_tree, CDC* pDC) = 0;
	//段落的部分排版,执行插入操作后的排版算法
	virtual void compose(LxCursor cursor) = 0;
	virtual void compose(ComposeRow* row_to_compose, Paragraph* pagraph, size_t& index_begin, size_t& index_inner, TreeBase* font_tree, CDC* pDC) = 0;
};

class LxSimpleComposeAlgo : public LxComposeAlgom
{
public:
	LxSimpleComposeAlgo();
	virtual ~LxSimpleComposeAlgo();
	virtual void compose() override;
	virtual void layout() override;
	virtual ComposeParagraph* compose(int offset_y, size_t pagra_index_begin, Paragraph* pagraph, TreeBase* font_tree, CDC* pDC) override;
	virtual void compose(LxCursor cursor) override;
	virtual void compose(ComposeRow* row_to_compose, Paragraph* pagraph, size_t& index_begin, size_t& index_inner, TreeBase* font_tree, CDC* pDC) override;
};

class ComposeAlgoFactory
{
public:
	~ComposeAlgoFactory()
	{
		for (auto algo : algoms)
			delete algo;
	}
	LxComposeAlgom* get_compose_algom(ComposeAlgoType type)
	{
		return algoms[type];
	}
public:
	static ComposeAlgoFactory* GetComposeAlgoFactInstance()
	{
		if (lpComposeAlgoFactoryInstance == nullptr)
			lpComposeAlgoFactoryInstance = new ComposeAlgoFactory();
		return lpComposeAlgoFactoryInstance;
	}
private:
	static ComposeAlgoFactory* lpComposeAlgoFactoryInstance;
	LxComposeAlgom* algoms[1];
private:
	ComposeAlgoFactory()
	{
		algoms[0] = new LxSimpleComposeAlgo();
	}
	ComposeAlgoFactory(const ComposeAlgoFactory&) = delete;
	ComposeAlgoFactory& operator=(const ComposeAlgoFactory&) = delete;
};

#endif