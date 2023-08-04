//
// * C++ 语言: 红黑树
// Created by dguco on 2022/4/13.
// https://www.cnblogs.com/skywang12345/p/3245399.html
// https://www.cnblogs.com/skywang12345/p/3603935.html
//

#ifndef RBTARR_MAP_RB_TREE_H
#define RBTARR_MAP_RB_TREE_H

#include <iomanip>
#include <iostream>
#include <list>
using namespace std;

#define RB_TREE_JAVA 1
#define DEBUG_RB_TREE 0
#define NodeType_ RBTNode<KeyType_,INDEX_TYPE>
#define ARRAY_OFFSET(array,node) (node - array + 1)   //这里取真实的索引+1作为数组索引[1,Cap_]

enum RBTColor
{
    RB_NONE = 0,
    RB_RED = 1,
    RB_BLACK = 2,
};

template<typename KeyType_,typename T>
struct ValueNode
{
    char data[sizeof(T)];
    inline T* pointer()                       { return (T*)data; }
    inline T& value()                         { return *((T*)data); }
    KeyType_ Key()
    {
        return value().Key();
    }

    bool IsValid()
    {
        return value().IsValid();;
    }
};

/**
 *单个节点类
* */
template<typename KeyType_,typename INDEX_TYPE = unsigned int>
struct RBTNode
{
public:
    RBTNode()
    {
        clear();
    }

    ~RBTNode()
    {
       clear();
    }

    void init_rb()
    {
        parent_ = 0;
        left_ = 0;
        right_ = 0;
        color_ = RB_BLACK;
    }

    void clear_rb()
    {
        parent_ = 0;
        left_ = 0;
        right_ = 0;
        color_ = RB_NONE;
    }

    void clear()
    {
        key_ = 0;
        parent_ = 0;
        left_ = 0;
        right_ = 0;
        color_ = RB_NONE;
        data_ = 0;
    }

    inline void set_prev(INDEX_TYPE value)   { left_ = value;}
    inline INDEX_TYPE  get_prev()            { return left_;}

    inline void set_next(INDEX_TYPE value)   { right_ = value;}
    inline INDEX_TYPE  get_next()            { return right_;}

    inline void set_parent(INDEX_TYPE value) { parent_ = value;}
    inline INDEX_TYPE  get_parent()          { return parent_;}

    inline void set_left(INDEX_TYPE value)   { left_ = value;}
    inline INDEX_TYPE  get_left()            { return left_;}

    inline void set_right(INDEX_TYPE value)  { right_ = value;}
    inline INDEX_TYPE  get_right()           { return right_;}

    inline void set_color(RBTColor value)    { color_ = value;}
    inline RBTColor  get_color()             { return (RBTColor)color_;}

    inline void set_key(KeyType_ key)       {  key_ = key;}
    inline KeyType_      get_key()           {  return  key_;}

    inline void set_data(INDEX_TYPE value)       {  data_ = value;}
    inline INDEX_TYPE      get_data()            {  return  data_;}

    void dis_from_list()
    {
        set_next(0);
        set_prev(0);
        set_parent(0);
    }

public:
    RBTNode(KeyType_ value, RBTColor c, INDEX_TYPE p, INDEX_TYPE l, INDEX_TYPE r,INDEX_TYPE data)
        : key_(value),color_(c), parent_(p), left_(l), right_(r),data_(data)
    {}
private:
    KeyType_                            key_;                          //节点对象信息
    INDEX_TYPE                          parent_;                       //父亲节点在数组中的索引位置+1
    INDEX_TYPE                          left_;                         //左子节点在数组中的索引位置+1
    INDEX_TYPE                          right_;                        //右子节点在数组中的索引位置+1
    INDEX_TYPE                          data_;                         //data在数组中的索引位置+1
    unsigned  char                      color_;                        //节点color
};

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE = unsigned int,std::size_t Cap_ = 0>
class RBTree
{
public:
    RBTree(NodeType_ *pool,INDEX_TYPE root = 0);
    ~RBTree();
    bool isRBTree();
    // 前序遍历"红黑树"
    void preOrder(std::list<NodeType_*>& resList);
    // 中序遍历"红黑树"
    void inOrder(std::list<NodeType_*>& resList);
    // 后序遍历"红黑树"
    void postOrder(std::list<NodeType_*>& resList);
    // (递归实现)查找"红黑树"中键值为key的节点
    NodeType_ *search(KeyType_ key);
    // (非递归实现)查找"红黑树"中键值为key的节点
    NodeType_ *iterativeSearch(KeyType_ key);
    // 查找最小结点：返回最小结点的键值。
    NodeType_ * minimum();
    // 查找最大结点：返回最大结点的键值。
    NodeType_ * maximum();
    // 找结点(x)的后继结点。即，查找"红黑树中数据值大于该结点"的"最小结点"。
    NodeType_ *successor(NodeType_ *x);
    // 找结点(x)的前驱结点。即，查找"红黑树中数据值小于该结点"的"最大结点"。
    NodeType_ *predecessor(NodeType_ *x);
    // 插入函数
    void insert(NodeType_ *node);
    // 删除结点(key为节点键值)
    NodeType_* remove(KeyType_ key);
    // 删除函数
    NodeType_* remove(NodeType_ *node);
    // 打印红黑树
    void print();
    bool isEmpty();
    INDEX_TYPE root();
private:
    bool isRBTree(NodeType_* root, int blacknum, int count);
    // 前序遍历"红黑树"
    void preOrder(NodeType_ *tree, std::list<NodeType_*>& resList) ;
    // 中序遍历"红黑树"
    void inOrder(NodeType_ *tree, std::list<NodeType_*>& resList) ;
    // 后序遍历"红黑树"
    void postOrder(NodeType_ *tree, std::list<NodeType_*>& resList) ;
    // (递归实现)查找"红黑树x"中键值为key的节点
    NodeType_ *search(NodeType_ *x, KeyType_ key);
    // (非递归实现)查找"红黑树x"中键值为key的节点
    NodeType_ *iterativeSearch(NodeType_ *x, KeyType_ key);
    // 查找最小结点：返回tree为根结点的红黑树的最小结点。
    NodeType_ *minimum(NodeType_ *tree);
    // 查找最大结点：返回tree为根结点的红黑树的最大结点。
    NodeType_ *maximum(NodeType_ *tree);
    // 左旋
    void leftRotate(NodeType_ *x);
    // 右旋
    void rightRotate(NodeType_ *y);
    // 插入修正函数
    void insertFixUp(NodeType_ *node);
    // 删除修正函数
    void removeFixUp(NodeType_ *node);
    void removeFixUp(NodeType_ *node, NodeType_ *parent);
    // 打印红黑树
    void print(NodeType_ *tree, KeyType_ key, int direction);
public:
    inline NodeType_* getNode(INDEX_TYPE index);
    inline NodeType_ *parentOf(NodeType_ *node);
    inline void setParent(NodeType_ *node, NodeType_ *parent);
    inline NodeType_ *leftOf(NodeType_ *node);
    inline void setLeft(NodeType_ *node, NodeType_ *left);
    inline NodeType_ *rightOf(NodeType_ *node);
    inline void setRight(NodeType_ *node, NodeType_ *right);
    inline INDEX_TYPE curOf(NodeType_ *node);
    static inline bool isRed(NodeType_ *node);
    static inline bool isBlack(NodeType_ *node);
    static inline void setRed(NodeType_ *node);
    static inline void setBlack(NodeType_ *node);
    static inline RBTColor rbColorOf(NodeType_ *node);
    static inline void setRbColor(NodeType_ *node, RBTColor color);
private:
    NodeType_               *m_Pool;    //
    INDEX_TYPE               m_Root;
};

/*
 * 构造函数
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::RBTree(NodeType_  *pool,INDEX_TYPE root)
    :m_Pool(pool),m_Root(root)
{
}

/*
 * 析构函数
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::~RBTree()
{
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
bool RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::isRBTree()
{
    //空树
    if (getNode(m_Root) == nullptr)
    {
        return true;
    }
    //根节点为黑色
    if (isRed(getNode(m_Root)))
    {
        cout << "根节点为红色" << endl;
        return false;
    }
    //黑色结点数量各路径上相同
    //先走一条得到基准值
    int Blacknum = 0;
    NodeType_* cur = getNode(m_Root);
    while (cur)
    {
        if (isBlack(cur))
            Blacknum++;
        cur = leftOf(cur);
    }
    //检查子树
    int i = 0;
    return isRBTree(getNode(m_Root), Blacknum, i);
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
bool RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::isRBTree(NodeType_* root, int blacknum, int count)
{
    //递归到空节点
    if (root == nullptr)
    {
        if (blacknum == count)
            return true;
        cout << "各路径上黑色节点个数不同" << endl;
        return false;
    }
    //子节点为红则检查父节点是否为红（通过父节点检查子节点会遇到空节点）
    if (isRed(root) && isRed(parentOf(root)))
    {
        cout << "存在连续红色节点" << endl;
        return false;
    }
    //计数黑结点
    if (isBlack(root))
        count++;
    //递归左右子树
    return isRBTree(leftOf(root), blacknum, count) && isRBTree(rightOf(root), blacknum, count);
}

/*
 * 前序遍历"红黑树"
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::preOrder(NodeType_ *tree, std::list<NodeType_*>& resList)
{
    if (tree != NULL) {
        if(DEBUG_RB_TREE)
        {
            cout << tree->get_key() << " ";
        }
        resList.push_back(tree);
        preOrder(leftOf(tree),resList);
        preOrder(rightOf(tree),resList);
    }
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::preOrder(std::list<NodeType_*>& resList)
{
    preOrder(getNode(m_Root),resList);
}

/*
 * 中序遍历"红黑树"
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::inOrder(NodeType_ *tree, std::list<NodeType_*>& resList)
{
    if (tree != NULL) {
        inOrder(leftOf(tree),resList);
        if(DEBUG_RB_TREE)
        {
            cout << tree->get_key() << " ";
        }
        resList.push_back(tree);
        inOrder(rightOf(tree),resList);
    }
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::inOrder(std::list<NodeType_*>& resList)
{
    inOrder(getNode(m_Root),resList);
}

/*
 * 后序遍历"红黑树"
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::postOrder(NodeType_ *tree, std::list<NodeType_*>& resList)
{
    if (tree != NULL)
    {
        postOrder(leftOf(tree),resList);
        postOrder(rightOf(tree),resList);
        if(DEBUG_RB_TREE)
        {
            cout << tree->get_key() << " ";
        }
        resList.push_back(tree);
    }
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::postOrder(std::list<NodeType_*>& resList)
{
    postOrder(getNode(m_Root),resList);
}

/*
 * (递归实现)查找"红黑树x"中键值为key的节点
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_ *RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::search(NodeType_ *x, KeyType_ key)
{
    if (x == NULL || x->get_key() == key)
        return x;

    if (key < x->get_key())
        return search(leftOf(x), key);
    else
        return search(rightOf(x), key);
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_ *RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::search(KeyType_ key)
{
    return search(getNode(m_Root), key);
}

/*
 * (非递归实现)查找"红黑树x"中键值为key的节点
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_ *RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::iterativeSearch(NodeType_ *x, KeyType_ key)
{
    while ((x != NULL) && (x->key != key)) {
        if (key < x->key)
            x = x->left;
        else
            x = x->right;
    }

    return x;
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_ *RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::iterativeSearch(KeyType_ key)
{
    iterativeSearch(getNode(m_Root), key);
}

/*
 * 查找最小结点：返回tree为根结点的红黑树的最小结点。
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_ *RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::minimum(NodeType_ *tree)
{
    if (tree == NULL)
        return NULL;

    while (leftOf(tree) != NULL)
        tree = leftOf(tree);
    return tree;
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_ * RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::minimum()
{
    NodeType_ *p = minimum(getNode(m_Root));
    if (p != NULL)
        return p;

    return NULL;
}

/*
 * 查找最大结点：返回tree为根结点的红黑树的最大结点。
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_ *RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::maximum(NodeType_ *tree)
{
    if (tree == NULL)
        return NULL;

    while (rightOf(tree) != NULL)
        tree = rightOf(tree);
    return tree;
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_ * RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::maximum()
{
    NodeType_ *p = maximum(getNode(m_Root));
    if (p != NULL)
        return p;
    return NULL;
}

/*
 * 找结点(x)的后继结点。即，查找"红黑树中数据值大于该结点"的"最小结点"。
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_ *RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::successor(NodeType_ *x)
{
    if(x == NULL)
    {
        return NULL;
    }

    // 如果x存在右孩子，则"x的后继结点"为 "以其右孩子为根的子树的最小结点"。
    if (rightOf(x) != NULL)
        return minimum(rightOf(x));

    // 如果x没有右孩子。则x有以下两种可能：
    // (01) x是"一个左孩子"，则"x的后继结点"为 "它的父结点"。
    // (02) x是"一个右孩子"，则查找"x的最低的父结点，并且该父结点要具有左孩子"，找到的这个"最低的父结点"就是"x的后继结点"。
    NodeType_ *y = parentOf(x);
    while ((y != NULL) && (x == rightOf(y)))
    {
        x = y;
        y = parentOf(y);
    }
    return y;
}

/*
 * 找结点(x)的前驱结点。即，查找"红黑树中数据值小于该结点"的"最大结点"。
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_ *RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::predecessor(NodeType_ *x)
{
    // 如果x存在左孩子，则"x的前驱结点"为 "以其左孩子为根的子树的最大结点"。
    if (x->left != NULL)
        return maximum(x->left);

    // 如果x没有左孩子。则x有以下两种可能：
    // (01) x是"一个右孩子"，则"x的前驱结点"为 "它的父结点"。
    // (01) x是"一个左孩子"，则查找"x的最低的父结点，并且该父结点要具有右孩子"，找到的这个"最低的父结点"就是"x的前驱结点"。
    NodeType_ *y = x->parent;
    while ((y != NULL) && (x == y->left)) {
        x = y;
        y = y->parent;
    }

    return y;
}

/*
 * 对红黑树的节点(x)进行左旋转
 * 左旋示意图(对节点x进行左旋)：
 *      px                              px
 *     /                               /
 *    x                               y
 *   /  \      --(左旋)-->           / \                #
 *  lx   y                          x  ry
 *     /   \                       /  \
 *    ly   ry                     lx  ly
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::leftRotate(NodeType_ *x)
{
    // 设置x的右孩子为y
    NodeType_ *y = rightOf(x);

    // 将 “y的左孩子” 设为 “x的右孩子”；
    // 如果y的左孩子非空，将 “x” 设为 “y的左孩子的父亲”
    setRight(x, leftOf(y));
    if (leftOf(y) != NULL)
        setParent(leftOf(y),x);

    // 将 “x的父亲” 设为 “y的父亲”
    setParent(y, parentOf(x));

    if (parentOf(x) == NULL) {
        m_Root = curOf(y);            // 如果 “x的父亲” 是空节点，则将y设为根节点
    }
    else {
        if (leftOf(parentOf(x)) == x)
            setLeft(parentOf(x),y); //如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
        else
            setRight(parentOf(x),y); // 如果 x是它父节点的右孩子，则将y设为“x的父节点的右孩子”
    }

    // 将 “x” 设为 “y的左孩子”
    setLeft(y,x);
    // 将 “x的父节点” 设为 “y”
    setParent(x,y);
}

/*
 * 对红黑树的节点(y)进行右旋转
 * 右旋示意图(对节点y进行左旋)：
 *            py                               py
 *           /                                /
 *          y                                x
 *         /  \      --(右旋)-->            /  \                     #
 *        x   ry                           lx   y
 *       / \                                   / \                   #
 *      lx  rx                                rx  ry
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::rightRotate(NodeType_ *y)
{
    // 设置x是当前节点的左孩子。
    NodeType_ *x = leftOf(y);

    // 将 “x的右孩子” 设为 “y的左孩子”；
    // 如果"x的右孩子"不为空的话，将 “y” 设为 “x的右孩子的父亲”
    setLeft(y, rightOf(x));
    if (rightOf(x) != NULL)
        setParent(rightOf(x),y);

    // 将 “y的父亲” 设为 “x的父亲”
    setParent(x, parentOf(y));

    if (parentOf(y) == NULL) {
        m_Root = curOf(x);            // 如果 “y的父亲” 是空节点，则将x设为根节点
    }
    else {
        if (y == rightOf(parentOf(y)))
            setRight(parentOf(y),x);// 如果 y是它父节点的右孩子，则将x设为“y的父节点的右孩子”
        else
            setLeft(parentOf(y),x); // (y是它父节点的左孩子) 将x设为“x的父节点的左孩子”
    }

    // 将 “y” 设为 “x的右孩子”
    setRight(x,y);
    // 将 “y的父节点” 设为 “x”
    setParent(y,x);
}

/*
 * 红黑树插入修正函数
 *
 * 在向红黑树中插入节点之后(失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 插入的结点    对应《算法导论》中的z
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::insertFixUp(NodeType_ *node)
{
    NodeType_ *parent, *gparent;

    // 若父节点存在，并且父节点的颜色是红色,根节点是黑色(被插入节点是一定存在非空祖父节点)
    parent = parentOf(node);
    while (parent && isRed(parent))
    {
        gparent = parentOf(parent);
        //若父节点是祖父节点的左孩子
        if (parent == leftOf(gparent))
        {
            // Case 1：叔叔节点是红色
            NodeType_ *uncle = rightOf(gparent);
            if (isRed(uncle))
            {
                setBlack(parent);//  (01) 将“父节点”设为黑色。
                setBlack(uncle); //  (02) 将“叔叔节点”设为黑色。
                setRed(gparent); //  (03) 将“祖父节点”设为“红色”。
                node = gparent;      //  (04) 将“祖父节点”设为“当前节点”(红色节点)
            }else  //红黑树特性,空节点或者黑色的非空节点都为黑色节点(叔叔是黑色)
            {
                if (rightOf(parent) == node)
                {
                    node = parent;
                    leftRotate(node);
                }
                //如意如果parent->right == node，这里的rb_parent(node)是对左旋之后的树进行操作，最终的结果和方法1是一样的
                setBlack(parentOf(node));
                setRed(parentOf(parentOf(node)));
                rightRotate(parentOf(parentOf(node)));
            }
        }
        else//若父节点是祖父节点的右孩子,将上面的操作中“rightRotate”和“leftRotate”交换位置，然后依次执行。
        {
            // Case 1条件：叔叔节点是红色
            NodeType_ *uncle = leftOf(gparent);
            if (isRed(uncle))
            {
                setBlack(parent); //  (01) 将“父节点”设为黑色。
                setBlack(uncle);  //  (02) 将“叔叔节点”设为黑色。
                setRed(gparent);  //  (03) 将“祖父节点”设为“红色”。
                node = gparent;       //  (04) 将“祖父节点”设为“当前节点”(红色节点)
            }else //红黑树特性,空节点或者黑色的非空节点都为黑色节点(叔叔是黑色)
            {
                if (leftOf(parent) == node)
                {
                    node = parent;
                    rightRotate(node);
                }
                setBlack(parentOf(node));
                setRed(parentOf(parentOf(node)));
                leftRotate(parentOf(parentOf(node)));
            }
        }
        parent = parentOf(node);
    }

    // 将根节点设为黑色
    setBlack(getNode(m_Root));
}

/*
 * 将结点插入到红黑树中
 *
 * 参数说明：
 *     root 红黑树的根结点
 *     node 插入的结点        // 对应《算法导论》中的node
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::insert(NodeType_ *node)
{
    NodeType_ *parent_node = NULL;
    NodeType_ *tmp_node = getNode(m_Root);

    //将红黑树当作一颗二叉查找树，将节点添加到二叉查找树中。
    {
        //存在root节点,先找新节点的父亲节点在哪
        while (tmp_node != NULL)
        {
            parent_node = tmp_node;
            //新节点比当前节点key小，往左子树开始找
            if (node->get_key() < tmp_node->get_key())
                tmp_node = leftOf(tmp_node);
            else //新节点比当前节点key大，往右子树开始找
                tmp_node = rightOf(tmp_node);
        }

        //把新节点父亲节点指向找到的节点
        setParent(node,parent_node);
        //如果有根节点
        if (parent_node != NULL)
        {
            //新节点比当前节点key小，插入父亲节点的左边
            if (node->get_key() < parent_node->get_key())
                setLeft(parent_node,node);
            else //新节点比当前节点key大，插入父亲节点的右边
                setRight(parent_node,node);
        }
        else
            m_Root = curOf(node);
    }

    //设置节点的颜色为红色
    setRed(node);
    //将它重新修正为一颗二叉查找树
    insertFixUp(node);
}

/*
 * 红黑树删除修正函数
 *
 * 在从红黑树中删除插入节点之后(红黑树失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 待修正的节点
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::removeFixUp(NodeType_ *node)
{
    NodeType_ *other;

    while (node != getNode(m_Root) && isBlack(node))
    {
        //若node是“它父节点的左孩子”，则设置 “other”为“x的叔叔”(即x为它父节点的右孩子)
        if (leftOf(parentOf(node)) == node)
        {
            other = rightOf(parentOf(node));
            //Case 1: x是“黑+黑”节点，x的兄弟节点是红色。(此时x的父节点和x的兄弟节点的子节点都是黑节点)。
            if (isRed(other))
            {
                setBlack(other);              //  (01) 将x的兄弟节点设为“黑色”。
                setRed(parentOf(node)); //   (02) 将x的父节点设为“红色”。
                leftRotate(parentOf(node));//   (03) 对x的父节点进行左旋。
                other = rightOf(parentOf(node));// (04) 左旋后，重新设置x的兄弟节点。
            }
            // Case 2: x是“黑+黑”节点，x的兄弟节点是黑色，x的兄弟节点的两个孩子都是黑色。
            if (isBlack(leftOf(other)) && isBlack(rightOf(other)))
            {
                setRed(other);          //   (01) 将x的兄弟节点设为“红色”。
                node = parentOf(node);  //   (02) 设置“x的父节点”为“新的x节点”。
            }
            else {
                // Case 3: x是“黑+黑”节点，x的兄弟节点是黑色；x的兄弟节点的左孩子是红色，右孩子是黑色的。
                if (isBlack(rightOf(other)))
                {
                    setBlack(leftOf(other));              //   (01) 将x兄弟节点的左孩子设为“黑色”。
                    setRed(other);                              //   (02) 将x兄弟节点设为“红色”。
                    rightRotate(other);                         //   (03) 对x的兄弟节点进行右旋。
                    other = rightOf(parentOf(node));      //   (04) 右旋后，重新设置x的兄弟节点。
                }
                // Case 4: x是“黑+黑”节点，x的兄弟节点是黑色；x的兄弟节点的右孩子是红色的。
                setRbColor(other, rbColorOf(parentOf(node)));// (01) 将x父节点颜色 赋值给 x的兄弟节点。
                setBlack(parentOf(node));//   (02) 将x父节点设为“黑色”。
                setBlack(rightOf(other));//   (03) 将x兄弟节点的右子节设为“黑色”。
                leftRotate(parentOf(node)); //   (04) 对x的父节点进行左旋。
                node = getNode(m_Root);        //   (05) 设置“x”为“根节点”。
            }
        }
        else {
            // 若 “x”是“它父节点的右孩子”，将上面的操作中“right”和“left”交换位置，然后依次执行。
            other = leftOf(parentOf(node));
            if (isRed(other))
            {
                // Case 1: x的兄弟w是红色的
                setBlack(other);
                setRed(parentOf(node));
                rightRotate(parentOf(node));
                other = leftOf(parentOf(node));
            }
            if (isBlack(leftOf(other)) && isBlack(rightOf(other)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
                setRed(other);
                node = parentOf(node);
            }
            else {
                if (isBlack(leftOf(other)))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
                    setBlack(rightOf(other));
                    setRed(other);
                    leftRotate(other);
                    other = leftOf(parentOf(node));
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                setRbColor(other, rbColorOf(parentOf(node)));
                setBlack(parentOf(node));
                setBlack(leftOf(other));
                rightRotate(parentOf(node));
                node = getNode(m_Root);
            }
        }
    }
    if (node)
        setBlack(node);
}

/*
 * 红黑树删除修正函数
 *
 * 在从红黑树中删除插入节点之后(红黑树失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 待修正的节点
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::removeFixUp(NodeType_ *node, NodeType_ *parent)
{
    NodeType_ *other;
    while ((!node || isBlack(node)) && curOf(node) != m_Root)
    {
        if (leftOf(parent) == node)
        {
            other = rightOf(parent);
            if (isRed(other))
            {
                // Case 1: x的兄弟w是红色的
                setBlack(other);
                setRed(parent);
                leftRotate(parent);
                other = rightOf(parent);
            }
            if (isBlack(leftOf(other)) && isBlack(rightOf(other)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
                setRed(other);
                node = parent;
                parent = parentOf(node);
            }
            else
            {
                if (isBlack(rightOf(other)))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
                    setBlack(leftOf(other));
                    setRed(other);
                    rightRotate(other);
                    other = rightOf(parent);
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                setRbColor(other, rbColorOf(parent));
                setBlack(parent);
                setBlack(rightOf(other));
                leftRotate(parent);
                node = getNode(m_Root);
                break;
            }
        }
        else
        {
            other = leftOf(parent);
            if (isRed(other))
            {
                // Case 1: x的兄弟w是红色的
                setBlack(other);
                setRed(parent);
                rightRotate(parent);
                other = leftOf(parent);
            }
            if (isBlack(leftOf(other)) && isBlack(rightOf(other)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
                setRed(other);
                node = parent;
                parent = parentOf(node);
            }
            else
            {
                if (isBlack(leftOf(other)))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
                    setBlack(rightOf(other));
                    setRed(other);
                    leftRotate(other);
                    other = leftOf(parent);
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                setRbColor(other, rbColorOf(parent));
                setBlack(parent);
                setBlack(leftOf(other));
                rightRotate(parent);
                node = getNode(m_Root);
                break;
            }
        }
    }
    if (node)
        setBlack(node);
}
/*
 * 删除结点(node)，并返回被删除的结点
 *
 * 参数说明：
 *     root 红黑树的根结点
 *     node 删除的结点
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_* RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::remove(NodeType_ *node)
{
    /**
     * 两个删除逻辑最重要的区别，java的实现中，假如你要删除a节点，实际上有可能删除的不是a节点，而是b节点，只不过在删除之前会把b节点的内容复制给a，保留a的颜色，
     * 但是第二种实现删除a的过程是通过调整父子节点关系来实现不会有节点的内容复制过程，注意这个很关键，如果选用java的实现删除的时候不光影响红黑树本身的节点关系，
     * 还会打乱节点内部对应的数据引用关系(上层hashmap中节点a中保存的数据引用本身是a删除后红黑树节点a对应的数据引用有可能不再是a而是b)
     */
    if(RB_TREE_JAVA)
    {
        // 被删除节点的"左右孩子都不为空"的情况
        if ((leftOf(node) != NULL) && (rightOf(node) != NULL))
        {
            //寻找删除节点的后继节点,即查找"红黑树中数据值大于该结点"的"最小结点"。
            NodeType_* succNode = successor(node);
            /**
             * 通过把后继节点的内容复制给要删除的节点,让后继节点处于被删除node的位置，且保留node的color(根据后继节点的特性可以知道，把后继
             * 节点的内容复制到删除节点的位置上并不破坏红黑树二叉搜索树的特性)，但是有可能破坏红黑树的平衡特性,下面只要删除后继节点然后调整
             * 红黑树的平衡就可以了
             */
            KeyType_    key = node->get_key();
            INDEX_TYPE  data = node->get_data();
            node->set_key(succNode->get_key());
            node->set_data(succNode->get_data());
            succNode->set_key(key);
            succNode->set_data(data);
            //把要删除的节点换位后继节点,准备删除后继节点
            node = succNode;
        }

        //这里不可能左右都不为空，因为如果左右都不为空删除的节点在前面会转为节点的后继节点（该节点的左节点肯定为空）
        NodeType_* replacement = (leftOf(node) != NULL) ? leftOf(node) : rightOf(node);
        //如果删除的节点有子节点
        if(replacement != NULL)
        {
            //重置删除后的父子节点关系
            setParent(replacement, parentOf(node));
            if (parentOf(node) == NULL)
                m_Root = curOf(replacement); //如果要删除的节点没有父节点，则他的子节点成为新的根节点
            else if (node == leftOf(parentOf(node)))
                setLeft(parentOf(node), replacement); //是父节点的左子树
            else
                setRight(parentOf(node), replacement);

            node->dis_from_list();
            // Fix replacement
            if (isBlack(node))
                removeFixUp(replacement);

        }else if(parentOf(node) == NULL)//如果要删除的节点没有父节点，则删除的节点是根节点,直接把根节点清掉返回
        {
            m_Root = 0;
        }else //No children. Use self as phantom replacement and unlink.
        {
            if(isBlack(node))
            {
                removeFixUp(node);
            }

            if(parentOf(node) != NULL)
            {
                if(node == leftOf(parentOf(node)))
                {
                    setLeft(parentOf(node),NULL);
                }else if(node == rightOf(parentOf(node)))
                {
                    setRight(parentOf(node),NULL);
                }
                setParent(node,NULL);
            }
        }
        node->clear_rb();
        return node;
    }else
    {
        NodeType_ *child, *parent;
        RBTColor color;
        // 被删除节点的"左右孩子都不为空"的情况。
        if ((leftOf(node) != NULL) && (rightOf(node) != NULL))
        {
            // 被删节点的后继节点。(称为"取代节点")
            // 用它来取代"被删节点"的位置，然后再将"被删节点"去掉。
            NodeType_ *replace = node;
            // 获取后继节点
            replace = rightOf(replace);
            while (leftOf(replace) != NULL)
                replace = leftOf(replace);

            // "node节点"不是根节点(只有根节点不存在父节点)
            if (parentOf(node))
            {
                if (leftOf(parentOf(node)) == node)
                    setLeft(parentOf(node),replace);
                else
                    setRight(parentOf(node),replace);
            }
            else
                // "node节点"是根节点，更新根节点。
                m_Root = curOf(replace);

            // child是"取代节点"的右孩子，也是需要"调整的节点"。
            // "取代节点"肯定不存在左孩子！因为它是一个后继节点。
            child = rightOf(replace);
            parent = parentOf(replace);
            // 保存"取代节点"的颜色
            color = rbColorOf(replace);

            // "被删除节点"是"它的后继节点的父节点"
            if (parent == node)
            {
                parent = replace;
            }
            else
            {
                // child不为空
                if (child)
                    setParent(child,parent);
                setLeft(parent,child);
                setRight(replace,rightOf(node));
                setParent(rightOf(node), replace);
            }

            setParent(replace,parentOf(node));
            setRbColor(replace,rbColorOf(node));
            setLeft(replace,leftOf(node));
            setParent(leftOf(node),replace);

            if (color == RB_BLACK)
                removeFixUp(child, parent);

            node->clear_rb();
            return node;
        }

        if (leftOf(node) !=NULL)
            child = leftOf(node);
        else
            child = rightOf(node);

        parent = parentOf(node);
        // 保存"取代节点"的颜色
        color = rbColorOf(node);

        if (child)
            setParent(child,parent);

        // "node节点"不是根节点
        if (parent)
        {
            if (leftOf(parent) == node)
                setLeft(parent,child);
            else
                setRight(parent,child);
        }
        else
            m_Root = curOf(child);

        if (color == RB_BLACK)
            removeFixUp(child, parent);
        node->clear_rb();
        return node;
    }
}

/*
 * 删除红黑树中键值为key的节点
 *
 * 参数说明：
 *     tree 红黑树的根结点
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_* RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::remove(KeyType_ key)
{
    NodeType_ *node = NULL;
    // 查找key对应的节点(node)，找到的话就删除该节点
    if ((node = search(getNode(m_Root), key)) != NULL)
       return remove(node);
    return  NULL;
}

/*
 * 销毁红黑树
 */
//template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
//void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::destroy(NODE_TYPE *&tree)
//{
//    if (tree == NULL)
//        return;
//
//    if (tree->left != NULL)
//        return destroy(tree->left);
//    if (tree->right != NULL)
//        return destroy(tree->right);
//
//    delete tree;
//    tree = NULL;
//}

//template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
//void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::destroy()
//{
//    destroy(mRoot);
//}

/*
 * 打印"二叉查找树"
 *
 * key        -- 节点的键值
 * direction  --  0，表示该节点是根节点;
 *               -1，表示该节点是它的父结点的左孩子;
 *                1，表示该节点是它的父结点的右孩子。
 */
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::print(NodeType_ *tree, KeyType_ key, int direction)
{
    if (tree != NULL) {
        if (direction == 0)    // tree是根节点
            cout << setw(2) << tree->get_key() << "(B) is root" << endl;
        else                // tree是分支节点
            cout << setw(2) << tree->get_key() << ( isRed(tree) ? "(R)" : "(B)") << " is " << setw(2) << key << "'s "
                 << setw(12) << (direction == 1 ? "right child" : "left child") << endl;

        print(leftOf(tree), tree->get_key(), -1);
        print(rightOf(tree), tree->get_key(), 1);
    }
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
NodeType_* RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::getNode(INDEX_TYPE index)
{
    if(index > 0 && index <= Cap_)
    {
        return &m_Pool[index -1];
    }
    return NULL;
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::print()
{
    if (getNode(m_Root) != NULL)
        print(getNode(m_Root), getNode(m_Root)->get_key(), 0);
}
template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
bool RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::isEmpty()
{
    return m_Root == 0;
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
INDEX_TYPE RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::root()
{
    return m_Root;
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline NodeType_* RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::parentOf(NodeType_* node)
{
    return  node == NULL ? NULL : getNode(node->get_parent());
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::setParent(NodeType_ *node, NodeType_ *parent)
{
    if(node)
        node->set_parent(parent == NULL ? 0 : curOf(parent));
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline RBTColor RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::rbColorOf(NodeType_* node)
{
    return isBlack(node) ? RB_BLACK : RB_RED;
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline bool RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::isRed(NodeType_* node)
{
    return (node != NULL && (node->get_color() == RB_RED));
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline bool RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::isBlack(NodeType_* node)
{
    return (node == NULL || (node->get_color() == RB_BLACK));
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::setRbColor(NodeType_* node, RBTColor color)
{
    if(node != NULL) color == RB_BLACK ? setBlack(node) : setRed(node);
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::setRed(NodeType_* node)
{
    if(node)
    {
        //如果节点颜色变化，则说明树发生了调整，直接清除身上的最大节点和最小节点标记
        node->set_color(RB_RED);
    }
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::setBlack(NodeType_* node)
{
    if(node)
    {
        //如果节点颜色变化，则说明树发生了调整，直接清除身上的最大节点和最小节点标记
        node->set_color(RB_BLACK);
    }
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline NodeType_* RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::leftOf(NodeType_* node)
{
    if(node == NULL) return NULL;
    return getNode(node->get_left());
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::setLeft(NodeType_ *node, NodeType_ *left)
{
    if(node)
    {
        node->set_left(left == NULL ? 0 : (curOf(left)));
    }
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline NodeType_* RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::rightOf(NodeType_* node)
{
    if(node == NULL) return  NULL;
    return getNode(node->get_right());
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::setRight(NodeType_ *node, NodeType_ *right)
{
    if(node)
    {
        node->set_right(right == NULL ? 0 : curOf(right));
    }
}

template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
inline INDEX_TYPE RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::curOf(NodeType_ *node)
{
    return node == NULL ? 0 : ARRAY_OFFSET(m_Pool,node);
}

#endif //RBTARR_MAP_RB_TREE_H
