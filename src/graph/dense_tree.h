#ifndef DENSE_TREE_H
#define DENSE_TREE_H

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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
class DenseTreeBuf {
    static constexpr int DefaultBufSize = 256;

public:
    size_t size {};
    uint8_t data[DefaultBufSize] {};

    // return offset aligned to T, changes size to align TAlign
    template <typename T>
    size_t allocate(size_t num)
    {
        const size_t alignedPtrStart = alignToSize<alignof(T)>((size_t)data + size);
        const size_t alignedOffsetStart = alignedPtrStart - (size_t)data;
        size = alignedOffsetStart + num * sizeof(T);

        assert(size < sizeof(data));
        return alignedOffsetStart;
    }
};

// node with relative pointers (aka uint8_t, uint16_t)
template <typename DataType, typename RelPtrType>
struct alignas(alignof(DataType)) DenseTreeNode {
    RelPtrType l, r;

    // Data is located after node
    // DenseTreeNode is aligned to DataType, so data after will be correctly read
    DataType* getData() { return (DataType*)((size_t)this + sizeof(DenseTreeNode)); }
};

// Tree from list of variable size string array
template <typename Node_t, typename RelPtrType>
RelPtrType makeRandomTree(DenseTreeBuf& buf,
    int level, char** strings, int stringNum)
{
    if (level == 0)
        return (RelPtrType)-1;

    RelPtrType nodeOffset = buf.allocate<Node_t>(1);

    auto str = strings[rand() % stringNum];
    auto arenaStr = buf.data + buf.allocate<char>(strlen(str) + 1);
    strcpy((char*)arenaStr, str);

    //  printf("%s \n", arenaStr);
    Node_t* nodePtr = (Node_t*)(buf.data + nodeOffset);
    nodePtr->l = makeRandomTree<Node_t, RelPtrType>(buf, level - 1, strings, stringNum);
    nodePtr->r = makeRandomTree<Node_t, RelPtrType>(buf, level - 1, strings, stringNum);
    return nodeOffset;
}

// Draw tree structure
template <typename Node_t, typename RelPtrType>
void printTree(DenseTreeBuf& buf,
    RelPtrType nodeOffset, int level, size_t childBitfield)
{
    if (nodeOffset == (RelPtrType)-1)
        return;

    for (int i = level - 1; i >= 0; --i) {
        if (childBitfield >> i & 0b1) {
            if (childBitfield & 1 && i == 0)
                printf("└─ ");
            else
                printf("   ");
        } else {
            if (i == 0)
                printf("├─ ");
            else
                printf("|  ");
        }
    }

    Node_t* nodePtr = (Node_t*)(buf.data + nodeOffset);
    printf("%s\n", nodePtr->getData());

    childBitfield <<= 1;
    printTree<Node_t>(buf, nodePtr->l, level + 1, childBitfield);
    childBitfield |= 1;
    printTree<Node_t>(buf, nodePtr->r, level + 1, childBitfield);
}

#endif // DENSE_TREE_H
