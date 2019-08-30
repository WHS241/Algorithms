#ifndef FIBONACCI_HEAP_CPP
#define FIBONACCI_HEAP_CPP

#include <algorithm>
#include <iterator>
#include <memory>

template <typename T, typename Compare>
FibonacciHeap<T, Compare>::FibonacciHeap(Compare comp)
    : NodeHeap<T, Compare>(comp)
    , trees()
    , min(nullptr)
{
}

template <typename T, typename Compare> FibonacciHeap<T, Compare>::~FibonacciHeap() noexcept
{
    for (Node* root : trees)
        delete root;
}

template <typename T, typename Compare>
FibonacciHeap<T, Compare>::FibonacciHeap(const FibonacciHeap<T, Compare>& src)
    : NodeHeap<T, Compare>(src)
    , trees()
    , min(nullptr)
{
    try {
        std::transform(src.trees.begin(), src.trees.end(), std::back_inserter(trees),
            [this, &src](const Node* root) {
                std::unique_ptr<Node> newTree(root->deepClone());
                newTree->flag = root->flag;
                if (root == src.min)
                    min = newTree.get();
                return newTree.release();
            });
    } catch (...) {
        for (Node* root : trees)
            delete root;
        throw;
    }
}

template <typename T, typename Compare>
FibonacciHeap<T, Compare>& FibonacciHeap<T, Compare>::operator=(
    const FibonacciHeap<T, Compare>& rhs)
{
    if (this != &rhs) {
        FibonacciHeap<T, Compare> temp(rhs);
        *this = std::move(rhs);
    }
    return *this;
}

template <typename T, typename Compare>
FibonacciHeap<T, Compare>::FibonacciHeap(FibonacciHeap<T, Compare>&& src)
    : FibonacciHeap<T, Compare>(src.comp)
{
    *this = std::move(src);
}

template <typename T, typename Compare>
FibonacciHeap<T, Compare>& FibonacciHeap<T, Compare>::operator=(FibonacciHeap<T, Compare>&& rhs)
{
    if (this != &rhs) {
        for (Node* root : trees)
            delete root;
        min = nullptr;
        this->_size = 0;

        std::swap(trees, rhs.trees);
        std::swap(min, rhs.min);
        std::swap(this->_size, rhs._size);
    }
    return *this;
}

template <typename T, typename Compare>
typename FibonacciHeap<T, Compare>::Node* FibonacciHeap<T, Compare>::add(const T& item)
{
    std::unique_ptr<Node> newNode(this->makeNode(item));
    trees.push_back(newNode.get());
    if (min == nullptr || this->compare(item, **min))
        min = newNode.get();

    ++this->_size;
    return newNode.release();
}

template <typename T, typename Compare> T FibonacciHeap<T, Compare>::getRoot() const
{
    if (this->empty())
        throw std::underflow_error("Empty heap");
    return **min;
}

template <typename T, typename Compare> T FibonacciHeap<T, Compare>::removeRoot()
{
    if (this->empty())
        throw std::underflow_error("Empty heap");

    T oldRoot = **min;
    if (this->_size == 1) {
        delete min;
        trees.clear();
        this->_size = 0;
    } else {

        // number of new trees grows logarithmically (a tree of size n must have height <= log_phi
        // n) for simplicity, we know sqrt(2) < phi < 2
        uint32_t numTrees = 1;
        for (uint32_t i = this->_size - 1; i > 1; i /= 2)
            numTrees += 2;
        std::vector<typename std::list<Node*>::iterator> newRoots(numTrees, trees.end());

        // detach all children
        for (Node* child : min->children)
            child->parent = nullptr;
        trees.splice(trees.end(), min->children);

        // remove min
        auto found = std::find(trees.begin(), trees.end(), min);
        auto removeBound = found;
        ++removeBound;
        trees.erase(found, removeBound);
        delete min;
        min = nullptr;
        --this->_size;


        // consolidate
        for (auto it = trees.begin(); it != trees.end(); ++it) {
            uint32_t treeSize = (*it)->children.size();

            while (newRoots[treeSize] != trees.end()) {
                auto toCompare = newRoots[treeSize];
                if (this->compare(***toCompare, ***it))
                    std::iter_swap(it, toCompare);
                auto nextRemove = toCompare;
                ++nextRemove;
                (*it)->children.splice((*it)->children.end(), trees, toCompare, nextRemove);
                (*toCompare)->parent = *it;
                newRoots[treeSize] = trees.end();
                ++treeSize;
            }

            newRoots[treeSize] = it;
        }

        // unmark any roots and find new min
        for (Node* root : trees) {
            root->flag = false;
            if (min == nullptr || this->compare(**root, **min))
                min = root;
        }
    }
    return oldRoot;
}

template <typename T, typename Compare>
void FibonacciHeap<T, Compare>::decrease(
    typename FibonacciHeap<T, Compare>::Node* target, const T& newVal)
{
    **target = newVal;
    if (this->compare(newVal, **min))
        min = target;

    // cut and mark, repeat until root or unmarked
    if (target->parent != nullptr && this->compare(**target, **target->parent)) {
        do {
            Node* oldParent = target->parent;
            target->parent = nullptr;
            target->flag = false;
            auto toRemove
                = std::find(oldParent->children.begin(), oldParent->children.end(), target);
            auto toRemoveNext = toRemove;
            ++toRemoveNext;
            trees.splice(trees.end(), oldParent->children, toRemove, toRemoveNext);
            target = oldParent;
        } while (target->flag);

        if (target->parent != nullptr)
            target->flag = true;
    }
}

template <typename T, typename Compare>
void FibonacciHeap<T, Compare>::merge(FibonacciHeap<T, Compare>& src)
{
    if (min == nullptr || (src.min != nullptr && this->compare(**src.min, **min)))
        min = src.min;
    trees.splice(trees.end(), src.trees);
    this->_size += src._size;
    src._size = 0;
    src.min = nullptr;
}

#endif // FIBONACCI_HEAP_CPP