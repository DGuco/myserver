//
// Created by dguco on 20-1-9.
//

#ifndef SERVER_SKIP_LIST_H
#define SERVER_SKIP_LIST_H

#include <unordered_map>
#include <cmath>
using namespace std;

#define ZSKIPLIST_MAXLEVEL 64 /* Should be enough for 2^64 elements */
#define ZSKIPLIST_P 0.25      /* Skiplist P = 1/4 */

template <typename SCORE_TYPE,typename MEMBER_TYPE>
struct skiplistNode
{
    SCORE_TYPE score;  //排序key，必须是一个可以比较的类型，必须实现对象的比较操作符
    MEMBER_TYPE* mem;  //分数对应的对象信息
    skiplistNode *backward;
    struct zskiplistLevel
    {
        skiplistNode<SCORE_TYPE,MEMBER_TYPE> *forward;
        unsigned long span;
    } level[];
};

template <typename SCORE_TYPE,typename MEMBER_TYPE>
struct skiplist
{
    skiplistNode<SCORE_TYPE,MEMBER_TYPE> *header, *tail;
    unsigned long length;
    int level;
};

template <typename SCORE_TYPE,typename MEMBER_TYPE>
class CRankList
{
public:
    CRankList()
    {
        int j;
        mskiplist = new skiplist;
        mskiplist->level = 1;
        mskiplist->length = 0;
        mskiplist->header = CreateSkipListNode(ZSKIPLIST_MAXLEVEL,0,NULL);
        for (j = 0; j < ZSKIPLIST_MAXLEVEL; j++) {
            mskiplist->header->level[j].forward = NULL;
            mskiplist->header->level[j].span = 0;
        }
        mskiplist->header->backward = NULL;
        mskiplist->tail = NULL;
    }

    /* Insert a new node in the skiplist. Assumes the element does not already
    * exist (up to the caller to enforce that). The skiplist takes ownership
    * of the passed SDS string 'ele'.
    * */
    skiplistNode *Insert(skiplist *zsl, SCORE_TYPE score, MEMBER_TYPE ele) {
        skiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
        unsigned int rank[ZSKIPLIST_MAXLEVEL];
        int i, level;

        x = zsl->header;
        for (i = zsl->level-1; i >= 0; i--)
        {
            /* store rank that is crossed to reach the insert position */
            rank[i] = i == (zsl->level-1) ? 0 : rank[i+1];
            while (x->level[i].forward &&
                  (x->level[i].forward->score < score || (x->level[i].forward->score == score &&
                      x->level[i].forward->mem < ele)))
            {
                rank[i] += x->level[i].span;
                x = x->level[i].forward;
            }
            update[i] = x;
        }
        /* we assume the element is not already inside, since we allow duplicated
         * scores, reinserting the same element should never happen since the
         * caller of zslInsert() should test in the hash table if the element is
         * already inside or not. */
        level = RandomLevel();
        if (level > zsl->level)
        {
            for (i = zsl->level; i < level; i++) {
                rank[i] = 0;
                update[i] = zsl->header;
                update[i]->level[i].span = zsl->length;
            }
            zsl->level = level;
        }
        x = CreateSkipListNode(level,score,ele);
        for (i = 0; i < level; i++)
        {
            x->level[i].forward = update[i]->level[i].forward;
            update[i]->level[i].forward = x;

            /* update span covered by update[i] as x is inserted here */
            x->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
            update[i]->level[i].span = (rank[0] - rank[i]) + 1;
        }

        /* increment span for untouched levels */
        for (i = level; i < zsl->level; i++)
        {
            update[i]->level[i].span++;
        }

        x->backward = (update[0] == zsl->header) ? NULL : update[0];
        if (x->level[0].forward)
            x->level[0].forward->backward = x;
        else
            zsl->tail = x;
        zsl->length++;
        return x;
    }

    /* Internal function used by zslDelete, zslDeleteByScore and zslDeleteByRank */
    void zslDeleteNode(skiplist *zsl, skiplistNode *x, skiplistNode **update) {
        int i;
        for (i = 0; i < zsl->level; i++) {
            if (update[i]->level[i].forward == x) {
                update[i]->level[i].span += x->level[i].span - 1;
                update[i]->level[i].forward = x->level[i].forward;
            } else {
                update[i]->level[i].span -= 1;
            }
        }
        if (x->level[0].forward) {
            x->level[0].forward->backward = x->backward;
        } else {
            zsl->tail = x->backward;
        }
        while(zsl->level > 1 && zsl->header->level[zsl->level-1].forward == NULL)
            zsl->level--;
        zsl->length--;
    }

    int zslDelete(skiplist *zsl, SCORE_TYPE score, MEMBER_TYPE ele, skiplistNode **node) {
        skiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
        int i;

        x = zsl->header;
        for (i = zsl->level-1; i >= 0; i--) {
            while (x->level[i].forward &&
                  (x->level[i].forward->score < score || (x->level[i].forward->score == score && x->level[i].forward->mem < ele)))
            {
                x = x->level[i].forward;
            }
            update[i] = x;
        }
        /* We may have multiple elements with the same score, what we need
         * is to find the element with both the right score and object. */
        x = x->level[0].forward;
        if (x && score == x->score && x->mem == ele) {
            zslDeleteNode(zsl, x, update);
            if (!node)
                delete x;
            else
                *node = x;
            return 1;
        }
        return 0; /* not found */
    }

    skiplistNode *zslUpdateScore(skiplist *zsl, double curscore, MEMBER_TYPE ele, double newscore) {
        skiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
        int i;

        /* We need to seek to element to update to start: this is useful anyway,
         * we'll have to update or remove it. */
        x = zsl->header;
        for (i = zsl->level-1; i >= 0; i--) {
            while (x->level[i].forward &&
                (x->level[i].forward->score < curscore ||
                    (x->level[i].forward->score == curscore &&
                        x->level[i].forward->mem < ele)))
            {
                x = x->level[i].forward;
            }
            update[i] = x;
        }

        /* Jump to our element: note that this function assumes that the
         * element with the matching score exists. */
        x = x->level[0].forward;
        //serverAssert(x && curscore == x->score && sdscmp(x->ele,ele) == 0);

        /* If the node, after the score update, would be still exactly
         * at the same position, we can just update the score without
         * actually removing and re-inserting the element in the skiplist. */
        if ((x->backward == NULL || x->backward->score < newscore) &&
            (x->level[0].forward == NULL || x->level[0].forward->score > newscore))
        {
            x->score = newscore;
            return x;
        }

        /* No way to reuse the old node: we need to remove and insert a new
         * one at a different place. */
        zslDeleteNode(zsl, x, update);
        skiplistNode *newnode = Insert(zsl,newscore,x->mem);
        /* We reused the old node x->ele SDS string, free the node now
         * since zslInsert created a new one. */
        x->mem = NULL;
        delete(x);
        return newnode;
    }
private:
    skiplistNode* CreateSkipListNode(int level,SCORE_TYPE score,MEMBER_TYPE value)
    {
        skiplistNode *zn = (skiplistNode*) malloc(sizeof(skiplistNode) + level * sizeof(struct zskiplistLevel));
        zn->score = score;
        zn->mem = value;
        return zn;
    }

    //随机跳表的层数
    int RandomLevel(void)
    {
        int level = 1;
        while ((random() & 0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
            level += 1;
        return (level < ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
    }
private:
    unordered_map<MEMBER_TYPE,SCORE_TYPE> mrankMap;
    skiplist<SCORE_TYPE,MEMBER_TYPE>* mskiplist;
};



#endif //SERVER_SKIP_LIST_H
