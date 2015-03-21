/**
Attention!! 注意许多操作不仅仅会改变排版文档，还会改变 font_trrr 和 color_tree.
next to do:
	1.无选择区域的情况下的删除操作						done
		1.1删除某一个物理段                                     done
		1.2将某个物理段与她之前的物理段合并       done
	2.完成保存和打开功能                                        have the solution
	3.添加滚动条														 done
	4.未选择区域下的字体和颜色设置						 done
	5.选择区域的显示(物理变化已实现)					 next to do
	6.选择区域下的字体和颜色设置						 refactoring with 4
*/



/**
所有的command的Execute实现现在都依赖于当前cursor的位置，如果要实现undo和redo应该修改为与当前光标位置无关
Document实现add_paragraph（paragraph）
ComposeDoc实现 LxParagraphInDocIter add_paragraph(Paragraph* pgh, LxParagraphInDocIter pos, int before_after)
{
  add
  1.先计算出插入的段的 index, global_index, top_pos
  2.生成段,插入
  3.对段进行排版
  4.排版过程中如果超出当前页,将页之后的段移入下一页
  5.排版完成
  6.modify_index(pgh->size());返回
 }
 splitcmd exec
 1.modify_index(calc);
 2.modify(LxParagraphInDocIter);
 3.add_paragraph();
*/
