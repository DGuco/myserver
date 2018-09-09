//
// Created by DGuco on 18-8-28.
// 使用共享内存的定长vector,vector的是长度一定的,vector中的对象内存地址在事先申请好的共享内存中
//

#ifndef SERVER_VECTOR_H
#define SERVER_VECTOR_H

#include "share_container_interface.h"
#include "array_list.h"
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <list>

/**
 *  sharemem,value_type 非指针非引用
 */
namespace sharemem
{
	template<typename T, std::size_t count>
	class vector final: public container
	{
	public:
		// type definitions
		typedef T value_type;
		typedef T *iterator;
		typedef T *const const_iterator;
		typedef T &reference;
		typedef const T &const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

		vector()
		{
			if (std::is_pointer<T>::value || std::is_reference<T>::value) {
				throw std::invalid_argument("the value type of share map is illegal");
			}
		}

		vector(const vector &other)
		{
			if (&other == this) {
				return;
			}
			memcpy(this->elems_, other.elems_, sizeof(T) * size_);
			this->size_ = other.size_;
		}

		//no move
		vector(vector &&other) = delete;

		vector &operator=(vector &other)
		{
			if (this == &other) {
				return *this;
			}
			memcpy(this->elems_, other.elems_, sizeof(T) * size_);
			this->size_ = other.size_;
		}

		vector &operator=(vector &&other)
		{
			if (this == &other) {
				return *this;
			}
			memcpy(this->elems_, other.elems_, sizeof(T) * size_);
			this->size_ = other.size_;
		}

		void initialize() override
		{
			//do nothing
		}

		void resume() override
		{
			//do nothing
		}

		// iterator support
		iterator begin()
		{
			return elems_;
		}

		const_iterator begin() const
		{
			return elems_;
		}

		iterator end()
		{
			return elems_ + size_;
		}

		const_iterator end() const
		{
			return elems_ + size_;
		}

		// reverse iterator support
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

		reverse_iterator rbegin()
		{
			return reverse_iterator(end( ));
		}

		const_reverse_iterator rbegin() const
		{
			return const_reverse_iterator(end( ));
		}

		reverse_iterator rend()
		{
			return reverse_iterator(begin( ));
		}

		const_reverse_iterator rend() const
		{
			return const_reverse_iterator(begin( ));
		}

		// operator[]
		reference operator[](size_type i)
		{
			MY_ASSERT(i < size_ && "operator[] out of range", ;);
			return elems_[i];
		}

		const_reference operator[](size_type i) const
		{
			MY_ASSERT(i < size_ && "operator[] out of range", ;);
			return elems_[i];
		}

		// front() and back()
		reference front()
		{
			return elems_[0];
		}

		const_reference front() const
		{
			return elems_[0];
		}

		reference back()
		{
			return elems_[size_ - 1];
		}

		const_reference back() const
		{
			return elems_[size_ - 1];
		}

		size_type size()
		{
			return size_;
		}

		bool empty()
		{
			return 0 == size_;
		}

		static size_type max_size()
		{
			return count;
		}

		// 后面插入一个
		void push_back(const T &t)
		{
			MY_ASSERT_STR((size_ < count), return, "share vector push_back size_ = %d > capacity =  %d", size_, count);
			elems_[size_++] = t;
		}

		//在头部插入一个元素
		void push_front(const T &t)
		{
			MY_ASSERT_STR((size_ < count), return, "share vector push_back size_ = %d > capacity = %d", size_, count);
			for (int i = size_; i > 0; i--) {
				elems_[i] = elems_[i - 1];
			}
			elems_[0] = t;
			size_++;
		}

		// erase a element and move the back elemts to front
		iterator erase(iterator pos)
		{
			iterator endPos = end( );
			if (pos != endPos) {
				iterator nextPos = pos + 1;
				if (nextPos != endPos) {
					std::copy(nextPos, endPos, pos);
				}
				--size_;
			}
			return pos;
		}

		void erase(size_type pos)
		{
			MY_ASSERT_STR(pos < size_, return, "share vector erase out of range,pos = %d,size = %d", pos, size_);
			size_type next = pos + 1;
			if (next < size_) {
				std::copy(elems_ + next, elems_ + size_, elems_ + pos);
			}
			--size_;
		}

		iterator erase(iterator first, iterator last)
		{
			iterator __i(std::copy(last, end( ), first));
			size_ -= (last - first);
			return first;
		}

		void pop_back()
		{
			size_--;
		}

		size_t capacity()
		{
			return count;
		}

		void clear()
		{
			size_ = 0;
		}

	private:
		T elems_[count];
		size_t size_;
	};
}
#endif //SERVER_VECTOR_H
