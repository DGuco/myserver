//
// Created by dguco on 2022/4/13.
//

#ifndef RBTARR_MAP_HASH_HELPER_H
#define RBTARR_MAP_HASH_HELPER_H

#include <cstddef>
#include <new>
#include <memory>
#include <limits.h>
#include <string.h>
#include <map>
#include "rb_tree.h"

namespace rbt_hash
{

template<typename IndexType_>
struct bucket_type
{
    IndexType_ root_;  			    //相同bucket rbtree根节点索引
    IndexType_ prebucket_;  		//pre  bucket
    IndexType_ nextbucket_;  		//next bucket
};

template<typename ValueType_>
struct MapValueType
{
    bool            succ_;
    ValueType_ *    pointer_;
    MapValueType() : succ_(false),pointer_(NULL)
    {}
    MapValueType(ValueType_* ppointer) : succ_(false),pointer_(ppointer)
    {}
    MapValueType(const MapValueType& other)
    {
        succ_ = other.succ_;
        pointer_ = other.pointer_;
    }
    MapValueType<ValueType_>& operator =(const ValueType_& other)
    {
        if(pointer_ != NULL)
        {
            *pointer_ = other;
            succ_ = true;
        }else
        {
            succ_ = false;
        }
        return  *this;
    }
    bool operator()()
    {
        return  succ_;
    }

    operator bool()
    {
        return  succ_;
    }
};

/**
 *迭代器class
* */
template<typename KeyType_,typename ValueType_,typename IndexType_,std::size_t Cap_ >
class node_list_iterator;

template<typename KeyType_,typename ValueType_,typename IndexType_,std::size_t Cap_ = 0>
class const_node_list_iterator
{
public:
    typedef RBTree<KeyType_,ValueType_,IndexType_,Cap_> tree_type;
    typedef RBTNode<KeyType_,IndexType_> node_type;
    typedef node_list_iterator<KeyType_,ValueType_,IndexType_,Cap_> iterator_type;
    typedef const_node_list_iterator<KeyType_,ValueType_,IndexType_,Cap_> const_iterator_type;
    typedef const std::pair<KeyType_,const ValueType_ * const> *pointer;
    typedef const std::pair<KeyType_,const ValueType_ * const> &reference;

    const_node_list_iterator(const const_iterator_type &other)
        : bucket_(other.bucket_), node_array_(other.node_array_), data_array_(other.data_array_), bucket_array_(other.bucket_array_), curnode_(other.curnode_)
    {}

    const_node_list_iterator(const iterator_type &other)
        : bucket_(other.bucket_), node_array_(other.node_array_), data_array_(other.data_array_), bucket_array_(other.bucket_array_), curnode_(other.curnode_)
    {}

    explicit const_node_list_iterator(IndexType_ bucket_, node_type *parray_,ValueNode<KeyType_,ValueType_>* pdata_,bucket_type<IndexType_>* pbucket_)
        : bucket_(bucket_), node_array_(parray_), data_array_(pdata_), bucket_array_(pbucket_), curnode_(NULL)
    {
        look_rbtree();
    }

    explicit const_node_list_iterator(node_type* node,node_type *parray_,ValueNode<KeyType_,ValueType_>* pdata_,bucket_type<IndexType_>* pbucket_)
        : bucket_(0), node_array_(parray_), data_array_(pdata_), bucket_array_(pbucket_), curnode_(node)
    {

    }

    const_node_list_iterator()
        : bucket_(0), node_array_(NULL), data_array_(NULL), bucket_array_(NULL), curnode_(NULL)
    {}

    reference operator*() const
    {
        if(curnode_ != NULL)
        {
            IndexType_ index = curnode_->get_data();
            if(index > 0 && index <= Cap_)
            {
                iteator_.first = curnode_->get_key();
                iteator_.second = data_array_[index - 1].pointer();
            }else
            {
                iteator_.first = curnode_->get_key();
                iteator_.second = NULL;
            }
        }else
        {
            iteator_.second = NULL;
        }
        return iteator_;
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    const_node_list_iterator &operator++()
    {
        look_rbtree();
        return *this;
    }

    const_node_list_iterator operator++(int)
    {
        const_node_list_iterator __tmp(*this);
        ++*this;
        return __tmp;
    }

    bool operator==(const const_node_list_iterator &other) const
    {
        return curnode_ == other.curnode_;
    }

    bool operator!=(const const_node_list_iterator &other) const
    {
        return !(*this == other);
    }

    const_node_list_iterator &operator=(const iterator_type &other)
    {
        if (this != &other)
        {
            bucket_ = other.bucket_;
            node_array_ = other.node_array_;
            data_array_ = other.data_array_;
            bucket_array_ = other.bucket_array_;
            iteator_ = other.iteator_;
            curnode_ = other.curnode_;
        }
        return *this;
    }

    const_node_list_iterator &operator=(const const_iterator_type &other)
    {
        if (this != &other)
        {
            bucket_ = other.bucket_;
            node_array_ = other.node_array_;
            data_array_ = other.data_array_;
            bucket_array_ = other.bucket_array_;
            iteator_ = other.iteator_;
            curnode_ = other.curnode_;
        }
        return *this;
    }
    node_type *get_node(std::size_t index) const
    {
        if (index > 0 && index <= Cap_) {
            return &node_array_[index - 1];
        }
        return 0;
    }

    IndexType_ get_cur() const
    {
        if(curnode_)
        {
            return ARRAY_OFFSET(node_array_, curnode_);
        }
        return 0;
    }

    node_type* curNode()
    {
        return curnode_;
    }

    operator bool()
    {
        return (*this != const_node_list_iterator());
    }

private:
    void look_rbtree()
    {
        //没有可遍历的节点了
        if(bucket_ <= 0 || bucket_ > Cap_)
        {
            curnode_ = NULL;
            return;
        }
        //寻找第一个节点,从树的最小节点开始遍历每一个树的所有节点
        if(curnode_ == NULL)
        {
            bucket_type<IndexType_> bucket = bucket_array_[bucket_ - 1];
            tree_type rbtree(node_array_, bucket.root_);
            curnode_ = rbtree.minimum();
        }else
        {
            bucket_type<IndexType_> bucket = bucket_array_[bucket_ - 1];
            tree_type rbtree(node_array_, bucket.root_);
            curnode_ = rbtree.successor(curnode_);
            //当前的树遍历完了
            if(curnode_ == NULL)
            {
                //遍历下一个树
                bucket_ = bucket_array_[bucket_ - 1].nextbucket_;
                if(bucket_ > 0 && bucket_ <= Cap_)
                {
                    bucket = bucket_array_[bucket_ - 1];
                    tree_type newtree(node_array_, bucket.root_);
                    curnode_ = newtree.minimum();
                    return;
                }else
                {
                    bucket_ = 0;
                    curnode_ = NULL;
                    return;
                }
            }
        }
    }
public:
    IndexType_                      bucket_;                //红黑树的根节点
    node_type*                      node_array_;          //节点所属的数组
    ValueNode<KeyType_,ValueType_>* data_array_;          //数据数组指针
    bucket_type<IndexType_>*        bucket_array_;        //hash数据数组指针
    std::pair<KeyType_,ValueType_*> iteator_;             //当前节点
    node_type                       *curnode_;            //节点
};

template<typename KeyType_,typename ValueType_,typename IndexType_,std::size_t Cap_ = 0>
class node_list_iterator
{
    friend class const_node_list_iterator<KeyType_,ValueType_,IndexType_,Cap_>;
public:
    typedef RBTree<KeyType_,ValueType_,IndexType_,Cap_> tree_type;
    typedef RBTNode<KeyType_,IndexType_> node_type;
    typedef node_list_iterator<KeyType_,ValueType_,IndexType_,Cap_> iterator_type;
    typedef std::pair<KeyType_,ValueType_*> *pointer;
    typedef std::pair<KeyType_,ValueType_*> &reference;

    node_list_iterator(const iterator_type &other)
        : bucket_(other.bucket_), node_array_(other.node_array_), data_array_(other.data_array_), bucket_array_(other.bucket_array_), curnode_(other.curnode_)
    {}

    explicit node_list_iterator(IndexType_ bucket_, node_type *parray_,ValueNode<KeyType_,ValueType_>* pdata_,bucket_type<IndexType_>* pbucket_)
        : bucket_(bucket_), node_array_(parray_), data_array_(pdata_), bucket_array_(pbucket_), curnode_(NULL)
    {
        look_rbtree();
    }

    explicit node_list_iterator(node_type* node,node_type *parray_,ValueNode<KeyType_,ValueType_>* pdata_,bucket_type<IndexType_>* pbucket_)
        : bucket_(0), node_array_(parray_), data_array_(pdata_), bucket_array_(pbucket_), curnode_(node)
    {

    }

    node_list_iterator()
        : bucket_(0), node_array_(NULL), data_array_(NULL), bucket_array_(NULL), curnode_(NULL)
    {}

    reference operator*()
    {
        if(curnode_ != NULL)
        {
            IndexType_ index = curnode_->get_data();
            if(index > 0 && index <= Cap_)
            {
                iteator_.first = curnode_->get_key();
                iteator_.second = data_array_[index - 1].pointer();
            }else
            {
                iteator_.first = curnode_->get_key();
                iteator_.second = NULL;
            }
        }else
        {
            iteator_.second = NULL;
        }
        return iteator_;
    }

    pointer operator->()
    {
        return &(operator*());
    }

    iterator_type &operator++()
    {
        look_rbtree();
        return *this;
    }

    iterator_type operator++(int)
    {
        iterator_type __tmp(*this);
        ++*this;
        return __tmp;
    }

    bool operator==(const iterator_type &other) const
    {
        return curnode_ == other.curnode_;
    }

    bool operator!=(const iterator_type &other) const
    {
        return !(*this == other);
    }

    iterator_type &operator=(const iterator_type &other)
    {
        if (this != &other)
        {
            bucket_ = other.bucket_;
            node_array_ = other.node_array_;
            data_array_ = other.data_array_;
            bucket_array_ = other.bucket_array_;
            iteator_ = other.iteator_;
            curnode_ = other.curnode_;
        }
        return *this;
    }

    node_type *get_node(std::size_t index) const
    {
        if (index > 0 && index <= Cap_) {
            return &node_array_[index - 1];
        }
        return 0;
    }

    IndexType_ get_cur() const
    {
        if(curnode_)
        {
            return ARRAY_OFFSET(node_array_, curnode_);
        }
        return 0;
    }

    node_type* curNode()
    {
        return curnode_;
    }

    operator bool()
    {
        return (*this != node_list_iterator());
    }

private:
    void look_rbtree()
    {
        //没有可遍历的节点了
        if(bucket_ <= 0 || bucket_ > Cap_)
        {
            curnode_ = NULL;
            return;
        }
        //寻找第一个节点,从树的最小节点开始遍历每一个树的所有节点
        if(curnode_ == NULL)
        {
            bucket_type<IndexType_> bucket = bucket_array_[bucket_ - 1];
            tree_type rbtree(node_array_, bucket.root_);
            curnode_ = rbtree.minimum();
        }else
        {
            bucket_type<IndexType_> bucket = bucket_array_[bucket_ - 1];
            tree_type rbtree(node_array_, bucket.root_);
            curnode_ = rbtree.successor(curnode_);
            //当前的树遍历完了
            if(curnode_ == NULL)
            {
                //遍历下一个树
                bucket_ = bucket_array_[bucket_ - 1].nextbucket_;
                if(bucket_ > 0 && bucket_ <= Cap_)
                {
                    bucket = bucket_array_[bucket_ - 1];
                    tree_type newtree(node_array_, bucket.root_);
                    curnode_ = newtree.minimum();
                    return;
                }else
                {
                    bucket_ = 0;
                    curnode_ = NULL;
                    return;
                }
            }
        }
    }
private:
    IndexType_                      bucket_;                //红黑树的根节点
    node_type*                      node_array_;          //节点所属的数组
    ValueNode<KeyType_,ValueType_>* data_array_;          //数据数组指针
    bucket_type<IndexType_>*        bucket_array_;        //hash数据数组指针
    std::pair<KeyType_,ValueType_*> iteator_;             //当前节点
    node_type                       *curnode_;            //节点
};

/**
 * 内存管理器
 */
template<typename KeyType_, typename ValueType_,typename IndexType_,std::size_t Cap_ = 0>
class node_pool
{
public:
    typedef RBTNode<KeyType_,IndexType_> node_type;
    typedef RBTree<KeyType_,ValueType_,IndexType_,Cap_> tree_type;
    typedef node_list_iterator<KeyType_,ValueType_,IndexType_,Cap_> iterator;
    typedef const_node_list_iterator<KeyType_,ValueType_,IndexType_,Cap_> const_iterator;
    typedef std::pair<KeyType_,ValueType_> class_type;
    typedef class_type *pointer;
    typedef const class_type  *const_pointer;
    typedef class_type &reference;
    typedef const class_type &const_reference;

    //构造函数
    node_pool()
    {
        size_ = 0;
    }

    //析构函数
    ~node_pool()
    {

    }

    //清理
    void clear()
    {
        for(int index = 0;index < size_;index++)
        {
            data_array_[index].value().~ValueType_();
        }
        //构造空闲链表信息
        node_array_[0].clear();
        //设置前向节点为空
        node_array_[0].set_prev(0);
        node_array_[0].set_data(1);
        for (IndexType_ i = 1; i < Cap_; i++) {
            node_array_[i - 1].set_next(i + 1/*真正的索引+1*/);
            node_array_[i].clear();
            node_array_[i].set_data(i + 1);
            node_array_[i].set_prev(i - 1 + 1/*真正的索引+1*/);
        }
        size_ = 0;
        //设置后向节点为空
        node_array_[Cap_ - 1].set_next(0);
        //已用的节点链表头节点的索引
        rb_tree_head_bucket_ = 0;
        //默认数组首个元素即可用节点链表的头结点
        free_node_head_ = 1;
    }

    //内存池当前大小
    std::size_t size() const
    {
        return size_;
    }

    //内存池当前容量
    std::size_t cap() const
    {
        return Cap_;
    }

    //申请一个可用节点
    node_type *allocate_node(const class_type &v)
    {
        node_type *p = allocate(v);
        if (p)
        {
            return p;
        }
        return 0;
    }

    //申请一个可用节点
    node_type *allocate_node(const class_type &v, node_type *next_node)
    {
        node_type *p = allocate(v);
        if (p) {
            return p;
        }
        return 0;
    }

    void deallocate_node(node_type *node_)
    {
        //回收内存空间
        deallocate(node_);
    }

    node_type *get_node(IndexType_ index)
    {
        if (index > 0 && index <= Cap_)
        {
            return &node_array_[index - 1];
        }
        return NULL;
    }

    IndexType_ get_cur(node_type* node)
    {
        return ARRAY_OFFSET(node_array_,node);
    }

    void updateValue(node_type* node,ValueType_ value)
    {
        IndexType_ index = node->get_data();
        if(index > 0 && index <= Cap_)
        {
            //调用析构函数
            data_array_[index - 1].value() = value;
        }
    }

    tree_type make_rbtree(IndexType_ root)
    {
        return tree_type(node_array_,root);
    }

    iterator make_iterator(IndexType_ root,bucket_type<IndexType_>* pbucket)
    {
        return iterator(root,node_array_,data_array_,pbucket);
    }

    iterator make_iterator(node_type* node,bucket_type<IndexType_>* pbucket)
    {
        return iterator(node,node_array_,data_array_,pbucket);
    }

    const_iterator make_iterator(IndexType_ root,bucket_type<IndexType_>* pbucket) const
    {
        return const_iterator (root,node_array_,data_array_,pbucket);
    }

    const_iterator make_iterator(node_type* node,bucket_type<IndexType_>* pbucket) const
    {
        return const_iterator(node,node_array_,data_array_,pbucket);
    }

    void set_rb_tree_head_bucket(IndexType_ value)
    {
        rb_tree_head_bucket_ = value;
    }

    IndexType_ rb_tree_head_bucket()
    {
        return rb_tree_head_bucket_;
    }
    // 下面为访问已经分配对象的iterator
    iterator begin(bucket_type<IndexType_>* pbucket)
    {
        if (rb_tree_head_bucket_ != 0) {
            return iterator(rb_tree_head_bucket_, node_array_, data_array_,pbucket);
        }
        else {
            return end();
        }
    }

    iterator end()
    {
        return iterator();
    }

    const_iterator begin(bucket_type<IndexType_>* pbucket) const
    {
        if (rb_tree_head_bucket_ != 0) {
            return const_iterator(rb_tree_head_bucket_, node_array_, data_array_,pbucket);
        }
        else {
            return end();
        }
    }

    const_iterator end() const
    {
        return iterator();
    }

    void resetMapFromBinary(ValueNode<KeyType_,ValueType_>* data)
    {
        memcpy(data_array_,data,sizeof(ValueNode<KeyType_,ValueType_>) * Cap_);
    }

    ValueNode<KeyType_,ValueType_>* data()
    {
        return  data_array_;
    }
private:
    //申请空间
    node_type *allocate(const class_type &v)
    {
        if (size_ >= Cap_)
        {
            return 0;
        }
        node_type *p = get_node(free_node_head_);
        if (p)
        {
            size_++;
            //call c++ placement new
            //把空闲头结点指向当前空闲头结点的下一个节点
            free_node_head_ = p->get_next();
            node_type *newhead = get_node(free_node_head_);
            if(newhead != NULL)
            {
                newhead->set_prev(0);
            }
            p->set_key(v.first);
            IndexType_ index = p->get_data();
            if(index > 0 && index <= Cap_)
            {
                new(data_array_[index - 1].data) ValueType_(v.second);
            }
            p->init_rb();
            return p;
        }
        else
        {
            return NULL;
        }
    }

    void deallocate(node_type *node_)
    {
        if (node_ == NULL)
        {
            return;
        }
        IndexType_ index = node_->get_data();
        if(index > 0 && index <= Cap_)
        {
            //调用析构函数
            data_array_[index - 1].value().~ValueType_();
        }
        //插入空闲链表头部
        insert_node(get_node(free_node_head_), node_);
        size_--;
        free_node_head_ = get_cur(node_);
    }

    //插入一个节点到指定节点前
    void insert_node(node_type *tar_node, node_type *new_head)
    {
        if (new_head == NULL)
        {
            return;
        }
        if(tar_node)
        {
            int tar_prev = tar_node->get_prev();
            node_type *tar_prev_node = get_node(tar_prev);
            //有前节点
            if (tar_prev_node)
            {
                //目标节点前指针指向要插入的节点
                tar_prev_node->set_next(get_cur(new_head));
                //要插入的节点前指针指向tar_prev_node
                new_head->set_prev(get_cur(tar_prev_node));
            }
            else
            {
                //要插入的节点前指针置为0
                new_head->set_prev(0);
            }
            tar_node->set_prev(get_cur(new_head));
            new_head->set_next(get_cur(tar_node));
        }else
        {
            new_head->dis_from_list();
        }
    }
private:
    IndexType_                  size_;                            //内存池已用数量
    IndexType_                  rb_tree_head_bucket_;             //红黑树链的头的根节点索引
    IndexType_                  free_node_head_;                  //空闲的节点链表头节点的索引
    //这里为了序列化map的时候能够快速的序列化整个数据的内存块，把真正的数据和红黑树节点信息分成两个数组
    //可以相互通过同一个索引快速取到对应的信息
    node_type                   node_array_[Cap_];
    ValueNode<KeyType_,ValueType_>       data_array_[Cap_];
};
}

#endif //RBTARR_MAP_HASH_HELPER_H
