//
// Created by dguco on 2022/4/13.
//

#ifndef RBTARR_MAP_RBTARR_MAP_H
#define RBTARR_MAP_RBTARR_MAP_H

#include <map>
#include <limits.h>
#include "hash_helper.h"
#include "hash_function.h"

using namespace std;

/**
 *
 * @tparam KeyType_   key的类型
 * @tparam ValueType_ value的类型
 * @tparam _Cap      map的容量上限
 * @tparam IndexType_   map的数组下标数据类型，默认为unsigned int(_Cap的大小必须小于IndexType_的最大值，ex:当IndexType_为unsigned int时_Cap支持的最大值为UINT_MAX -1)
 *     给这个类型参数主要是为了当_Cap较小时可以有效的减少map的内存占用
 */
template <typename KeyType_, typename ValueType_, std::size_t _Cap,typename IndexType_ = unsigned int>
class RbtHashMap
{
public:
    typedef node_pool<KeyType_,ValueType_,IndexType_, _Cap> hash_array;
    typedef MapValueType<ValueType_> mapped_type;
    typedef typename hash_array::tree_type tree_type;
    typedef typename hash_array::class_type class_type;
    typedef typename hash_array::iterator iterator;
    typedef typename hash_array::const_iterator const_iterator;
    typedef typename hash_array::reference reference;
    typedef typename hash_array::node_type node_type;

    iterator begin()
    {
        return hash_array_.begin(buckets_);
    }

    iterator end()
    {
        return hash_array_.end();
    }

    const_iterator begin() const
    {
        return hash_array_.begin(buckets_);
    }

    const_iterator end() const
    {
        return hash_array_.end();
    }

    std::size_t cap() const
    {
        return hash_array_.cap();
    }

    std::size_t size() const
    {
        return hash_array_.size();
    }

    iterator insert(const class_type& v)
    {
        hash_function::hash<KeyType_> hash_func;
        std::size_t bucket = hash_func(v.first) % _Cap;

        tree_type rb_tree = hash_array_.make_rbtree(buckets_[bucket].root_);
        //该bucket是空的
        if ( rb_tree.isEmpty())
        {
            //申请一个节点
            node_type* new_node = hash_array_.allocate_node(v);
            if(!new_node)
            {
                return end();
            }
            rb_tree.insert(new_node);
            IndexType_ oldhead = hash_array_.rb_tree_head_bucket();
            //设置root信息
            buckets_[bucket].root_ = rb_tree.root();
            //新的rbtree成为新的树链头部,重新设置树链的前后指针
            buckets_[bucket].prebucket_ = 0;
            buckets_[bucket].nextbucket_ = oldhead;
            if(oldhead > 0 && oldhead <= _Cap)
            {
                buckets_[oldhead - 1].prebucket_ = bucket + 1;
            }
            hash_array_.set_rb_tree_head_bucket(bucket + 1);
            return hash_array_.make_iterator(new_node,buckets_);
        }
        node_type* old_node = rb_tree.search(v.first);
        if(old_node != NULL)
        {
            return end();
        }

        //申请一个节点
        node_type* new_node = hash_array_.allocate_node(v,hash_array_.get_node(buckets_[bucket].root_));
        if( !new_node )
        {
            return end();
        }
        //把新节点插入红黑树
        rb_tree.insert(new_node);
        buckets_[bucket].root_ = rb_tree.root();
        return hash_array_.make_iterator(new_node,buckets_);
    }

    iterator insert(const KeyType_& k, const ValueType_& v)
    {
        return insert(std::make_pair(k,v));
    }

    iterator find(const KeyType_& k)
    {
        hash_function::hash<KeyType_> hash_func;
        std::size_t bucket = hash_func(k) % _Cap;

        tree_type rb_tree = hash_array_.make_rbtree(buckets_[bucket].root_);
        //该bucket是空的
        if ( rb_tree.isEmpty())
        {
            return this->end();
        }
        return hash_array_.make_iterator(rb_tree.search(k),buckets_);
    }

    const_iterator find(const KeyType_& k) const
    {
        hash_function::hash<KeyType_> hash_func;
        std::size_t bucket = hash_func(k) % _Cap;

        tree_type rb_tree = hash_array_.make_rbtree(buckets_[bucket].root_);
        //该bucket是空的
        if ( rb_tree.isEmpty())
        {
            return this->end();
        }
        return hash_array_.make_iterator(rb_tree.search(k),buckets_);
    }

    iterator erase( iterator it )
    {
        if(it == hash_array_.end() || it.curNode() == NULL)
        {
            return hash_array_.end();
        }

        hash_function::hash<KeyType_> hash_func;
        std::size_t bucket = hash_func(it->first) % _Cap;
        tree_type rb_tree = hash_array_.make_rbtree(buckets_[bucket].root_);
        //该bucket是空的
        if ( rb_tree.isEmpty())
        {
            return hash_array_.end();
        }

        iterator tmit(it);
        tmit++;
        node_type* remove_node = rb_tree.remove(it.curNode());
        if(!rb_tree.isEmpty())
        {
            buckets_[bucket].root_ = rb_tree.root();
        }else//树删空了
        {
            //update bucket info
            buckets_[bucket].root_ = 0;
            IndexType_ preBucket = buckets_[bucket].prebucket_;
            IndexType_ nextBucket = buckets_[bucket].nextbucket_;
            //前后两棵树不为空，把前树的尾部指向后树，后树的头部指向前树
            if((preBucket > 0 && preBucket <= _Cap)  && (nextBucket > 0 && nextBucket <= _Cap))
            {
                buckets_[preBucket - 1].nextbucket_ = nextBucket;
                buckets_[nextBucket - 1].prebucket_ = preBucket;
            }
            //前树不为空后树为空
            else if(preBucket > 0 && preBucket <= _Cap)
            {
                buckets_[preBucket - 1].nextbucket_ = 0;
            }
            //前树为空后树不为空,调整树链指针，并且后树成为新的树链的头
            else if(nextBucket > 0 && nextBucket <= _Cap)
            {
                buckets_[nextBucket - 1].prebucket_ = 0;
                hash_array_.set_rb_tree_head_bucket(nextBucket);
            }
            else if(preBucket == 0 && nextBucket == 0)
            {
                hash_array_.set_rb_tree_head_bucket(0);
            }
            buckets_[bucket].prebucket_ = 0;
            buckets_[bucket].nextbucket_ = 0;
        }
        hash_array_.deallocate_node(remove_node);
        return tmit;
    }

    bool try_erase(iterator it )
    {
        if(it == hash_array_.end() || it.curNode() == NULL)
        {
            return false;
        }

        hash_function::hash<KeyType_> hash_func;
        std::size_t bucket = hash_func(it->first) % _Cap;
        tree_type rb_tree = hash_array_.make_rbtree(buckets_[bucket].root_);
        //该bucket是空的
        if ( rb_tree.isEmpty())
        {
            return false;
        }

        iterator tmit(it);
        tmit++;
        node_type* remove_node = rb_tree.remove(it.curNode());
        if(!rb_tree.isEmpty())
        {
            buckets_[bucket].root_ = rb_tree.root();
        }else//树删空了
        {
            //update bucket info
            buckets_[bucket].root_ = 0;
            IndexType_ preBucket = buckets_[bucket].prebucket_;
            IndexType_ nextBucket = buckets_[bucket].nextbucket_;
            //前后两棵树不为空，把前树的尾部指向后树，后树的头部指向前树
            if((preBucket > 0 && preBucket <= _Cap)  && (nextBucket > 0 && nextBucket <= _Cap))
            {
                buckets_[preBucket - 1].nextbucket_ = nextBucket;
                buckets_[nextBucket - 1].prebucket_ = preBucket;
            }
                //前树不为空后树为空
            else if(preBucket > 0 && preBucket <= _Cap)
            {
                buckets_[preBucket - 1].nextbucket_ = 0;
            }
                //前树为空后树不为空,调整树链指针，并且后树成为新的树链的头
            else if(nextBucket > 0 && nextBucket <= _Cap)
            {
                buckets_[nextBucket - 1].prebucket_ = 0;
                hash_array_.set_rb_tree_head_bucket(nextBucket);
            }
            buckets_[bucket].prebucket_ = 0;
            buckets_[bucket].nextbucket_ = 0;
        }
        hash_array_.deallocate_node(remove_node);
        return true;
    }

    bool try_erase(const KeyType_& k )
    {
        return try_erase(find(k));
    }

    iterator erase( const KeyType_& k )
    {
        return erase(find(k));
    }

    void clear()
    {
        for( IndexType_ t = 0; t < _Cap; ++t )
        {
            buckets_[t].root_ = 0;
            buckets_[t].prebucket_ = 0;
            buckets_[t].nextbucket_ = 0;
        }
        hash_array_.clear();
    }

    ValueNode<KeyType_,ValueType_>* data()
    {
        return hash_array_.data();
    }

    mapped_type operator[](KeyType_ _KeyValue)
    {
        iterator it = find(_KeyValue);
        if(it != end())
        {
            return mapped_type(it->second);
        }else
        {
            it = insert(_KeyValue,ValueType_());
            if(it != end())
            {
                return mapped_type(it->second);
            }
        }
        return mapped_type();
    }

    RbtHashMap()
    {
        clear();
    }

    ~RbtHashMap()
    {
        clear();
    }
private:
    RbtHashMap(const RbtHashMap& other );

private:
    bucket_type<IndexType_>		buckets_[_Cap];     //bucket array
    hash_array	 	            hash_array_;		//内存管理器
};

#endif //RBTARR_MAP_RBTARR_MAP_H
