#ifndef __LEXI_TREE_BASE_H
#define __LEXI_TREE_BASE_H
#define MAX_CHILD_INDEX		7
#define MIN_CHILD_INDEX		MAX_CHILD_INDEX/2

#include <iostream>
#include <vector>
#include <list>
using namespace std;

struct node_branch
{
	bool bLeaf;
	size_t count;
	node_branch* parent;
	list<void*> childs;
	node_branch() : bLeaf(true), count(0), parent(NULL) {}
};

struct node_leaf
{
	size_t count;
	size_t src_index;	//index of color or font
	node_leaf* left;
	node_leaf* right;
	node_branch* parent;
	node_leaf() : count(0), left(NULL), right(NULL), parent(NULL) {}
};

class TreeBase
{
public:
	TreeBase();
	~TreeBase();
public:
	// note!!!position start with 0 not 1
	void insert(size_t position, size_t  count);
	void insert(size_t position, size_t  count, size_t src_index);
	void remove(size_t position);
	void remove(size_t position_begin, size_t position_end);
	void modify(size_t position_begin, size_t position_end, size_t src_index);
	void get_src_index(size_t position, size_t& src_index, size_t& last_cnt);
	size_t get_src_index(size_t position);
public:
	void clear();
	bool empty();
	void print_leafs();
	void print_hole_tree();
	void store_stream(FILE* file);
	void build_from_stream(FILE* file);
protected:
	void partial_clear(node_branch* parent);
	void show(node_branch* parent, int whitspace, vector<int>& IsEnd);
	void showleafchilds(node_branch* parent, int whitspace, vector<int>& IsEnd);
	bool add_child_after(node_branch* parent, void* left_brother, void* to_added);
	bool add_child_before(node_branch* parent, void* right_brother, void* to_added);
	void get_brother_branch(node_branch* branch, node_branch* &left_brother, node_branch* &right_brother);
	void shared_with_left(node_branch* parent, node_branch* left_brother);
	void shared_with_right(node_branch* parent, node_branch* right_brother);
	void shared_with_new_brother(node_branch* left_brother, node_branch* right_brother);
	node_leaf* find(size_t position, node_branch* root, size_t& offset);
	void tune_up(node_branch* parent, size_t count);
	void tune_down(node_branch* parent, size_t count);
	void split(node_branch* parent);
	void remove_child(node_branch* parent, void* child);
	void delete_leaf(node_leaf* leaf);
	void merge_with_left(node_branch* parent, node_branch* left_brother);
	void merge_with_right(node_branch* parent, node_branch* right_brother);
	void combine(node_branch* left, node_branch* right);
	void merge(node_branch* parent);
private:
	node_leaf* src_head_;
	node_leaf* src_tail_;
private:
	node_branch* root_;
};

#endif