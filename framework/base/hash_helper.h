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
    IndexType_ root_;  			    //��ͬbucket rbtree���ڵ�����
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
 *������class
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
        //û�пɱ����Ľڵ���
        if(bucket_ <= 0 || bucket_ > Cap_)
        {
            curnode_ = NULL;
            return;
        }
        //Ѱ�ҵ�һ���ڵ�,��������С�ڵ㿪ʼ����ÿһ���������нڵ�
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
            //��ǰ������������
            if(curnode_ == NULL)
            {
                //������һ����
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
    IndexType_                      bucket_;                //������ĸ��ڵ�
    node_type*                      node_array_;          //�ڵ�����������
    ValueNode<KeyType_,ValueType_>* data_array_;          //��������ָ��
    bucket_type<IndexType_>*        bucket_array_;        //hash��������ָ��
    std::pair<KeyType_,ValueType_*> iteator_;             //��ǰ�ڵ�
    node_type                       *curnode_;            //�ڵ�
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
        //û�пɱ����Ľڵ���
        if(bucket_ <= 0 || bucket_ > Cap_)
        {
            curnode_ = NULL;
            return;
        }
        //Ѱ�ҵ�һ���ڵ�,��������С�ڵ㿪ʼ����ÿһ���������нڵ�
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
            //��ǰ������������
            if(curnode_ == NULL)
            {
                //������һ����
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
    IndexType_                      bucket_;                //������ĸ��ڵ�
    node_type*                      node_array_;          //�ڵ�����������
    ValueNode<KeyType_,ValueType_>* data_array_;          //��������ָ��
    bucket_type<IndexType_>*        bucket_array_;        //hash��������ָ��
    std::pair<KeyType_,ValueType_*> iteator_;             //��ǰ�ڵ�
    node_type                       *curnode_;            //�ڵ�
};

/**
 * �ڴ������
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

    //���캯��
    node_pool()
    {
        size_ = 0;
    }

    //��������
    ~node_pool()
    {

    }

    //����
    void clear()
    {
        for(int index = 0;index < size_;index++)
        {
            data_array_[index].value().~ValueType_();
        }
        //�������������Ϣ
        node_array_[0].clear();
        //����ǰ��ڵ�Ϊ��
        node_array_[0].set_prev(0);
        node_array_[0].set_data(1);
        for (IndexType_ i = 1; i < Cap_; i++) {
            node_array_[i - 1].set_next(i + 1/*����������+1*/);
            node_array_[i].clear();
            node_array_[i].set_data(i + 1);
            node_array_[i].set_prev(i - 1 + 1/*����������+1*/);
        }
        size_ = 0;
        //���ú���ڵ�Ϊ��
        node_array_[Cap_ - 1].set_next(0);
        //���õĽڵ�����ͷ�ڵ������
        rb_tree_head_bucket_ = 0;
        //Ĭ�������׸�Ԫ�ؼ����ýڵ������ͷ���
        free_node_head_ = 1;
    }

    //�ڴ�ص�ǰ��С
    std::size_t size() const
    {
        return size_;
    }

    //�ڴ�ص�ǰ����
    std::size_t cap() const
    {
        return Cap_;
    }

    //����һ�����ýڵ�
    node_type *allocate_node(const class_type &v)
    {
        node_type *p = allocate(v);
        if (p)
        {
            return p;
        }
        return 0;
    }

    //����һ�����ýڵ�
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
        //�����ڴ�ռ�
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
            //������������
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
    // ����Ϊ�����Ѿ���������iterator
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
    //����ռ�
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
            //�ѿ���ͷ���ָ��ǰ����ͷ������һ���ڵ�
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
            //������������
            data_array_[index - 1].value().~ValueType_();
        }
        //�����������ͷ��
        insert_node(get_node(free_node_head_), node_);
        size_--;
        free_node_head_ = get_cur(node_);
    }

    //����һ���ڵ㵽ָ���ڵ�ǰ
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
            //��ǰ�ڵ�
            if (tar_prev_node)
            {
                //Ŀ��ڵ�ǰָ��ָ��Ҫ����Ľڵ�
                tar_prev_node->set_next(get_cur(new_head));
                //Ҫ����Ľڵ�ǰָ��ָ��tar_prev_node
                new_head->set_prev(get_cur(tar_prev_node));
            }
            else
            {
                //Ҫ����Ľڵ�ǰָ����Ϊ0
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
    IndexType_                  size_;                            //�ڴ����������
    IndexType_                  rb_tree_head_bucket_;             //���������ͷ�ĸ��ڵ�����
    IndexType_                  free_node_head_;                  //���еĽڵ�����ͷ�ڵ������
    //����Ϊ�����л�map��ʱ���ܹ����ٵ����л��������ݵ��ڴ�飬�����������ݺͺ�����ڵ���Ϣ�ֳ���������
    //�����໥ͨ��ͬһ����������ȡ����Ӧ����Ϣ
    node_type                   node_array_[Cap_];
    ValueNode<KeyType_,ValueType_>       data_array_[Cap_];
};
}

#endif //RBTARR_MAP_HASH_HELPER_H
