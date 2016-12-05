#ifndef YQ_HASHMAP_MOD_H_
#define YQ_HASHMAP_MOD_H_


#include "array_list.h"
#include "hash_function.h"

namespace yq
{
	// 这个hashmap是用于teamid无法均匀分布在原有hashmap中而做的优化
	// 并不是所有key都适合使用
	template <class _Kty, class _Ty, std::size_t _Cap, std::size_t _Mod>
	class hash_map_mod
	{

	public:

		typedef _Kty	key_type;
		typedef _Ty		mapped_type;

		typedef std::pair<key_type, mapped_type> value_type;
		typedef fixed_size_memory_pool<value_type, _Cap> hash_table;


		typedef typename hash_table::iterator iterator;
		typedef typename hash_table::const_iterator const_iterator;
		typedef typename hash_table::reference reference;
		typedef typename hash_table::const_reference const_reference;

		typedef typename hash_table::pointer pointer;
		typedef typename hash_table::const_pointer const_pointer;

		typedef typename hash_table::node_type node_type;
		typedef typename hash_table::size_type size_type;
		typedef typename hash_table::offset_type offset_type;
		typedef std::pair<iterator, bool> _Pairib;

		// Get an iterator for the beginning of the map.
		iterator begin()
		{
			return hash_table_.begin ();
		}

		// Get an iterator for the end of the map.
		iterator end()
		{
			return hash_table_.end();
		}

		// Get an iterator for the beginning of the map.
		const_iterator begin() const
		{
			return hash_table_.begin();
		}

		// Get an iterator for the end of the map.
		const_iterator end() const
		{
			return hash_table_.end();
		}

		// initialize the map.
		void initialize()
		{
			for( std::size_t t = 0; t < _Cap; ++t )
			{
				buckets_[t].first = -1;
				buckets_[t].last = -1;
			}

			hash_table_.initailize_pool();
		}

		size_type max_size() const
		{
			return hash_table_.max_size();
		}

		size_type size() const
		{
			return max_size() - hash_table_.size();
		}



		// Check whether the map is empty.
		bool empty() const
		{
			return size() == 0;
		}


		// Insert a new entry into the map.
		_Pairib insert(const value_type& v)
		{
			yq::hash<key_type> _hash;
			std::size_t bucket = (_hash(v.first) / _Mod) % _Cap;

			offset_type begin = buckets_[bucket].first;
			iterator it_begin = iterator( reinterpret_cast<node_type*>(hash_table_[begin]) );


			if ( it_begin == this->end()  )
			{
				pointer new_node = hash_table_.allocate(v);
				if( !new_node )
				{
					return _Pairib(this->end(), false);
				}
				buckets_[bucket].first = buckets_[bucket].last = hash_table_.node_offset(new_node);
				iterator it_find = iterator( reinterpret_cast<node_type*>( hash_table_[buckets_[bucket].first]) );
				return _Pairib(it_find, true);
			}

			offset_type end = buckets_[bucket].last;
			iterator it_end  = iterator( reinterpret_cast<node_type*>(hash_table_[end]) );

			++it_end;

			while ( it_begin != it_end )
			{
				if (it_begin->first == v.first)
				{
					return _Pairib(it_begin, false);
				}
				++it_begin;
			}

			pointer new_node = hash_table_.allocate(v);
			if( !new_node )
			{
				return _Pairib(this->end(), false);
			}

			hash_table_.insert_head_before(  hash_table_[begin] );

			it_begin = iterator( reinterpret_cast<node_type*>(hash_table_[begin]) );

			-- it_begin ;

			buckets_[bucket].first = hash_table_.node_offset(&*it_begin);

			return _Pairib(it_begin, true);

		}

		// Find an entry in the map.
		iterator find(const key_type& k)
		{
			yq::hash<key_type> _hash;
			std::size_t bucket = (_hash(k) / _Mod) % _Cap;

			offset_type begin = buckets_[bucket].first;
			iterator it_begin = iterator( reinterpret_cast<node_type*>(hash_table_[begin]) );

			if ( it_begin == this->end() )
			{
				return this->end();
			}

			offset_type end = buckets_[bucket].last;
			iterator it_end  = iterator( reinterpret_cast<node_type*>(hash_table_[end]) );

			++it_end;

			while ( it_begin != it_end )
			{
				if ( it_begin->first == k )
				{
					return it_begin;
				}

				++it_begin;
			}

			return this->end();

		}

		// Find an entry in the map.
		const_iterator find(const key_type& k) const
		{
			yq::hash<key_type> _hash;
			std::size_t bucket = (_hash(k) / _Mod) % _Cap;

			offset_type begin = buckets_[bucket].first;
			const_iterator it_begin = iterator( reinterpret_cast<node_type*>(hash_table_[begin]) );

			if ( it_begin == this->end() )
			{
				return this->end();
			}

			offset_type end = buckets_[bucket].last;
			const_iterator it_end  = iterator( reinterpret_cast<node_type*>(hash_table_[end]) );

			++it_end;

			while ( it_begin != it_end )
			{
				if ( it_begin->first == k )
				{
					return it_begin;
				}

				++it_begin;
			}

			return this->end();

		}


		// Erase an entry from the map.
		void erase( iterator it )
		{
			if ( it == this->end() )
			{
				return;
			}

			yq::hash<key_type> _hash;
			std::size_t bucket = (_hash(it->first) / _Mod) % _Cap;

			offset_type begin = buckets_[bucket].first;
			iterator it_begin = iterator( reinterpret_cast<node_type*>(hash_table_[begin]) );

			offset_type end = buckets_[bucket].last;
			iterator it_end  = iterator( reinterpret_cast<node_type*>(hash_table_[end]) );

			bool is_first = ( it == it_begin );
			bool is_last = ( it == it_end );

			if ( is_first && is_last )
			{
				buckets_[bucket].first = buckets_[bucket].last = -1;
			}
			else if ( is_first )
			{
				++ it_begin;
				buckets_[bucket].first = hash_table_.node_offset(&*it_begin);
			}
			else if ( is_last )
			{
				-- it_end;
				buckets_[bucket].last = hash_table_.node_offset(&*it_end);
			}

			hash_table_.deallocate( &*it);

		}

		size_type erase( const key_type& k )
		{
			erase (find(k));
			return size ();
		}

		// Remove all entries from the map.
		void clear()
		{
			// Clear the values.
			hash_table_.initailize_pool();

			// Initialise all buckets to empty.
			for (size_t i = 0; i < _Cap; ++i)
			{
				buckets_[i].first = buckets_[i].last = -1;
			}
		}


		hash_map_mod() {}
		~hash_map_mod() {}

	private:

		// Nocopyable
		hash_map_mod( const hash_map_mod& other );

	private:

		typedef struct bucket_type
		{
			offset_type first;
			offset_type last;
		}bucket_type;

		bucket_type	buckets_[_Cap];
		hash_table hash_table_;

	};

};


#endif /* YQ_HASHMAP_MOD_H_ */
