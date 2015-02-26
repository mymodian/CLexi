/**
所有的command的Execute实现现在都依赖于当前cursor的位置，如果要实现undo和redo应该修改为与当前光标位置无关
Document实现add_paragraph（paragraph）
ComposeDoc实现 LxParagraphInDocIter add_paragraph(Paragraph* pgh, LxParagraphInDocIter pos, int before_after)
{
  add
  1.先计算出插入的段的 index, global_index, top_pos
  2.生成段,插入
  3.modify_index(pgh->size());
  4.对段进行排版
  5.排版过程中如果超出当前页,将页之后的段移入下一页
  6.排版完成返回
 }
 splitcmd exec
 1.modify_index(calc);
 2.modify(LxParagraphInDocIter);
 3.add_paragraph();
*/