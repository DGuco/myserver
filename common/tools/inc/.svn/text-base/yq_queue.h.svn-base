/******************************************************************************
	文件名			: yq_queue.h
	版本号			: 1.0
	作者			: YQ
	生成日期		: 2009-07-10
	最近修改		: 
	功能描述		: 封装数据类型Queue, “FIFO”，先进先出，用前需初始化
	函数列表		: 
	修改历史		: 
		1 日期		: 2009-07-10
		作者		: YQ
		修改内容	: 生成
*******************************************************************************/
#ifndef __YQ_QUEUE_H__
#define __YQ_QUEUE_H__


#include "array_list.h"
#include "yq_assert.h"

namespace yq
{
	template < typename T, std::size_t count >
	class queue
	{
		typedef T value_type;
		typedef  value_type* pointer;
		typedef  value_type* const_pointer;
		typedef  value_type& reference;
		typedef  const value_type& const_reference;

		typedef fixed_size_memory_pool<value_type, count> mem_pool;
		typedef typename mem_pool::iterator iterator;
		typedef typename mem_pool::const_iterator const_iterator;
		typedef typename mem_pool::offset_type offset_type;
		typedef typename mem_pool::node_type node_type;
		typedef typename mem_pool::size_type size_type;

	public:
		void initailize( )
		{
			_front = 0;
			_mempool.initailize_pool();
		}

		void resume( )
		{
			_mempool.resume_pool();
		}

		bool empty( ) const
		{
			return size() == 0;
		}

		size_type size( ) const
		{
			return  count - _mempool.size( );
		}

		size_type max_size( ) const
		{
			return count;
		}

		void push( const value_type& _Val )
		{
			if ( empty() )
			{
				pointer p = _mempool.allocate( _Val );
				YQ_ASSERT( p != NULL, return );
				_front = _mempool.node_offset( p );
			}
			else
			{
				pointer p = _mempool.allocate( _Val );
				YQ_ASSERT( p != NULL, return );			
			}
		}

		void pop( )
		{
			iterator it = iterator( reinterpret_cast<node_type*>( _mempool[ _front ] ) );
			iterator front = it;
			if ( size() <= 1 )
			{
				_front = 0;
			}
			else
			{
				_front = _mempool.node_offset( &*(--it) );
			}

			_mempool.deallocate( &*front );
		}

		value_type& front( )
		{
			return * ( _mempool[ _front ] ) ;
		}

		const value_type& front( ) const
		{
			return * ( _mempool[ _front ] ) ;
		}

		value_type& back( )
		{
			return * _mempool.begin();
		}

		const value_type& back( ) const
		{
			return * _mempool.begin();
		}

	private:
		offset_type _front;
		mem_pool _mempool;
	};

};

#endif // __YQ_QUEUE_H__

