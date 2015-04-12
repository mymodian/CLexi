#include "stdafx.h"
#include <cassert>
#include "LxTreeBase.h"
#include "LxCommon.h"

TreeBase::TreeBase() : src_head_(NULL), src_tail_(NULL)
{
	root_ = new node_branch();
}
TreeBase::~TreeBase()
{
	clear();
}

void TreeBase::insert(size_t position, size_t  count)
{
	size_t offset = 0;
	position += 1;
	node_leaf* _dst = find(position, root_, offset);
	if (_dst == NULL) return;
	if (offset == 1 && _dst->left)
		_dst = _dst->left;
	_dst->count += count;
	tune_up(_dst->parent, count);
}
void TreeBase::insert(size_t position, size_t  count, size_t src_index)
{
	size_t offset = 0;
	position += 1;
	node_leaf* _dst = find(position, root_, offset);
	if (_dst == NULL)
	{
		src_head_ = src_tail_ = new node_leaf();
		src_head_->parent = root_;
		root_->childs.push_back(src_head_);
		src_head_->src_index = src_index;
		src_head_->count = count;
		tune_up(src_head_->parent, count);
		return;
	}
	//处理新加的节点，合并，或新加（可能会产生分裂）
	//offset==1    offset==cnt+1     offset==2~cnt
	if (offset == 1)
	{
		if (_dst->left != NULL)
		{
			_dst = _dst->left;
			if (src_index == _dst->src_index)
			{
				_dst->count += count;
				tune_up(_dst->parent, count);
				return;
			}
			if (src_index == _dst->right->src_index)
			{
				_dst->right->count += count;
				tune_up(_dst->right->parent, count);
				return;
			}
			//新插入
			node_leaf* new_leaf = new node_leaf();
			new_leaf->src_index = src_index;
			new_leaf->left = _dst;
			new_leaf->right = _dst->right;
			_dst->right->left = new_leaf;
			_dst->right = new_leaf;
			new_leaf->parent = _dst->parent;
			//add to parent's childs
			add_child_after(new_leaf->parent, _dst, new_leaf);
			new_leaf->count = count;
			tune_up(new_leaf->parent, count);
			if (new_leaf->parent->childs.size() > MAX_CHILD_INDEX)
				split(new_leaf->parent);
			return;
		}
		if (src_index == _dst->src_index)
		{
			_dst->count += count;
			tune_up(_dst->parent, count);
			return;
		}
		node_leaf* new_leaf = new node_leaf();
		new_leaf->src_index = src_index;
		new_leaf->right = _dst;
		_dst->left = new_leaf;
		src_head_ = new_leaf;
		new_leaf->parent = _dst->parent;
		_dst->parent->childs.push_front(new_leaf);
		new_leaf->count = count;
		tune_up(new_leaf->parent, count);
		if (new_leaf->parent->childs.size() > MAX_CHILD_INDEX)
			split(new_leaf->parent);
		return;
	}
	if (offset >= _dst->count + 1)
	{
		if (src_index == _dst->src_index)
		{
			_dst->count += count;
			tune_up(_dst->parent, count);
			return;
		}
		node_leaf* new_leaf = new node_leaf();
		new_leaf->src_index = src_index;
		new_leaf->left = _dst;
		_dst->right = new_leaf;
		src_tail_ = new_leaf;
		new_leaf->parent = _dst->parent;
		_dst->parent->childs.push_back(new_leaf);
		new_leaf->count = count;
		tune_up(new_leaf->parent, count);
		if (new_leaf->parent->childs.size() > MAX_CHILD_INDEX)
			split(new_leaf->parent);
		return;
	}
	if (src_index == _dst->src_index)
	{
		_dst->count += count;
		tune_up(_dst->parent, count);
		return;
	}
	//插入导致原有节点被分割
	node_leaf* _dst_part2 = new node_leaf();
	node_leaf* new_leaf = new node_leaf();
	_dst_part2->src_index = _dst->src_index;
	new_leaf->src_index = src_index;
	_dst_part2->count = _dst->count + 1 - offset;
	_dst->count = offset - 1;
	if (_dst->right != NULL)
	{
		_dst->right->left = _dst_part2;
		_dst_part2->right = _dst->right;
		src_tail_ = _dst_part2;
	}
	_dst->right = new_leaf;
	new_leaf->left = _dst;
	new_leaf->right = _dst_part2;
	_dst_part2->left = new_leaf;
	new_leaf->parent = _dst->parent;
	_dst_part2->parent = _dst->parent;
	add_child_after(_dst->parent, _dst, _dst_part2);
	add_child_after(_dst->parent, _dst, new_leaf);
	new_leaf->count = count;
	tune_up(new_leaf->parent, count);
	if (new_leaf->parent->childs.size() > MAX_CHILD_INDEX)
		split(new_leaf->parent);
	//判断是否需要分裂
}
//暂时用来测试删除合并  test success
void TreeBase::remove(size_t position)
{
	size_t offset;
	position += 1;
	node_leaf* leaf = find(position, root_, offset);
	delete_leaf(leaf);
}
void TreeBase::remove(size_t position_begin, size_t position_end)
{
	if (position_begin >= position_end) return;
	size_t offset_l = 0, offset_r = 0;
	position_begin += 1;
	position_end += 1;
	node_leaf* _dst_l = find(position_begin, root_, offset_l);
	node_leaf* _dst_r = find(position_end, root_, offset_r);
	if (_dst_l == NULL || _dst_r == NULL) return;

	node_leaf* left_compare = NULL;
	if (_dst_l == _dst_r)
	{
		if (offset_r - offset_l == _dst_l->count)
		{
			left_compare = _dst_l->left;
			delete_leaf(_dst_l);
		}
		else
		{
			_dst_l->count -= offset_r - offset_l;
			tune_down(_dst_l->parent, offset_r - offset_l);
			left_compare = _dst_l;
		}
	}
	else
	{
		node_leaf* next = _dst_l->right;

		if (offset_l == 1)
		{
			left_compare = _dst_l->left;
			delete_leaf(_dst_l);
		}
		else
		{
			tune_down(_dst_l->parent, _dst_l->count + 1 - offset_l);
			_dst_l->count = offset_l - 1;
			left_compare = _dst_l;
		}

		while (next != _dst_r)
		{
			node_leaf* tmp = next;
			next = next->right;
			delete_leaf(tmp);
		}

		if (offset_r == _dst_r->count + 1)
			delete_leaf(_dst_r);
		else
		{
			tune_down(_dst_r->parent, offset_r - 1);
			_dst_r->count -= offset_r - 1;
		}
	}
	if (left_compare != NULL && left_compare->right != NULL
		&& left_compare->src_index == left_compare->right->src_index)
	{
		left_compare->count += left_compare->right->count;
		tune_up(left_compare->parent, left_compare->right->count);
		delete_leaf(left_compare->right);
	}
}
void TreeBase::modify(size_t position_begin, size_t position_end, size_t src_index)
{
	if (position_begin >= position_end) return;
	size_t offset_l = 0, offset_r = 0;
	position_begin += 1;
	position_end += 1;
	node_leaf* _dst_l = find(position_begin, root_, offset_l);
	node_leaf* _dst_r = find(position_end, root_, offset_r);
	if (_dst_l == NULL || _dst_r == NULL) return;

	//在一个节点的中间改变字体
	//1. tune_down 两次insert
	if (_dst_l == _dst_r)
	{
		if (_dst_l->src_index == src_index) return;
		else if (offset_l == 1)
		{
			if (_dst_l->left != NULL && _dst_l->left->src_index == src_index)
			{
				_dst_l->count -= offset_r - offset_l;
				_dst_l->left->count += offset_r - offset_l;
				if (_dst_l->parent != _dst_l->left->parent)
				{
					tune_down(_dst_l->parent, offset_r - offset_l);
					tune_up(_dst_l->left->parent, offset_r - offset_l);
				}
				if (_dst_l->count == 0)
					delete_leaf(_dst_l);
			}
			else
			{
				if (_dst_l->count == offset_r - offset_l)
					_dst_l->src_index = src_index;
				else
				{
					_dst_l->count -= offset_r - offset_l;
					node_leaf* new_leaf = new node_leaf();
					new_leaf->src_index = src_index;
					new_leaf->count = offset_r - offset_l;
					new_leaf->parent = _dst_l->parent;
					new_leaf->right = _dst_l;
					new_leaf->left = _dst_l->left;
					if (_dst_l->left != NULL)
					{
						_dst_l->left->right = new_leaf;
						add_child_before(_dst_l->parent, _dst_l, new_leaf);
					}
					else
					{
						src_head_ = new_leaf;
						_dst_l->parent->childs.push_front(new_leaf);
					}
					_dst_l->left = new_leaf;
					if (new_leaf->parent->childs.size() > MAX_CHILD_INDEX)
						split(new_leaf->parent);
				}
			}
		}
		else
		{
			if (_dst_l->count + 1 - offset_r != 0)
			{
				node_leaf* new_leaf_r = new node_leaf();
				new_leaf_r->src_index = _dst_l->src_index;
				new_leaf_r->count = _dst_l->count + 1 - offset_r;
				_dst_l->count = offset_r - 1;
				new_leaf_r->parent = _dst_l->parent;
				new_leaf_r->left = _dst_l;
				if (_dst_l->right != NULL)
				{
					_dst_l->right->left = new_leaf_r;
					new_leaf_r->right = _dst_l->right;
					_dst_l->right = new_leaf_r;
					add_child_after(_dst_l->parent, _dst_l, new_leaf_r);
				}
				else
				{
					_dst_l->right = new_leaf_r;
					src_tail_ = new_leaf_r;
					_dst_l->parent->childs.push_back(new_leaf_r);
				}
				if (_dst_l->parent->childs.size() > MAX_CHILD_INDEX)
					split(_dst_l->parent);
			}

			node_leaf* new_leaf_m = new node_leaf();
			new_leaf_m->src_index = src_index;
			new_leaf_m->parent = _dst_l->parent;
			new_leaf_m->count = _dst_l->count + 1 - offset_l;
			_dst_l->count = offset_l - 1;
			new_leaf_m->left = _dst_l;
			new_leaf_m->right = _dst_l->right;
			if (_dst_l->right) _dst_l->right->left = new_leaf_m;
			_dst_l->right = new_leaf_m;
			add_child_after(_dst_l->parent, _dst_l, new_leaf_m);
			if (_dst_l->parent->childs.size() > MAX_CHILD_INDEX)
				split(_dst_l->parent);
		}
	}
	else
	{
		//左右指向不同的节点，先处理最左边的节点
		node_leaf* join = _dst_l;
		node_leaf* next = _dst_l->right;
		size_t added = 0;
		if (offset_l == 1)
		{
			if (_dst_l->left != NULL && _dst_l->left->src_index == src_index)
			{
				join = _dst_l->left;
				added += _dst_l->count;
				delete_leaf(_dst_l);
			}
			else
				_dst_l->src_index = src_index;
		}
		else
		{
			if (_dst_l->src_index != src_index)
			{
				node_leaf* new_leaf = new node_leaf();
				new_leaf->src_index = src_index;
				new_leaf->count = _dst_l->count + 1 - offset_l;
				_dst_l->count = offset_l - 1;
				new_leaf->parent = _dst_l->parent;
				new_leaf->left = _dst_l;
				new_leaf->right = _dst_l->right;
				_dst_l->right->left = new_leaf;
				_dst_l->right = new_leaf;
				add_child_after(_dst_l->parent, _dst_l, new_leaf);
				if (_dst_l->parent->childs.size() > MAX_CHILD_INDEX)
					split(_dst_l->parent);
				join = new_leaf;
			}
		}

		node_leaf* temp;
		while (next != _dst_r)
		{
			temp = next;
			next = next->right;
			added += temp->count;
			delete_leaf(temp);
		}

		if (offset_r != 1)
		{
			if (offset_r == _dst_r->count + 1 || _dst_r->src_index == src_index)
			{
				added += _dst_r->count;
				delete_leaf(_dst_r);
			}
			else
			{
				_dst_r->count -= offset_r - 1;
				added += offset_r - 1;
				tune_down(_dst_r->parent, offset_r - 1);
			}
		}
		else
		{
			if (_dst_r->src_index == src_index)
			{
				added += _dst_r->count;
				delete_leaf(_dst_r);
			}
		}

		join->count += added;
		tune_up(join->parent, added);
		//将中间节点数量加到最左边处理节点，然后依次删除中间节点
		//然后处理最右边的节点
	}
	//一个节点中间和另一个节点中间
	//判断是否分离出新节点
}
size_t TreeBase::get_src_index(size_t position)
{
	size_t offset = 0;
	position += 1;
	node_leaf* _dst = find(position, root_, offset);

	if (_dst == NULL) return 0 - 1;

	if (offset == 1 && _dst->left != NULL)
		_dst = _dst->left;
	return _dst->src_index;
}
void TreeBase::get_src_index(size_t position, size_t& src_index, size_t& last_cnt)
{
	size_t offset = 0;
	position += 1;
	node_leaf* _dst = find(position, root_, offset);

	if (_dst == NULL)
	{
		src_index = 0 - 1;
		return;
	}

	src_index = _dst->src_index;
	last_cnt = _dst->count + 1 - offset;
}

//protected
// 如果position比某个节点的序号刚好大一，那么结果转移到其右兄弟(如果存在，否则是自身)
node_leaf* TreeBase::find(size_t position, node_branch* root, size_t& offset)
{
	ASSERT(root != NULL);
	if (root->childs.empty()) return NULL;

	auto it = root->childs.begin();
	auto end = root->childs.end();
	size_t ori = position;
	if (!root->bLeaf)
	{
		for (; it != end; it++)
		{
			if (ori > ((node_branch*)(*it))->count)
				ori -= ((node_branch*)(*it))->count;
			else break;
		}
		if (it == end) { it--; ori += ((node_branch*)(*it))->count; }
		offset = ori;
		return find(ori, (node_branch*)(*it), offset);
	}
	else
	{
		for (; it != end; it++)
		{
			if (ori > ((node_leaf*)(*it))->count)
				ori -= ((node_leaf*)(*it))->count;
			else break;
		}
		if (it == end) { it--; ori += ((node_leaf*)(*it))->count; }
		offset = ori;
		return (node_leaf*)(*it);
	}
}
void TreeBase::tune_up(node_branch* parent, size_t count)
{
	while (parent != NULL)
	{
		parent->count += count;
		parent = parent->parent;
	}
}
void TreeBase::tune_down(node_branch* parent, size_t count)
{
	while (parent != NULL)
	{
		parent->count -= count;
		parent = parent->parent;
	}
}
bool TreeBase::add_child_before(node_branch* parent, void* right_brother, void* to_added)
{
	auto it = parent->childs.begin();
	auto end = parent->childs.end();
	for (; it != end; it++)
	{
		if (*it == right_brother)
		{
			parent->childs.insert(it, to_added);
			return true;
		}
	}
	return false;
}
bool TreeBase::add_child_after(node_branch* parent, void* left_brother, void* to_added)
{
	auto it = parent->childs.begin();
	auto end = parent->childs.end();
	for (; it != end; it++)
	{
		if (*it == left_brother)
		{
			it++;
			parent->childs.insert(it, to_added);
			return true;
		}
	}
	return false;
}
void TreeBase::get_brother_branch(node_branch* branch, node_branch* &left_brother, node_branch* &right_brother)
{
	node_branch* parent = branch->parent;
	if (parent == NULL)
	{
		left_brother = right_brother = NULL;
		return;
	}
	auto it = parent->childs.begin();
	auto end = parent->childs.end();
	for (; it != end; it++)
		if (*it == branch) break;
	if (it == end) { left_brother = right_brother = NULL; return; }
	if (it == parent->childs.begin())
	{
		left_brother = NULL;
		if (parent->childs.size() > 1)
			right_brother = (node_branch*)(*(++it));
		else
			right_brother = NULL;
		return;
	}
	if (it == --end)
	{
		if (parent->childs.size() > 1)
			left_brother = (node_branch*)(*(--it));
		else left_brother = NULL;
		right_brother = NULL;
		return;
	}
	left_brother = (node_branch*)(*(--it));
	it++;
	right_brother = (node_branch*)(*(++it));
}
void TreeBase::shared_with_left(node_branch* parent, node_branch* left_brother)
{
	void* first_child = parent->childs.front();
	if (parent->bLeaf)
	{
		tune_down(parent, ((node_leaf*)first_child)->count);
		tune_up(left_brother, ((node_leaf*)first_child)->count);
		((node_leaf*)first_child)->parent = left_brother;
	}
	else
	{
		tune_down(parent, ((node_branch*)first_child)->count);
		tune_up(left_brother, ((node_branch*)first_child)->count);
		((node_branch*)first_child)->parent = left_brother;
	}
	parent->childs.pop_front();
	left_brother->childs.push_back(first_child);
}
void TreeBase::shared_with_right(node_branch* parent, node_branch* right_brother)
{
	void* last_child = parent->childs.back();
	if (parent->bLeaf)
	{
		tune_down(parent, ((node_leaf*)last_child)->count);
		tune_up(right_brother, ((node_leaf*)last_child)->count);
		((node_leaf*)last_child)->parent = right_brother;
	}
	else
	{
		tune_down(parent, ((node_branch*)last_child)->count);
		tune_up(right_brother, ((node_branch*)last_child)->count);
		((node_branch*)last_child)->parent = right_brother;
	}
	parent->childs.pop_back();
	right_brother->childs.push_front(last_child);
}
void TreeBase::shared_with_new_brother(node_branch* left_brother, node_branch* right_brother)
{
	node_branch* new_brother = new node_branch();
	new_brother->parent = left_brother->parent;
	new_brother->bLeaf = left_brother->bLeaf;
	//分割左右兄弟的1/3子节点给新兄弟
	size_t left_partioned_cnt = 0, right_partioned_cnt = 0;
	if (left_brother->bLeaf)
	{
		node_leaf* child;
		for (int i = 0; i < left_brother->childs.size() / 3; i++)
		{
			child = (node_leaf*)left_brother->childs.back();
			left_partioned_cnt += child->count;
			child->parent = new_brother;
			new_brother->childs.push_front(child);
			left_brother->childs.pop_back();
		}
		for (int i = 0; i < right_brother->childs.size() / 3; i++)
		{
			child = (node_leaf*)right_brother->childs.front();
			right_partioned_cnt += child->count;
			child->parent = new_brother;
			new_brother->childs.push_back(child);
			right_brother->childs.pop_front();
		}
	}
	else
	{
		node_branch* child;
		for (int i = 0; i < left_brother->childs.size() / 3; i++)
		{
			child = (node_branch*)left_brother->childs.back();
			left_partioned_cnt += child->count;
			child->parent = new_brother;
			new_brother->childs.push_front(child);
			left_brother->childs.pop_back();
		}
		for (int i = 0; i < right_brother->childs.size() / 3; i++)
		{
			child = (node_branch*)right_brother->childs.front();
			right_partioned_cnt += child->count;
			child->parent = new_brother;
			new_brother->childs.push_back(child);
			right_brother->childs.pop_front();
		}
	}
	tune_down(left_brother, left_partioned_cnt);
	tune_down(right_brother, right_partioned_cnt);
	new_brother->count = left_partioned_cnt + right_partioned_cnt;
	add_child_after(left_brother->parent, left_brother, new_brother);
	tune_up(left_brother->parent, left_partioned_cnt + right_partioned_cnt);
	//递归检查父节点是否需要分裂
	if (left_brother->parent->childs.size() > MAX_CHILD_INDEX)
		split(left_brother->parent);
}

void TreeBase::split(node_branch* parent)
{
	//ASSERT(parent->childs.size() > MAX_CHILD_INDEX);
	//如果父节点为根，直接分裂，结束
	if (parent == root_)
	{
		node_branch* new_child1 = new node_branch();
		node_branch* new_child2 = new node_branch();
		new_child1->bLeaf = parent->bLeaf;
		new_child2->bLeaf = parent->bLeaf;
		new_child1->parent = parent;
		new_child2->parent = parent;

		auto it = parent->childs.begin();
		for (int i = 0; i < parent->childs.size() / 2; i++)
		{
			new_child1->childs.push_back(*it);
			if (parent->bLeaf)
			{
				((node_leaf*)(*it))->parent = new_child1;
				new_child1->count += ((node_leaf*)(*it))->count;
			}
			else
			{
				((node_branch*)(*it))->parent = new_child1;
				new_child1->count += ((node_branch*)(*it))->count;
			}
			it++;
		}
		for (; it != parent->childs.end(); it++)
		{
			new_child2->childs.push_back(*it);
			if (parent->bLeaf)
			{
				((node_leaf*)(*it))->parent = new_child2;
				new_child2->count += ((node_leaf*)(*it))->count;
			}
			else
			{
				((node_branch*)(*it))->parent = new_child2;
				new_child2->count += ((node_branch*)(*it))->count;
			}
		}
		parent->bLeaf = false;
		parent->childs.clear();
		parent->childs.push_back(new_child1);
		parent->childs.push_back(new_child2);
		return;
	}
	//查看左右兄弟节点，看是否可以和兄弟节点共享
	node_branch* left_brother, *right_brother;
	get_brother_branch(parent, left_brother, right_brother);
	if (left_brother != NULL && right_brother == NULL)
	{
		if (left_brother->childs.size() < MAX_CHILD_INDEX)
		{
			//左兄弟有空间
			shared_with_left(parent, left_brother);
			return;
		}
		//左兄弟没有空间，新建节点并获得他们各自1/3的子节点
		shared_with_new_brother(left_brother, parent);
		return;
	}
	else if (left_brother == NULL && right_brother != NULL)
	{
		if (right_brother->childs.size() < MAX_CHILD_INDEX)
		{
			//右兄弟有空间
			shared_with_right(parent, right_brother);
			return;
		}
		//右兄弟没有空间，新建节点并获得他们各自1/3的子节点
		shared_with_new_brother(parent, right_brother);
		return;
	}
	else
	{
		//左右兄弟都存在
		if (left_brother->childs.size() < MAX_CHILD_INDEX)
		{
			shared_with_left(parent, left_brother);
			return;
		}
		else if (right_brother->childs.size() < MAX_CHILD_INDEX)
		{
			shared_with_right(parent, right_brother);
			return;
		}
		else
		{
			//左右兄弟均满，优先选择与右兄弟分割
			shared_with_new_brother(parent, right_brother);
		}
	}
	//无法共享，在父节点创建新节点，并分担左右两个兄弟各自1/3的节点
}

void TreeBase::remove_child(node_branch* parent, void* child)
{
	auto it = parent->childs.begin();
	auto end = parent->childs.end();
	for (; it != end; it++)
		if (*it == child)
		{
		parent->childs.erase(it);
		return;
		}
}
void TreeBase::delete_leaf(node_leaf* leaf)
{
	if (leaf == src_head_) src_head_ = leaf->right;
	if (leaf == src_tail_) src_tail_ = leaf->left;
	if (leaf->left)
		leaf->left->right = leaf->right;
	if (leaf->right)
		leaf->right->left = leaf->left;
	tune_down(leaf->parent, leaf->count);
	remove_child(leaf->parent, leaf);
	node_branch* parent = leaf->parent;
	delete leaf;
	if (parent->childs.size() < MIN_CHILD_INDEX)
		merge(parent);
}
void TreeBase::merge_with_left(node_branch* parent, node_branch* left_brother)
{
	int shared_cnt = (left_brother->childs.size() - parent->childs.size()) / 2;
	if (shared_cnt == 0) shared_cnt = 1;
	void* child;
	size_t total_cnt = 0;
	for (; shared_cnt > 0; shared_cnt--)
	{
		child = left_brother->childs.back();
		parent->childs.push_front(child);
		if (parent->bLeaf)
		{
			((node_leaf*)child)->parent = parent;
			total_cnt += ((node_leaf*)child)->count;
		}
		else
		{
			((node_branch*)child)->parent = parent;
			total_cnt += ((node_branch*)child)->count;
		}
		left_brother->childs.pop_back();
	}
	tune_down(left_brother, total_cnt);
	tune_up(parent, total_cnt);
}
void TreeBase::merge_with_right(node_branch* parent, node_branch* right_brother)
{
	int shared_cnt = (right_brother->childs.size() - parent->childs.size()) / 2;
	if (shared_cnt == 0) shared_cnt = 1;
	void* child;
	size_t total_cnt = 0;
	for (; shared_cnt > 0; shared_cnt--)
	{
		child = right_brother->childs.front();
		parent->childs.push_back(child);
		if (parent->bLeaf)
		{
			((node_leaf*)child)->parent = parent;
			total_cnt += ((node_leaf*)child)->count;
		}
		else
		{
			((node_branch*)child)->parent = parent;
			total_cnt += ((node_branch*)child)->count;
		}
		right_brother->childs.pop_front();
	}
	tune_down(right_brother, total_cnt);
	tune_up(parent, total_cnt);
}
void TreeBase::combine(node_branch* left, node_branch* right)
{
	left->count += right->count;

	void* child;
	int cnt = right->childs.size();
	for (int i = 0; i < cnt; i++)
	{
		child = right->childs.front();
		left->childs.push_back(child);
		if (left->bLeaf) ((node_leaf*)child)->parent = left;
		else ((node_branch*)child)->parent = left;
		right->childs.pop_front();
	}
	remove_child(right->parent, right);
	delete right;
	if (left->parent->childs.size() < MIN_CHILD_INDEX)
		merge(left->parent);
}
void TreeBase::merge(node_branch* parent)
{
	if (parent == root_)
	{
		if (parent->childs.size() == 1 && !parent->bLeaf)
		{
			node_branch* root_old = root_;
			root_ = (node_branch*)root_old->childs.front();
			root_->parent = NULL;
			delete root_old;
		}
		return;
	}
	//查看左右兄弟节点，看是否可以从兄弟节点获取填充至MIN_CHILD
	node_branch* left_brother, *right_brother;
	get_brother_branch(parent, left_brother, right_brother);
	if (left_brother != NULL && right_brother == NULL)
	{
		if (left_brother->childs.size() > MIN_CHILD_INDEX)
		{
			//左兄弟有多余子节点
			merge_with_left(parent, left_brother);
			return;
		}
		//左兄弟没有多余，与左兄弟合并
		combine(left_brother, parent);
		return;
	}
	else if (left_brother == NULL && right_brother != NULL)
	{
		if (right_brother->childs.size() > MIN_CHILD_INDEX)
		{
			//右兄弟有多余子节点
			merge_with_right(parent, right_brother);
			return;
		}
		//右兄弟没有多余，与右兄弟合并
		combine(parent, right_brother);
		return;
	}
	else if (left_brother == NULL && right_brother == NULL) return;
	else
	{
		//左右兄弟都存在
		if (left_brother->childs.size() > MIN_CHILD_INDEX)
		{
			merge_with_left(parent, left_brother);
			return;
		}
		else if (right_brother->childs.size() > MIN_CHILD_INDEX)
		{
			merge_with_right(parent, right_brother);
			return;
		}
		else
		{
			//左右兄弟无多余，优先选择与左兄弟合并
			combine(left_brother, parent);
		}
	}
}

void TreeBase::store_stream(FILE* file)
{
	if (!root_) return;
	store_stream_int(file, root_->count);
	node_leaf* head = src_head_;
	while (head != NULL)
	{
		store_stream_int(file, head->count);
		store_stream_int(file, head->src_index);
		head = head->right;
	}
}
void TreeBase::build_from_stream(FILE* file)
{
	clear();
	root_ = new node_branch();
	int total = read_stream_int(file);
	int position = 0;
	while (position != total)
	{
		int count = read_stream_int(file);
		int src_index = read_stream_int(file);
		insert(position, count, src_index);
		position += count;
	}
}

void TreeBase::get_src_list_still_using(std::set<size_t>& font_list_still_using)
{
	node_leaf* head = src_head_;
	while (head != NULL)
	{
		font_list_still_using.insert(head->src_index);
		head = head->right;
	}
}
void TreeBase::print_leafs()
{
	node_leaf* head = src_head_;
	while (head != NULL)
	{
		cout << "->" << head->count << ',' << head->src_index;
		head = head->right;
	}
	cout << endl;
}
void TreeBase::print_hole_tree()
{
	vector<int> IsEnd;
	show(root_, 0, IsEnd);
}
void TreeBase::show(node_branch* parent, int whitspace, vector<int>& IsEnd)
{
	if (parent == NULL) return;
	if (parent->bLeaf)
	{
		showleafchilds(parent, whitspace, IsEnd);
		return;
	}
	cout << parent->count << endl;

	auto it = parent->childs.begin();
	int i = 0;
	for (; it != parent->childs.end(); it++)
	{
		i++;
		vector<int>::iterator it1 = IsEnd.begin(), it2 = IsEnd.end();
		for (int s = 0; s < whitspace && it1 != it2; s++, it1++)
		{
			if (*it1 == 0)
				cout << "   ";
			else
				cout << "│ ";
		}
		if (i < parent->childs.size()) { cout << "├─"; IsEnd.push_back(1); }
		else { cout << "└─"; IsEnd.push_back(0); }
		show((node_branch*)(*it), whitspace + 1, IsEnd);
		IsEnd.pop_back();
	}
}
void TreeBase::showleafchilds(node_branch* parent, int whitspace, vector<int>& IsEnd)
{
	if (parent == NULL) return;
	cout << parent->count << endl;

	auto it = parent->childs.begin();
	int i = 0;
	for (; it != parent->childs.end(); it++)
	{
		i++;
		vector<int>::iterator it1 = IsEnd.begin(), it2 = IsEnd.end();
		for (int s = 0; s < whitspace && it1 != it2; s++, it1++)
		{
			if (*it1 == 0)
				cout << "   ";
			else
				cout << "│ ";
		}
		if (i < parent->childs.size()) { cout << "├─"; IsEnd.push_back(1); }
		else { cout << "└─"; IsEnd.push_back(0); }
		cout << ((node_leaf*)(*it))->count << ',' << ((node_leaf*)(*it))->src_index << endl;
		//show((node_branch*)(*it),whitspace+1,IsEnd);
		IsEnd.pop_back();
	}
}

bool TreeBase::empty()
{
	ASSERT(root_ != NULL);
	return root_->childs.empty();
}
void TreeBase::clear()
{
	if (!root_) return;
	partial_clear(root_);
	root_ = NULL;
	src_head_ = NULL;
	src_tail_ = NULL;
}
void TreeBase::partial_clear(node_branch* parent)
{
	auto it = parent->childs.begin();
	auto end = parent->childs.end();

	if (parent->bLeaf)
		for (; it != end; it++)
			delete (node_leaf*)*it;
	else
		for (; it != end; it++)
			partial_clear((node_branch*)*it);

	delete parent;
}