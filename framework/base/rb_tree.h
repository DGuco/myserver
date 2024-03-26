//
// * C++ ����: �����
// Created by dguco on 2022/4/13.
// https://www.cnblogs.com/skywang12345/p/3245399.html
// https://www.cnblogs.com/skywang12345/p/3603935.html
//
#ifndef RBTARR_MAP_RB_TREE_H
#define RBTARR_MAP_RB_TREE_H
#include <iomanip>
#include <iostream>
#include <list>

#define RB_TREE_JAVA 1
#define DEBUG_RB_TREE 0
#define NodeType_ RBTNode<KeyType_,INDEX_TYPE>
#define ARRAY_OFFSET(array,node) (node - array + 1)   //����ȡ��ʵ������+1��Ϊ��������[1,Cap_]

using namespace std;
namespace my_std
{
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
     *�����ڵ���
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
        KeyType_                            key_;                          //�ڵ������Ϣ
        INDEX_TYPE                          parent_;                       //���׽ڵ��������е�����λ��+1
        INDEX_TYPE                          left_;                         //���ӽڵ��������е�����λ��+1
        INDEX_TYPE                          right_;                        //���ӽڵ��������е�����λ��+1
        INDEX_TYPE                          data_;                         //data�������е�����λ��+1
        unsigned  char                      color_;                        //�ڵ�color
    };

    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE = unsigned int,std::size_t Cap_ = 0>
    class RBTree
    {
    public:
        RBTree(NodeType_ *pool,INDEX_TYPE root = 0);
        ~RBTree();
        bool isRBTree();
        // ǰ�����"�����"
        void preOrder(std::list<NodeType_*>& resList);
        // �������"�����"
        void inOrder(std::list<NodeType_*>& resList);
        // �������"�����"
        void postOrder(std::list<NodeType_*>& resList);
        // (�ݹ�ʵ��)����"�����"�м�ֵΪkey�Ľڵ�
        NodeType_ *search(KeyType_ key);
        // (�ǵݹ�ʵ��)����"�����"�м�ֵΪkey�Ľڵ�
        NodeType_ *iterativeSearch(KeyType_ key);
        // ������С��㣺������С���ļ�ֵ��
        NodeType_ * minimum();
        // ��������㣺���������ļ�ֵ��
        NodeType_ * maximum();
        // �ҽ��(x)�ĺ�̽�㡣��������"�����������ֵ���ڸý��"��"��С���"��
        NodeType_ *successor(NodeType_ *x);
        // �ҽ��(x)��ǰ����㡣��������"�����������ֵС�ڸý��"��"�����"��
        NodeType_ *predecessor(NodeType_ *x);
        // ���뺯��
        void insert(NodeType_ *node);
        // ɾ�����(keyΪ�ڵ��ֵ)
        NodeType_* remove(KeyType_ key);
        // ɾ������
        NodeType_* remove(NodeType_ *node);
        // ��ӡ�����
        void print();
        bool isEmpty();
        INDEX_TYPE root();
    private:
        bool isRBTree(NodeType_* root, int blacknum, int count);
        // ǰ�����"�����"
        void preOrder(NodeType_ *tree, std::list<NodeType_*>& resList) ;
        // �������"�����"
        void inOrder(NodeType_ *tree, std::list<NodeType_*>& resList) ;
        // �������"�����"
        void postOrder(NodeType_ *tree, std::list<NodeType_*>& resList) ;
        // (�ݹ�ʵ��)����"�����x"�м�ֵΪkey�Ľڵ�
        NodeType_ *search(NodeType_ *x, KeyType_ key);
        // (�ǵݹ�ʵ��)����"�����x"�м�ֵΪkey�Ľڵ�
        NodeType_ *iterativeSearch(NodeType_ *x, KeyType_ key);
        // ������С��㣺����treeΪ�����ĺ��������С��㡣
        NodeType_ *minimum(NodeType_ *tree);
        // ��������㣺����treeΪ�����ĺ����������㡣
        NodeType_ *maximum(NodeType_ *tree);
        // ����
        void leftRotate(NodeType_ *x);
        // ����
        void rightRotate(NodeType_ *y);
        // ������������
        void insertFixUp(NodeType_ *node);
        // ɾ����������
        void removeFixUp(NodeType_ *node);
        void removeFixUp(NodeType_ *node, NodeType_ *parent);
        // ��ӡ�����
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
     * ���캯��
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::RBTree(NodeType_  *pool,INDEX_TYPE root)
        :m_Pool(pool),m_Root(root)
    {
    }

    /*
     * ��������
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::~RBTree()
    {
    }

    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    bool RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::isRBTree()
    {
        //����
        if (getNode(m_Root) == nullptr)
        {
            return true;
        }
        //���ڵ�Ϊ��ɫ
        if (isRed(getNode(m_Root)))
        {
            cout << "���ڵ�Ϊ��ɫ" << endl;
            return false;
        }
        //��ɫ���������·������ͬ
        //����һ���õ���׼ֵ
        int Blacknum = 0;
        NodeType_* cur = getNode(m_Root);
        while (cur)
        {
            if (isBlack(cur))
                Blacknum++;
            cur = leftOf(cur);
        }
        //�������
        int i = 0;
        return isRBTree(getNode(m_Root), Blacknum, i);
    }

    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    bool RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::isRBTree(NodeType_* root, int blacknum, int count)
    {
        //�ݹ鵽�սڵ�
        if (root == nullptr)
        {
            if (blacknum == count)
                return true;
            cout << "��·���Ϻ�ɫ�ڵ������ͬ" << endl;
            return false;
        }
        //�ӽڵ�Ϊ�����鸸�ڵ��Ƿ�Ϊ�죨ͨ�����ڵ����ӽڵ�������սڵ㣩
        if (isRed(root) && isRed(parentOf(root)))
        {
            cout << "����������ɫ�ڵ�" << endl;
            return false;
        }
        //�����ڽ��
        if (isBlack(root))
            count++;
        //�ݹ���������
        return isRBTree(leftOf(root), blacknum, count) && isRBTree(rightOf(root), blacknum, count);
    }

    /*
     * ǰ�����"�����"
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
     * �������"�����"
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
     * �������"�����"
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
     * (�ݹ�ʵ��)����"�����x"�м�ֵΪkey�Ľڵ�
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
     * (�ǵݹ�ʵ��)����"�����x"�м�ֵΪkey�Ľڵ�
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
     * ������С��㣺����treeΪ�����ĺ��������С��㡣
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
     * ��������㣺����treeΪ�����ĺ����������㡣
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
     * �ҽ��(x)�ĺ�̽�㡣��������"�����������ֵ���ڸý��"��"��С���"��
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    NodeType_ *RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::successor(NodeType_ *x)
    {
        if(x == NULL)
        {
            return NULL;
        }

        // ���x�����Һ��ӣ���"x�ĺ�̽��"Ϊ "�����Һ���Ϊ������������С���"��
        if (rightOf(x) != NULL)
            return minimum(rightOf(x));

        // ���xû���Һ��ӡ���x���������ֿ��ܣ�
        // (01) x��"һ������"����"x�ĺ�̽��"Ϊ "���ĸ����"��
        // (02) x��"һ���Һ���"�������"x����͵ĸ���㣬���Ҹø����Ҫ��������"���ҵ������"��͵ĸ����"����"x�ĺ�̽��"��
        NodeType_ *y = parentOf(x);
        while ((y != NULL) && (x == rightOf(y)))
        {
            x = y;
            y = parentOf(y);
        }
        return y;
    }

    /*
     * �ҽ��(x)��ǰ����㡣��������"�����������ֵС�ڸý��"��"�����"��
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    NodeType_ *RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::predecessor(NodeType_ *x)
    {
        // ���x�������ӣ���"x��ǰ�����"Ϊ "��������Ϊ���������������"��
        if (x->left != NULL)
            return maximum(x->left);

        // ���xû�����ӡ���x���������ֿ��ܣ�
        // (01) x��"һ���Һ���"����"x��ǰ�����"Ϊ "���ĸ����"��
        // (01) x��"һ������"�������"x����͵ĸ���㣬���Ҹø����Ҫ�����Һ���"���ҵ������"��͵ĸ����"����"x��ǰ�����"��
        NodeType_ *y = x->parent;
        while ((y != NULL) && (x == y->left)) {
            x = y;
            y = y->parent;
        }

        return y;
    }

    /*
     * �Ժ�����Ľڵ�(x)��������ת
     * ����ʾ��ͼ(�Խڵ�x��������)��
     *      px                              px
     *     /                               /
     *    x                               y
     *   /  \      --(����)-->           / \                #
     *  lx   y                          x  ry
     *     /   \                       /  \
     *    ly   ry                     lx  ly
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::leftRotate(NodeType_ *x)
    {
        // ����x���Һ���Ϊy
        NodeType_ *y = rightOf(x);

        // �� ��y�����ӡ� ��Ϊ ��x���Һ��ӡ���
        // ���y�����ӷǿգ��� ��x�� ��Ϊ ��y�����ӵĸ��ס�
        setRight(x, leftOf(y));
        if (leftOf(y) != NULL)
            setParent(leftOf(y),x);

        // �� ��x�ĸ��ס� ��Ϊ ��y�ĸ��ס�
        setParent(y, parentOf(x));

        if (parentOf(x) == NULL) {
            m_Root = curOf(y);            // ��� ��x�ĸ��ס� �ǿսڵ㣬��y��Ϊ���ڵ�
        }
        else {
            if (leftOf(parentOf(x)) == x)
                setLeft(parentOf(x),y); //��� x�������ڵ�����ӣ���y��Ϊ��x�ĸ��ڵ�����ӡ�
            else
                setRight(parentOf(x),y); // ��� x�������ڵ���Һ��ӣ���y��Ϊ��x�ĸ��ڵ���Һ��ӡ�
        }

        // �� ��x�� ��Ϊ ��y�����ӡ�
        setLeft(y,x);
        // �� ��x�ĸ��ڵ㡱 ��Ϊ ��y��
        setParent(x,y);
    }

    /*
     * �Ժ�����Ľڵ�(y)��������ת
     * ����ʾ��ͼ(�Խڵ�y��������)��
     *            py                               py
     *           /                                /
     *          y                                x
     *         /  \      --(����)-->            /  \                     #
     *        x   ry                           lx   y
     *       / \                                   / \                   #
     *      lx  rx                                rx  ry
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::rightRotate(NodeType_ *y)
    {
        // ����x�ǵ�ǰ�ڵ�����ӡ�
        NodeType_ *x = leftOf(y);

        // �� ��x���Һ��ӡ� ��Ϊ ��y�����ӡ���
        // ���"x���Һ���"��Ϊ�յĻ����� ��y�� ��Ϊ ��x���Һ��ӵĸ��ס�
        setLeft(y, rightOf(x));
        if (rightOf(x) != NULL)
            setParent(rightOf(x),y);

        // �� ��y�ĸ��ס� ��Ϊ ��x�ĸ��ס�
        setParent(x, parentOf(y));

        if (parentOf(y) == NULL) {
            m_Root = curOf(x);            // ��� ��y�ĸ��ס� �ǿսڵ㣬��x��Ϊ���ڵ�
        }
        else {
            if (y == rightOf(parentOf(y)))
                setRight(parentOf(y),x);// ��� y�������ڵ���Һ��ӣ���x��Ϊ��y�ĸ��ڵ���Һ��ӡ�
            else
                setLeft(parentOf(y),x); // (y�������ڵ������) ��x��Ϊ��x�ĸ��ڵ�����ӡ�
        }

        // �� ��y�� ��Ϊ ��x���Һ��ӡ�
        setRight(x,y);
        // �� ��y�ĸ��ڵ㡱 ��Ϊ ��x��
        setParent(y,x);
    }

    /*
     * �����������������
     *
     * ���������в���ڵ�֮��(ʧȥƽ��)���ٵ��øú�����
     * Ŀ���ǽ������������һ�ź������
     *
     * ����˵����
     *     root ������ĸ�
     *     node ����Ľ��    ��Ӧ���㷨���ۡ��е�z
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::insertFixUp(NodeType_ *node)
    {
        NodeType_ *parent, *gparent;

        // �����ڵ���ڣ����Ҹ��ڵ����ɫ�Ǻ�ɫ,���ڵ��Ǻ�ɫ(������ڵ���һ�����ڷǿ��游�ڵ�)
        parent = parentOf(node);
        while (parent && isRed(parent))
        {
            gparent = parentOf(parent);
            //�����ڵ����游�ڵ������
            if (parent == leftOf(gparent))
            {
                // Case 1������ڵ��Ǻ�ɫ
                NodeType_ *uncle = rightOf(gparent);
                if (isRed(uncle))
                {
                    setBlack(parent);//  (01) �������ڵ㡱��Ϊ��ɫ��
                    setBlack(uncle); //  (02) ��������ڵ㡱��Ϊ��ɫ��
                    setRed(gparent); //  (03) �����游�ڵ㡱��Ϊ����ɫ����
                    node = gparent;      //  (04) �����游�ڵ㡱��Ϊ����ǰ�ڵ㡱(��ɫ�ڵ�)
                }else  //���������,�սڵ���ߺ�ɫ�ķǿսڵ㶼Ϊ��ɫ�ڵ�(�����Ǻ�ɫ)
                {
                    if (rightOf(parent) == node)
                    {
                        node = parent;
                        leftRotate(node);
                    }
                    //�������parent->right == node�������rb_parent(node)�Ƕ�����֮��������в��������յĽ���ͷ���1��һ����
                    setBlack(parentOf(node));
                    setRed(parentOf(parentOf(node)));
                    rightRotate(parentOf(parentOf(node)));
                }
            }
            else//�����ڵ����游�ڵ���Һ���,������Ĳ����С�rightRotate���͡�leftRotate������λ�ã�Ȼ������ִ�С�
            {
                // Case 1����������ڵ��Ǻ�ɫ
                NodeType_ *uncle = leftOf(gparent);
                if (isRed(uncle))
                {
                    setBlack(parent); //  (01) �������ڵ㡱��Ϊ��ɫ��
                    setBlack(uncle);  //  (02) ��������ڵ㡱��Ϊ��ɫ��
                    setRed(gparent);  //  (03) �����游�ڵ㡱��Ϊ����ɫ����
                    node = gparent;       //  (04) �����游�ڵ㡱��Ϊ����ǰ�ڵ㡱(��ɫ�ڵ�)
                }else //���������,�սڵ���ߺ�ɫ�ķǿսڵ㶼Ϊ��ɫ�ڵ�(�����Ǻ�ɫ)
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

        // �����ڵ���Ϊ��ɫ
        setBlack(getNode(m_Root));
    }

    /*
     * �������뵽�������
     *
     * ����˵����
     *     root ������ĸ����
     *     node ����Ľ��        // ��Ӧ���㷨���ۡ��е�node
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::insert(NodeType_ *node)
    {
        NodeType_ *parent_node = NULL;
        NodeType_ *tmp_node = getNode(m_Root);

        //�����������һ�Ŷ�������������ڵ���ӵ�����������С�
        {
            //����root�ڵ�,�����½ڵ�ĸ��׽ڵ�����
            while (tmp_node != NULL)
            {
                parent_node = tmp_node;
                //�½ڵ�ȵ�ǰ�ڵ�keyС������������ʼ��
                if (node->get_key() < tmp_node->get_key())
                    tmp_node = leftOf(tmp_node);
                else //�½ڵ�ȵ�ǰ�ڵ�key������������ʼ��
                    tmp_node = rightOf(tmp_node);
            }

            //���½ڵ㸸�׽ڵ�ָ���ҵ��Ľڵ�
            setParent(node,parent_node);
            //����и��ڵ�
            if (parent_node != NULL)
            {
                //�½ڵ�ȵ�ǰ�ڵ�keyС�����븸�׽ڵ�����
                if (node->get_key() < parent_node->get_key())
                    setLeft(parent_node,node);
                else //�½ڵ�ȵ�ǰ�ڵ�key�󣬲��븸�׽ڵ���ұ�
                    setRight(parent_node,node);
            }
            else
                m_Root = curOf(node);
        }

        //���ýڵ����ɫΪ��ɫ
        setRed(node);
        //������������Ϊһ�Ŷ��������
        insertFixUp(node);
    }

    /*
     * �����ɾ����������
     *
     * �ڴӺ������ɾ������ڵ�֮��(�����ʧȥƽ��)���ٵ��øú�����
     * Ŀ���ǽ������������һ�ź������
     *
     * ����˵����
     *     root ������ĸ�
     *     node �������Ľڵ�
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::removeFixUp(NodeType_ *node)
    {
        NodeType_ *other;

        while (node != getNode(m_Root) && isBlack(node))
        {
            //��node�ǡ������ڵ�����ӡ��������� ��other��Ϊ��x�����塱(��xΪ�����ڵ���Һ���)
            if (leftOf(parentOf(node)) == node)
            {
                other = rightOf(parentOf(node));
                //Case 1: x�ǡ���+�ڡ��ڵ㣬x���ֵܽڵ��Ǻ�ɫ��(��ʱx�ĸ��ڵ��x���ֵܽڵ���ӽڵ㶼�Ǻڽڵ�)��
                if (isRed(other))
                {
                    setBlack(other);              //  (01) ��x���ֵܽڵ���Ϊ����ɫ����
                    setRed(parentOf(node)); //   (02) ��x�ĸ��ڵ���Ϊ����ɫ����
                    leftRotate(parentOf(node));//   (03) ��x�ĸ��ڵ����������
                    other = rightOf(parentOf(node));// (04) ��������������x���ֵܽڵ㡣
                }
                // Case 2: x�ǡ���+�ڡ��ڵ㣬x���ֵܽڵ��Ǻ�ɫ��x���ֵܽڵ���������Ӷ��Ǻ�ɫ��
                if (isBlack(leftOf(other)) && isBlack(rightOf(other)))
                {
                    setRed(other);          //   (01) ��x���ֵܽڵ���Ϊ����ɫ����
                    node = parentOf(node);  //   (02) ���á�x�ĸ��ڵ㡱Ϊ���µ�x�ڵ㡱��
                }
                else {
                    // Case 3: x�ǡ���+�ڡ��ڵ㣬x���ֵܽڵ��Ǻ�ɫ��x���ֵܽڵ�������Ǻ�ɫ���Һ����Ǻ�ɫ�ġ�
                    if (isBlack(rightOf(other)))
                    {
                        setBlack(leftOf(other));              //   (01) ��x�ֵܽڵ��������Ϊ����ɫ����
                        setRed(other);                              //   (02) ��x�ֵܽڵ���Ϊ����ɫ����
                        rightRotate(other);                         //   (03) ��x���ֵܽڵ����������
                        other = rightOf(parentOf(node));      //   (04) ��������������x���ֵܽڵ㡣
                    }
                    // Case 4: x�ǡ���+�ڡ��ڵ㣬x���ֵܽڵ��Ǻ�ɫ��x���ֵܽڵ���Һ����Ǻ�ɫ�ġ�
                    setRbColor(other, rbColorOf(parentOf(node)));// (01) ��x���ڵ���ɫ ��ֵ�� x���ֵܽڵ㡣
                    setBlack(parentOf(node));//   (02) ��x���ڵ���Ϊ����ɫ����
                    setBlack(rightOf(other));//   (03) ��x�ֵܽڵ�����ӽ���Ϊ����ɫ����
                    leftRotate(parentOf(node)); //   (04) ��x�ĸ��ڵ����������
                    node = getNode(m_Root);        //   (05) ���á�x��Ϊ�����ڵ㡱��
                }
            }
            else {
                // �� ��x���ǡ������ڵ���Һ��ӡ���������Ĳ����С�right���͡�left������λ�ã�Ȼ������ִ�С�
                other = leftOf(parentOf(node));
                if (isRed(other))
                {
                    // Case 1: x���ֵ�w�Ǻ�ɫ��
                    setBlack(other);
                    setRed(parentOf(node));
                    rightRotate(parentOf(node));
                    other = leftOf(parentOf(node));
                }
                if (isBlack(leftOf(other)) && isBlack(rightOf(other)))
                {
                    // Case 2: x���ֵ�w�Ǻ�ɫ����w����������Ҳ���Ǻ�ɫ��
                    setRed(other);
                    node = parentOf(node);
                }
                else {
                    if (isBlack(leftOf(other)))
                    {
                        // Case 3: x���ֵ�w�Ǻ�ɫ�ģ�����w�������Ǻ�ɫ���Һ���Ϊ��ɫ��
                        setBlack(rightOf(other));
                        setRed(other);
                        leftRotate(other);
                        other = leftOf(parentOf(node));
                    }
                    // Case 4: x���ֵ�w�Ǻ�ɫ�ģ�����w���Һ����Ǻ�ɫ�ģ�����������ɫ��
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
     * �����ɾ����������
     *
     * �ڴӺ������ɾ������ڵ�֮��(�����ʧȥƽ��)���ٵ��øú�����
     * Ŀ���ǽ������������һ�ź������
     *
     * ����˵����
     *     root ������ĸ�
     *     node �������Ľڵ�
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
                    // Case 1: x���ֵ�w�Ǻ�ɫ��
                    setBlack(other);
                    setRed(parent);
                    leftRotate(parent);
                    other = rightOf(parent);
                }
                if (isBlack(leftOf(other)) && isBlack(rightOf(other)))
                {
                    // Case 2: x���ֵ�w�Ǻ�ɫ����w����������Ҳ���Ǻ�ɫ��
                    setRed(other);
                    node = parent;
                    parent = parentOf(node);
                }
                else
                {
                    if (isBlack(rightOf(other)))
                    {
                        // Case 3: x���ֵ�w�Ǻ�ɫ�ģ�����w�������Ǻ�ɫ���Һ���Ϊ��ɫ��
                        setBlack(leftOf(other));
                        setRed(other);
                        rightRotate(other);
                        other = rightOf(parent);
                    }
                    // Case 4: x���ֵ�w�Ǻ�ɫ�ģ�����w���Һ����Ǻ�ɫ�ģ�����������ɫ��
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
                    // Case 1: x���ֵ�w�Ǻ�ɫ��
                    setBlack(other);
                    setRed(parent);
                    rightRotate(parent);
                    other = leftOf(parent);
                }
                if (isBlack(leftOf(other)) && isBlack(rightOf(other)))
                {
                    // Case 2: x���ֵ�w�Ǻ�ɫ����w����������Ҳ���Ǻ�ɫ��
                    setRed(other);
                    node = parent;
                    parent = parentOf(node);
                }
                else
                {
                    if (isBlack(leftOf(other)))
                    {
                        // Case 3: x���ֵ�w�Ǻ�ɫ�ģ�����w�������Ǻ�ɫ���Һ���Ϊ��ɫ��
                        setBlack(rightOf(other));
                        setRed(other);
                        leftRotate(other);
                        other = leftOf(parent);
                    }
                    // Case 4: x���ֵ�w�Ǻ�ɫ�ģ�����w���Һ����Ǻ�ɫ�ģ�����������ɫ��
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
     * ɾ�����(node)�������ر�ɾ���Ľ��
     *
     * ����˵����
     *     root ������ĸ����
     *     node ɾ���Ľ��
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    NodeType_* RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::remove(NodeType_ *node)
    {
        /**
         * ����ɾ���߼�����Ҫ������java��ʵ���У�������Ҫɾ��a�ڵ㣬ʵ�����п���ɾ���Ĳ���a�ڵ㣬����b�ڵ㣬ֻ������ɾ��֮ǰ���b�ڵ�����ݸ��Ƹ�a������a����ɫ��
         * ���ǵڶ���ʵ��ɾ��a�Ĺ�����ͨ���������ӽڵ��ϵ��ʵ�ֲ����нڵ�����ݸ��ƹ��̣�ע������ܹؼ������ѡ��java��ʵ��ɾ����ʱ�򲻹�Ӱ����������Ľڵ��ϵ��
         * ������ҽڵ��ڲ���Ӧ���������ù�ϵ(�ϲ�hashmap�нڵ�a�б�����������ñ�����aɾ���������ڵ�a��Ӧ�����������п��ܲ�����a����b)
         */
        if(RB_TREE_JAVA)
        {
            // ��ɾ���ڵ��"���Һ��Ӷ���Ϊ��"�����
            if ((leftOf(node) != NULL) && (rightOf(node) != NULL))
            {
                //Ѱ��ɾ���ڵ�ĺ�̽ڵ�,������"�����������ֵ���ڸý��"��"��С���"��
                NodeType_* succNode = successor(node);
                /**
                 * ͨ���Ѻ�̽ڵ�����ݸ��Ƹ�Ҫɾ���Ľڵ�,�ú�̽ڵ㴦�ڱ�ɾ��node��λ�ã��ұ���node��color(���ݺ�̽ڵ�����Կ���֪�����Ѻ��
                 * �ڵ�����ݸ��Ƶ�ɾ���ڵ��λ���ϲ����ƻ����������������������)�������п����ƻ��������ƽ������,����ֻҪɾ����̽ڵ�Ȼ�����
                 * �������ƽ��Ϳ�����
                 */
                KeyType_    key = node->get_key();
                INDEX_TYPE  data = node->get_data();
                node->set_key(succNode->get_key());
                node->set_data(succNode->get_data());
                succNode->set_key(key);
                succNode->set_data(data);
                //��Ҫɾ���Ľڵ㻻λ��̽ڵ�,׼��ɾ����̽ڵ�
                node = succNode;
            }

            //���ﲻ�������Ҷ���Ϊ�գ���Ϊ������Ҷ���Ϊ��ɾ���Ľڵ���ǰ���תΪ�ڵ�ĺ�̽ڵ㣨�ýڵ����ڵ�϶�Ϊ�գ�
            NodeType_* replacement = (leftOf(node) != NULL) ? leftOf(node) : rightOf(node);
            //���ɾ���Ľڵ����ӽڵ�
            if(replacement != NULL)
            {
                //����ɾ����ĸ��ӽڵ��ϵ
                setParent(replacement, parentOf(node));
                if (parentOf(node) == NULL)
                    m_Root = curOf(replacement); //���Ҫɾ���Ľڵ�û�и��ڵ㣬�������ӽڵ��Ϊ�µĸ��ڵ�
                else if (node == leftOf(parentOf(node)))
                    setLeft(parentOf(node), replacement); //�Ǹ��ڵ��������
                else
                    setRight(parentOf(node), replacement);

                node->dis_from_list();
                // Fix replacement
                if (isBlack(node))
                    removeFixUp(replacement);

            }else if(parentOf(node) == NULL)//���Ҫɾ���Ľڵ�û�и��ڵ㣬��ɾ���Ľڵ��Ǹ��ڵ�,ֱ�ӰѸ��ڵ��������
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
            // ��ɾ���ڵ��"���Һ��Ӷ���Ϊ��"�������
            if ((leftOf(node) != NULL) && (rightOf(node) != NULL))
            {
                // ��ɾ�ڵ�ĺ�̽ڵ㡣(��Ϊ"ȡ���ڵ�")
                // ������ȡ��"��ɾ�ڵ�"��λ�ã�Ȼ���ٽ�"��ɾ�ڵ�"ȥ����
                NodeType_ *replace = node;
                // ��ȡ��̽ڵ�
                replace = rightOf(replace);
                while (leftOf(replace) != NULL)
                    replace = leftOf(replace);

                // "node�ڵ�"���Ǹ��ڵ�(ֻ�и��ڵ㲻���ڸ��ڵ�)
                if (parentOf(node))
                {
                    if (leftOf(parentOf(node)) == node)
                        setLeft(parentOf(node),replace);
                    else
                        setRight(parentOf(node),replace);
                }
                else
                    // "node�ڵ�"�Ǹ��ڵ㣬���¸��ڵ㡣
                    m_Root = curOf(replace);

                // child��"ȡ���ڵ�"���Һ��ӣ�Ҳ����Ҫ"�����Ľڵ�"��
                // "ȡ���ڵ�"�϶����������ӣ���Ϊ����һ����̽ڵ㡣
                child = rightOf(replace);
                parent = parentOf(replace);
                // ����"ȡ���ڵ�"����ɫ
                color = rbColorOf(replace);

                // "��ɾ���ڵ�"��"���ĺ�̽ڵ�ĸ��ڵ�"
                if (parent == node)
                {
                    parent = replace;
                }
                else
                {
                    // child��Ϊ��
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
            // ����"ȡ���ڵ�"����ɫ
            color = rbColorOf(node);

            if (child)
                setParent(child,parent);

            // "node�ڵ�"���Ǹ��ڵ�
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
     * ɾ��������м�ֵΪkey�Ľڵ�
     *
     * ����˵����
     *     tree ������ĸ����
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    NodeType_* RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::remove(KeyType_ key)
    {
        NodeType_ *node = NULL;
        // ����key��Ӧ�Ľڵ�(node)���ҵ��Ļ���ɾ���ýڵ�
        if ((node = search(getNode(m_Root), key)) != NULL)
           return remove(node);
        return  NULL;
    }

    /*
     * ���ٺ����
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
     * ��ӡ"���������"
     *
     * key        -- �ڵ�ļ�ֵ
     * direction  --  0����ʾ�ýڵ��Ǹ��ڵ�;
     *               -1����ʾ�ýڵ������ĸ���������;
     *                1����ʾ�ýڵ������ĸ������Һ��ӡ�
     */
    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::print(NodeType_ *tree, KeyType_ key, int direction)
    {
        if (tree != NULL) {
            if (direction == 0)    // tree�Ǹ��ڵ�
                cout << setw(2) << tree->get_key() << "(B) is root" << endl;
            else                // tree�Ƿ�֧�ڵ�
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
            //����ڵ���ɫ�仯����˵���������˵�����ֱ��������ϵ����ڵ����С�ڵ���
            node->set_color(RB_RED);
        }
    }

    template<typename KeyType_,typename ValueType_,typename INDEX_TYPE,std::size_t Cap_>
    inline void RBTree<KeyType_,ValueType_,INDEX_TYPE,Cap_>::setBlack(NodeType_* node)
    {
        if(node)
        {
            //����ڵ���ɫ�仯����˵���������˵�����ֱ��������ϵ����ڵ����С�ڵ���
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
}
#endif //RBTARR_MAP_RB_TREE_H
