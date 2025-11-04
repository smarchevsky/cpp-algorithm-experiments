#ifndef DENSE_TREE_H
#define DENSE_TREE_H

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

/* Dense tree
 *
 * 1. Each node data can be different size.
 * 2. Nodes contain not pointers but offsets RELATIVE to the BUFFER BEGIN, offsets can be 1 byte in size.
 * 3. Nodes and data are stored in the same contiguous array.
 * 4. Data is stored after node: Node1, Data1, Node2, LargeData2, Node3, Data3 ...
 * 5. As pointers are relative the tree is copyable without braking the hierarchy.
 *
 * Reallocation, rearrange tree is not supported yet.
 */

template <int alignSize>
inline constexpr size_t alignToSize(size_t p)
{
    static_assert(((alignSize - 1) & alignSize) == 0); // is pow of 2
    constexpr size_t alignMask = (alignSize - 1);
    return (p + alignMask) & ~alignMask;
}

// pre-allocated buffer
template <typename RelPtrType>
class DenseTreeBuf {
    static constexpr int DefaultBufSize = 256;
    static_assert(DefaultBufSize - 1 <= std::numeric_limits<RelPtrType>::max());

public:
    size_t size {};
    uint8_t data[DefaultBufSize] {};

    // return offset aligned to T, changes size to align TAlign
    template <typename T, size_t CustomAlignment = 1>
    RelPtrType allocate(size_t num)
    {
        constexpr size_t maxAlignment = std::max(alignof(T), CustomAlignment);
        const size_t alignedPtrStart = alignToSize<maxAlignment>((size_t)data + size);
        const size_t alignedOffsetStart = alignedPtrStart - (size_t)data;
        size = alignedOffsetStart + num * sizeof(T);

        assert(size < sizeof(data));
        return (RelPtrType)alignedOffsetStart;
    }
};

constexpr int CustomStringAlignment = 4;

// node with relative pointers (aka uint8_t, uint16_t)
template <typename RelPtrType>
struct DenseTreeNode {
    RelPtrType l, r;

    // Data is located after node
    // if you allocate node with:  buf.allocate<char, CustomStringAlignment>(len);
    //                      Read:    nodePtr->getData<CustomStringAlignment>();
    // Read and write alignment must be consistent.

    template <typename T, size_t CustomAlignment = 1>
    T* getData()
    {
        constexpr size_t maxAlignment = std::max(alignof(T), CustomAlignment);
        const size_t unaligned = (size_t)this + sizeof(DenseTreeNode);
        return (T*)alignToSize<maxAlignment>(unaligned);
    }
};

// Tree from list of variable size string array
template <typename RelPtrType>
RelPtrType makeRandomTree(DenseTreeBuf<RelPtrType>& buf, int level, char** strings, int stringNum)
{
    using Node_t = DenseTreeNode<RelPtrType>;
    if (level == 0)
        return (RelPtrType)-1;

    RelPtrType n = buf.template allocate<Node_t>(1);

    auto str = strings[rand() % stringNum];
    auto arenaStr = buf.data + buf.template allocate<char, CustomStringAlignment>(strlen(str) + 1);
    strcpy((char*)arenaStr, str);

    //  printf("%s \n", arenaStr);
    Node_t* nodePtr = (Node_t*)(buf.data + n);
    nodePtr->l = makeRandomTree<RelPtrType>(buf, level - 1, strings, stringNum);
    nodePtr->r = makeRandomTree<RelPtrType>(buf, level - 1, strings, stringNum);
    return n;
}

// Draw tree structure
template <typename RelPtrType>
void printTree(DenseTreeBuf<RelPtrType>& buf, RelPtrType n, int level)
{
    using Node_t = DenseTreeNode<RelPtrType>;

    if (n == (RelPtrType)-1)
        return;

    for (int i = 0; i < level; ++i) {
        printf("  ");
    }

    Node_t* nodePtr = (Node_t*)(buf.data + n);
    printf("%s\n", nodePtr->template getData<char, CustomStringAlignment>());

    printTree(buf, nodePtr->l, level + 1);
    printTree(buf, nodePtr->r, level + 1);
}

#endif // DENSE_TREE_H
