//
// Created by DGuco on 18-8-25.
// @brief  一个基于数组的双向链表
// 在此链表的基础上设计了一个固定大小的内存分配器,主要是用于对象的大小固定(对象中没有类似与标准库中的list,map,vector等线性容器),
// 对象最大个数固定，并且对象的成员无指针或者引用类型的对象,这样保证对象可以放在固定大小的共享内存中(原理:c++ placement new 操作符)。
//

#ifndef SERVER_ARRAY_LIST_H
#define SERVER_ARRAY_LIST_H

#include <cstddef>
#include "log.h"
#include "my_assert.h"

namespace sharemem
{
// 由于需要从类型T转化array_list_node_base<T>所以T应该放在最前面
// 为了方便快捷的删除，还是需要使用双向链表
	template<class T, int L>
	struct array_list_node_base
	{
	public:
		typedef std::ptrdiff_t difference_type;
		typedef array_list_node_base<T, L> _Self;
		typedef T value_type;
		typedef value_type &reference;
		typedef const value_type &const_reference;
		typedef value_type *pointer;
		typedef const value_type *const_pointer;

	private:
		char value_[sizeof(value_type)]; // need default constructor
		difference_type next_; // 下一个节点和当前位置的偏移，所以next_不能为0
		difference_type prev_; // 同next_一样，也不能是0

		difference_type other_link_[L];

		std::size_t max_count;
		difference_type start_off_; // 当前节点与数组头节点的偏移量

	public:
		void set_next_idx(difference_type idx)
		{
			next_ = idx;
		}

		difference_type get_next_idx() const
		{
			return next_;
		}

		void set_prev_idx(difference_type idx)
		{
			prev_ = idx;
		}

		difference_type get_prev_idx() const
		{
			return prev_;
		}

		bool set_other_link(difference_type idx_value, int idx)
		{
			if (idx < 0 || idx >= L)
				return false;
			other_link_[idx] = idx_value;
			return true;
		}

		bool get_other_link(difference_type &idx_value, int idx)
		{
			if (idx < 0 || idx >= L)
				return false;
			idx_value = other_link_[idx];
			return true;
		}

		void set_max_count(std::size_t count)
		{
			max_count = count;
		}

		std::size_t get_max_count()
		{
			return max_count;
		}

		void set_node_start(difference_type off)
		{
			start_off_ = off;
		}

		difference_type get_node_start()
		{
			return start_off_;
		}

		_Self *other_link(int idx)
		{
			if (idx >= L || idx < 0) {
				return 0;
			}

			if (0 != other_link_[idx]) {
				return this + other_link_[idx];
			}
			else {
				return 0;
			}

		}

		void clear_otherlink()
		{
			for (int i = 0; i < L; ++i) {
				other_link_[i] = 0;
			}

		}

		_Self *next()
		{
			if (0 != get_next_idx( )) {
				return this + get_next_idx( );
			}
			else {
				return 0;
			}
		}

		const _Self *next() const
		{
			if (0 != get_next_idx( )) {
				return this + get_next_idx( );
			}
			else {
				return 0;
			}
		}

		_Self *prev()
		{
			if (0 != get_prev_idx( )) {
				return this + get_prev_idx( );
			}
			else {
				return 0;
			}
		}

		const _Self *prev() const
		{
			if (0 != get_prev_idx( )) {
				return this + get_prev_idx( );
			}
			else {
				return 0;
			}
		}

		// insert this before the old head node
		_Self &attach_head(_Self &head)
		{
			//difference_type off = &head - this;
			difference_type off = head.get_node_start( ) - get_node_start( );

			head.set_prev_idx(-off);
			set_next_idx(off);
			set_prev_idx(0);
			return *this;
		}

		_Self &attach_head(_Self *head)
		{
			if (0 == head) {
				alone_list( );
			}
			else {
				//difference_type off = head - this;
				difference_type off = head->get_node_start( ) - get_node_start( );

				head->set_prev_idx(-off);
				set_next_idx(off);
				set_prev_idx(0);
			}
			return *this;
		}

		_Self &attach(_Self *node)
		{
			if (node) {
				//difference_type off = node - this;
				difference_type off = node->get_node_start( ) - get_node_start( );

				node->set_prev_idx(-off);
				set_next_idx(off);
			}
			else {
				set_next_idx(0);
			}
			return *this;
		}

		// insert a chain list form first to last(including both nodes) before this node
		_Self *insert_head(_Self &first, _Self &last)
		{
			//difference_type off = this - &last;
			difference_type off = get_node_start( ) - last.get_node_start( );

			last.set_next_idx(off);
			set_prev_idx(-off);
			first.set_prev_idx(0);
			return &first;
		}

		static _Self *alone_list(_Self &first, _Self &last)
		{
			last.set_next_idx(0);
			first.set_prev_idx(0);
			return &first;
		}

		_Self *delete_head()
		{
			_Self *new_head = this->next( );
			alone_list( );
			if (new_head) {
				new_head->set_prev_idx(0);
			}
			return new_head;
		}

		_Self &delete_node()
		{
			_Self *prev_node = prev( );
			_Self *next_node = next( );
			difference_type off = 0;
			if (prev_node && next_node) {
				off = next_node - prev_node;
			}
			if (prev_node) {
				prev_node->set_next_idx(off);
			}
			if (next_node) {
				next_node->set_prev_idx(-off);
			}
			alone_list( );
			return *this;
		}

		// create an alone list
		_Self *alone_list()
		{
			set_next_idx(0);
			set_prev_idx(0);
			return this;
		}

		reference value()
		{
			return *(reinterpret_cast<pointer>(value_));
		}

		const_reference value() const
		{
			return *(reinterpret_cast<const_pointer>(value_));
		}
	};

//////////////////////////////////////////////////////////////////////////
// array_list_node_base  0数组模板特化
//////////////////////////////////////////////////////////////////////////


// 由于需要从类型T转化array_list_node_base<T>所以T应该放在最前面
// 为了方便快捷的删除，还是需要使用双向链表
	template<class T>
	struct array_list_node_base<T, 0>
	{
	public:
		typedef std::ptrdiff_t difference_type;
		typedef array_list_node_base<T, 0> _Self;
		typedef T value_type;
		typedef value_type &reference;
		typedef const value_type &const_reference;
		typedef value_type *pointer;
		typedef const value_type *const_pointer;

	private:
		char value_[sizeof(value_type)]; // need default constructor
		difference_type next_; // 下一个节点和当前位置的偏移，所以next_不能为0
		difference_type prev_; // 同next_一样，也不能是0

		difference_type other_link_[0];

		std::size_t max_count;
		difference_type start_off_; // 当前节点与数组头节点的偏移量

	public:
		void set_next_idx(difference_type idx)
		{
			next_ = idx;
		}

		difference_type get_next_idx() const
		{
			return next_;
		}

		void set_prev_idx(difference_type idx)
		{
			prev_ = idx;
		}

		difference_type get_prev_idx() const
		{
			return prev_;
		}

		bool set_other_link(difference_type idx_value, int idx)
		{
			if (idx != 0)
				return false;
			other_link_[idx] = idx_value;
			return true;
		}

		bool get_other_link(difference_type &idx_value, int idx)
		{
			if (idx != 0)
				return false;
			idx_value = other_link_[idx];
			return true;
		}

		void set_max_count(std::size_t count)
		{
			max_count = count;
		}

		std::size_t get_max_count()
		{
			return max_count;
		}

		void set_node_start(difference_type off)
		{
			start_off_ = off;
		}

		difference_type get_node_start()
		{
			return start_off_;
		}

		_Self *other_link(int idx)
		{
			return 0;
		}

		void clear_otherlink()
		{

		}

		_Self *next()
		{
			if (0 != get_next_idx( )) {
				return this + get_next_idx( );
			}
			else {
				return 0;
			}
		}

		const _Self *next() const
		{
			if (0 != get_next_idx( )) {
				return this + get_next_idx( );
			}
			else {
				return 0;
			}
		}

		_Self *prev()
		{
			if (0 != get_prev_idx( )) {
				return this + get_prev_idx( );
			}
			else {
				return 0;
			}
		}

		const _Self *prev() const
		{
			if (0 != get_prev_idx( )) {
				return this + get_prev_idx( );
			}
			else {
				return 0;
			}
		}

		// insert this before the old head node
		_Self &attach_head(_Self &head)
		{
			//difference_type off = &head - this;
			difference_type off = head.get_node_start( ) - get_node_start( );

			head.set_prev_idx(-off);
			set_next_idx(off);
			set_prev_idx(0);
			return *this;
		}

		_Self &attach_head(_Self *head)
		{
			if (0 == head) {
				alone_list( );
			}
			else {
				//difference_type off = head - this;
				difference_type off = head->get_node_start( ) - get_node_start( );

				head->set_prev_idx(-off);
				set_next_idx(off);
				set_prev_idx(0);
			}
			return *this;
		}

		_Self &attach(_Self *node)
		{
			if (node) {
				//difference_type off = node - this;
				difference_type off = node->get_node_start( ) - get_node_start( );

				node->set_prev_idx(-off);
				set_next_idx(off);
			}
			else {
				set_next_idx(0);
			}
			return *this;
		}

		// insert a chain list form first to last(including both nodes) before this node
		_Self *insert_head(_Self &first, _Self &last)
		{
			//difference_type off = this - &last;
			difference_type off = get_node_start( ) - last.get_node_start( );

			last.set_next_idx(off);
			set_prev_idx(-off);
			first.set_prev_idx(0);
			return &first;
		}

		static _Self *alone_list(_Self &first, _Self &last)
		{
			last.set_next_idx(0);
			first.set_prev_idx(0);
			return &first;
		}

		_Self *delete_head()
		{
			_Self *new_head = this->next( );
			alone_list( );
			if (new_head) {
				new_head->set_prev_idx(0);
			}
			return new_head;
		}

		_Self &delete_node()
		{
			_Self *prev_node = prev( );
			_Self *next_node = next( );
			difference_type off = 0;
			if (prev_node && next_node) {
				off = next_node - prev_node;
			}
			if (prev_node) {
				prev_node->set_next_idx(off);
			}
			if (next_node) {
				next_node->set_prev_idx(-off);
			}
			alone_list( );
			return *this;
		}

		// create an alone list
		_Self *alone_list()
		{
			set_next_idx(0);
			set_prev_idx(0);
			return this;
		}

		reference value()
		{
			return *(reinterpret_cast<pointer>(value_));
		}

		const_reference value() const
		{
			return *(reinterpret_cast<const_pointer>(value_));
		}
	};

//////////////////////////////////////////////////////////////////////////


	template<typename T, int L = 0>
	struct array_link_list_iterator
	{
		typedef array_list_node_base<T, L> node_type;
		typedef array_link_list_iterator<T, L> _Self;
		//typedef std::bidirectional_iterator_tag iterator_category;
		typedef T value_type;
		typedef value_type *pointer;
		typedef value_type &reference;
		typedef std::ptrdiff_t difference_type;
		array_link_list_iterator(const _Self &__x)
			: ptr_(__x.ptr_)
		{
		}

		explicit array_link_list_iterator(node_type *__x)
			: ptr_(__x)
		{
		}

		array_link_list_iterator()
			: ptr_(0)
		{}

		reference operator*() const
		{
			return ptr_->value( );
		}

		pointer operator->() const
		{
			return &(operator*( ));
		}

		_Self &operator++()
		{
			ptr_ = ptr_->next( );
			return *this;
		}
		_Self operator++(int)
		{
			_Self __tmp(*this);
			++*this;
			return __tmp;
		}

		_Self &operator--()
		{
			ptr_ = ptr_->prev( );
			return *this;
		}
		_Self operator--(int)
		{
			_Self __tmp(*this);
			--*this;
			return __tmp;
		}

		bool operator==(const _Self &__x) const
		{
			return ptr_ == __x.ptr_;
		}

		bool operator!=(const _Self &__x) const
		{
			return !(*this == __x);
		}

		_Self &operator=(const _Self &__x)
		{
			if (this != &__x) {
				ptr_ = __x.ptr_;
			}
			return *this;
		}
		node_type *ptr_;
	};

	template<typename T, int L = 0>
	struct array_link_list_const_iterator
	{
		typedef array_list_node_base<T, L> node_type;
		typedef array_link_list_iterator<T, L> _Self;
		//typedef std::bidirectional_iterator_tag iterator_category;
		typedef T value_type;
		typedef const value_type *pointer;
		typedef const value_type &reference;
		typedef std::ptrdiff_t difference_type;
		array_link_list_const_iterator(const _Self &__x)
			: ptr_(__x.ptr_)
		{
		}

		explicit array_link_list_const_iterator(const node_type *__x)
			: ptr_(__x)
		{
		}
		array_link_list_const_iterator()
			: ptr_(0)
		{}

		reference operator*() const
		{
			return ptr_->value( );
		}

		pointer operator->() const
		{
			return &(operator*( ));
		}

		_Self operator++()
		{
			ptr_ = ptr_->next( );
			return *this;
		}
		_Self operator++(int)
		{
			_Self __tmp(*this);
			++*this;
			return __tmp;
		}

		_Self &operator--()
		{
			ptr_ = ptr_->prev( );
			return *this;
		}
		_Self operator--(int)
		{
			_Self __tmp(*this);
			--*this;
			return __tmp;
		}

		bool operator==(const _Self &__x) const
		{
			return ptr_ == __x.ptr_;
		}

		bool operator!=(const _Self &__x) const
		{
			return !(*this == __x);
		}

		_Self &operator=(const _Self &__x)
		{
			if (this != &__x) {
				ptr_ = __x.ptr_;
			}
			return *this;
		}
		const node_type *ptr_;
	};

	template<class value_type, std::size_t MAX_COUNT, int L = 0>
	class fixed_size_array_list
	{
	public:
		static const int stack_bottom = -1;
		typedef array_list_node_base<value_type, L> node_type;
		typedef node_type *pointer;
		typedef const node_type *const_pointer;
		typedef node_type &reference;
		typedef const node_type &const_reference;
		typedef int offset_type;
		typedef std::size_t size_type;

	private:
		offset_type head_; // the start node of the array
		offset_type tail_; // the tail (valid) node of the array
		size_type size_;
		node_type node_[MAX_COUNT];

	public:
		// call this function to create the free list first
		// It should be called before any other calls
		// 这个函数没有放在构造函数中，是为了利用共享内存重新生成pool的时候，
		// 构造函数不需要调用此函数；也不能调用此函数
		void initailize_array_list()
		{
			segregate( );
		}

		void resume_array_list()
		{
			segregate_resume( );
		}

		/** ************************************************************************
		*  Returns true if the memory pool is empty.
		************************************************************************ */
		bool empty() const
		{
			return 0 == size_;
		}

		/** Returns the avaliable number of elements in the stack.
		*/
		size_type size() const
		{
			return size_;
		}

		/** Returns the size of the largest possible in the stack. */
		size_type max_size() const
		{
			return MAX_COUNT;
		}
		template<typename other>
		pointer allocate(other &v)
		{
			if (empty( )) {
				return 0;
			}
			else {
				--size_;
				node_type *p = node_ + head_;
				if (!empty( )) {
					MY_ASSERT((head_ >= 0 && head_ < (int) MAX_COUNT), return 0);
					// it's not the last one element in the dequeue
					head_ = head_ + node_[head_].get_next_idx( );
					node_[head_].set_prev_idx(0);
				}
				new(&p->value( )) value_type(v);
				return p;
			}
		}

		pointer allocate()
		{
			if (empty( )) {
				return 0;
			}
			else {
				--size_;
				node_type *p = node_ + head_;
				if (!empty( )) {
					MY_ASSERT((head_ >= 0 && head_ < (int) MAX_COUNT), return 0);
					// it's not the last one element in the dequeue
					head_ = head_ + node_[head_].get_next_idx( );

					/*if (head_ < 0 || head_ >= (int)MAX_COUNT)*/
					/*{*/
					/*// TODO:这里故意让它崩溃，方便定位问题*/
					/*node_type* tpNode = NULL;*/
					/*tpNode->set_prev_idx(0);*/
					/*}*/

					node_[head_].set_prev_idx(0);
				}
				new(&p->value( )) value_type( );
				return p;
			}
		}

		void deallocate(const_pointer __x)
		{
			if (__x) {
				offset_type __x_off = node_offset(__x);

				MY_ASSERT(__x_off >= 0 && __x_off < (int) MAX_COUNT, return);

				__x->value( ).~value_type( );
				if (empty( )) {
					head_ = tail_ = __x_off;
					node_[head_].alone_list( );
				}
				else {
					MY_ASSERT((tail_ >= 0 && tail_ < (int) MAX_COUNT), return);
					node_[tail_].attach(node_ + __x_off);
					tail_ = __x_off;
					node_[tail_].set_next_idx(0);
				}
				++size_;
			}
		}

		// the caller must make sure that from first to last is a chain
		// the last one is not included!
		size_type deallocate(const_pointer first, const_pointer last)
		{
			if (first == last) {
				return 0;
			}

			const_pointer cur_node = first;
			const_pointer next_node = cur_node->next( );
			size_type count = 1; // the first one
			for (; next_node != last; next_node = cur_node->next( )) {
				++count;
				cur_node->value( ).~value_type( );
				cur_node = next_node;
			}

			offset_type first_off = node_offset(first);
			offset_type last_off = node_offset(cur_node);

			MY_ASSERT(first_off >= 0 && first_off < (int) MAX_COUNT && last_off >= 0 && last_off < (int) MAX_COUNT,
					  return 0);

			if (empty( )) {
				head_ = node_offset(node_type::alone_list(
					node_[first_off], node_[last_off]));
			}
			else {
				MY_ASSERT((tail_ >= 0 && tail_ < (int) MAX_COUNT), return 0);
				node_[tail_].attach(node_ + first_off);
				node_[last_off].set_next_idx(0);
			}
			tail_ = last_off;
			size_ += count;
			return count;
		}

		// 下面是数组的特性
		offset_type node_offset(const_pointer __x) const
		{
			if (__x) {
				return __x - node_;
			}
			else {
				return stack_bottom;
			}
		}

		pointer operator[](offset_type __x)
		{
			if (valid_range(__x)) {
				return node_ + __x;
			}
			return 0;
		}

		const_pointer operator[](offset_type __x) const
		{
			if (valid_range(__x)) {
				return node_ + __x;
			}
			return 0;
		}

	private:
		bool valid_range(offset_type __x)
		{
			return __x >= 0 && __x < static_cast<offset_type>(MAX_COUNT);
		}

		void segregate()
		{
			head_ = 0;
			tail_ = static_cast<offset_type>(MAX_COUNT - 1);
			new(&node_[head_]) value_type( );
			node_[head_].set_max_count(MAX_COUNT);
			node_[head_].set_node_start(head_);
			for (offset_type i = 0; i < tail_; ++i) {
				new(&node_[i + 1]) value_type( );
				node_[i + 1].set_max_count(MAX_COUNT);
				node_[i + 1].set_node_start(i + 1);

				node_[i].attach(node_ + i + 1);

				node_[i].clear_otherlink( );
			}

			node_[tail_].clear_otherlink( );
			node_[tail_].set_next_idx(0);
			node_[head_].set_prev_idx(0);
			size_ = MAX_COUNT;
		}

		void segregate_resume()
		{
			// 之所以全部对象new一遍主要是为了重置虚表
			offset_type tmptail = static_cast<offset_type>(MAX_COUNT - 1);
			for (offset_type i = 0; i <= tmptail; ++i) {
				new(&node_[i]) value_type( );
			}
		}
	};

	/**
	* MAX_COUNT must be greater than 1
	* 主要提供了内存分配和释放功能。由于分配内存和释放内存调用
	* 对象的构造函数和析构函数
	* 提供的操作主要有allocate和deallocate
	* 另外还有方便作为数组访问的operator[]和node_offset
	*
	*/
	template<class value_type, std::size_t MAX_COUNT, int L = 0>
	class fixed_size_memory_pool
	{
	public:
		typedef sharemem::fixed_size_array_list<value_type, MAX_COUNT, L> Array_List;
		typedef sharemem::array_list_node_base<value_type, L> node_type;

		typedef sharemem::array_link_list_iterator<value_type, L> iterator;
		typedef sharemem::array_link_list_const_iterator<value_type, L> const_iterator;
		typedef value_type *pointer;
		typedef const value_type *const_pointer;
		typedef value_type &reference;
		typedef const value_type &const_reference;
		typedef int offset_type;
		typedef std::size_t size_type;
	private:
		void attach_head_node(node_type &__x)
		{
			if (Array_List::stack_bottom != allocated_start_) {
				__x.attach_head(array_list_[allocated_start_]);
			}
			else {
				__x.alone_list( );
			}
			allocated_start_ = array_list_.node_offset(&__x);
		}
		Array_List array_list_;
		offset_type allocated_start_;
	public:
		/*
		* 为了能使用共享内存，对于该类在构造函数中并不做什么工作。但是，
		* 如果需要初始化共享内存中的数据(即在共享内存中构造出该结构)，
		* 必须首先调用initailize_pool
		*/
		void initailize_pool()
		{
			array_list_.initailize_array_list( );
			allocated_start_ = Array_List::stack_bottom;
		}

		void resume_pool()
		{
			array_list_.resume_array_list( );
		}

		/** ************************************************************************
		*  Returns true if the memory pool is empty.
		************************************************************************ */
		bool empty() const
		{
			return array_list_.empty( );
		}

		/** Returns the avaliable number of elements in the stack.
		*/
		size_type size() const
		{
			return array_list_.size( );
		}

		/** Returns the size of the largest possible in the stack. */
		size_type max_size() const
		{
			return array_list_.max_size( );
		}

		template<typename other>
		pointer allocate(other &v)
		{
			node_type *p = array_list_.allocate(v);
			if (p) {
				attach_head_node(*p);
				return &p->value( );
			}
			return 0;
		}

		pointer allocate()
		{
			node_type *p = array_list_.allocate( );
			if (p) {
				attach_head_node(*p);
				return &p->value( );
			}
			return 0;
		}

		// 把头节点插入到某个节点之前
		bool insert_head_before(pointer dst_pos)
		{
			if (!dst_pos) {
				return false;
			}

			node_type *dst_node = reinterpret_cast<node_type *>(dst_pos);

			MY_ASSERT(allocated_start_ >= 0,
					  return false);

			node_type *head_node = array_list_[allocated_start_];

			allocated_start_ = array_list_.node_offset(head_node->delete_head( ));

			// dst node 是头节点
			if (allocated_start_ == array_list_.node_offset(dst_node)) {
				attach_head_node(*head_node);
				return true;
			}

			node_type *dst_prev_node = dst_node->prev( );

			head_node->attach(dst_node);

			if (NULL != dst_prev_node) {
				dst_prev_node->attach(head_node);
				return true;
			}

			return false;
		}

		void deallocate(const_pointer __x)
		{
			node_type *item = const_cast< node_type *>(reinterpret_cast<const node_type *>(__x));

			if (array_list_.node_offset(item) < 0 || array_list_.node_offset(item) >= (int) MAX_COUNT) {
				return;
			}

			if (allocated_start_ == array_list_.node_offset(item)) {
				allocated_start_ = array_list_.node_offset(item->delete_head( ));
			}
			else {
				item->delete_node( );
			}
			array_list_.deallocate(item);
		}

		// the caller must make sure that from first to last is a chain
		// the last one is not included!
		size_type deallocate(const_pointer first, const_pointer last)
		{
			node_type *first_node = const_cast< node_type *>(reinterpret_cast<const node_type *>(first));
			node_type *last_node = const_cast< node_type *>(reinterpret_cast<const node_type *>(last));
			node_type *prev_node = first_node->prev( );
			if (0 == prev_node) {
				if (last_node) {
					last_node->set_prev_idx(0);
				}
				allocated_start_ = array_list_.node_offset(last_node);
			}
			else {
				prev_node->attach(last_node);
			}
			return array_list_.deallocate(first_node, last_node);
		}

		// 下面是数组的特性
		offset_type node_offset(const_pointer __x) const
		{
			return array_list_.node_offset(reinterpret_cast<const node_type *>(__x));
		}

		pointer operator[](offset_type __x)
		{
			node_type *p = array_list_[__x];
			if (p) {
				return &p->value( );
			}
			return 0;
		}

		const_pointer operator[](offset_type __x) const
		{
			const node_type *p = array_list_[__x];
			if (p) {
				return &p->value( );
			}
			return 0;
		}

		// 下面为访问已经分配对象的iterator
		iterator begin()
		{
			if (Array_List::stack_bottom != allocated_start_) {
				return iterator(array_list_[allocated_start_]);
			}
			else {
				return end( );
			}
		}

		iterator end()
		{
			return iterator(0);
		}

		const_iterator begin() const
		{
			if (Array_List::stack_bottom != allocated_start_) {
				return const_iterator(array_list_[allocated_start_]);
			}
			else {
				return end( );
			}
		}

		const_iterator end() const
		{
			return const_iterator(0);
		}
	};
}

#endif //SERVER_ARRAY_LIST_H
