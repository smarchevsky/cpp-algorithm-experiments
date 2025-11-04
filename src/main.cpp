/*
 * by Stanilav Marchevsky
 *
 * Here I create different algorithms with C/C++.
 * For practice and curiocity.
 *
 */

#include "3party/fruits.h"
#include "graph/dense_tree.h"

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

int main()
{
    DenseTreeBuf buf;

    using Node_t = DenseTreeNode<char, uint8_t>;
    auto root = makeRandomTree<Node_t, uint8_t>(buf, 4, (char**)fruits, ARR_SIZE(fruits));
    printTree<Node_t>(buf, root, 0, 0);

#if 1
    FILE* f = fopen("tree.bin", "wb");
    fwrite(buf.data, 1, buf.size, f);
    fclose(f);
#endif

    printf("Tree size: %zu\n", buf.size);
}

// uncategorized drafts

/*class Arena {
public:
    void* ptr {};
    uint8_t data[2048 * 4] {};

    Arena() { ptr = data; }

    template <class T>
    T* allocate(size_t num)
    {
        constexpr size_t mask = (alignof(T) - 1);
        const size_t aligned = ((size_t)ptr + mask) & (~mask);
        ptr = (void*)(aligned + num * sizeof(T));
        assert(ptr < data + sizeof(data));
        return (T*)aligned;
    }
} arena;*/
