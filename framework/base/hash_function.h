//
// Created by dguco on 2022/4/14.
//

#ifndef RBTARR_MAP_HASH_FUNCTION_H
#define RBTARR_MAP_HASH_FUNCTION_H
namespace rbt_hash
{
namespace hash_function
{
template<class _Key>
struct hash
{
};

template<>
struct hash<short>
{
    size_t operator()(short value) const
    {
        return value;
    }
};

template<>
struct hash<unsigned short>
{
    size_t operator()(unsigned short value) const
    {
        return value;
    }
};

template<>
struct hash<int>
{
    size_t operator()(int value) const
    {
        return value;
    }
};

template<>
struct hash<unsigned int>
{
    size_t operator()(unsigned int value) const
    {
        return value;
    }
};

template<>
struct hash<long>
{
    size_t operator()(long value) const
    {
        return value;
    }
};

template<>
struct hash<unsigned long>
{
    size_t operator()(unsigned long value) const
    {
        return value;
    }
};

template<>
struct hash<unsigned long long>
{
    size_t operator()(unsigned long long value) const
    {
        return value;
    }
};
}

}
#endif //RBTARR_MAP_HASH_FUNCTION_H
