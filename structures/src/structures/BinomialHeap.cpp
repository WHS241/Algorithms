#ifndef BINOMIAL_HEAP_CPP
#define BINOMIAL_HEAP_CPP

#include <cmath>
#include <stdexcept>

template <typename T, typename Compare>
BinomialHeap<T, Compare>::BinomialHeap(Compare comp)
    : NodeHeap<T, Compare>(comp)
    , trees()
    , min(nullptr)
{
}

template <typename T, typename Compare> BinomialHeap<T, Compare>::~BinomialHeap() noexcept
{
    for (Node* root : trees)
        delete root;
}

template <typename T, typename Compare>
BinomialHeap<T, Compare>::BinomialHeap(const BinomialHeap<T, Compare>& src)
    : NodeHeap<T, Compare>(src)
    , trees(src.trees.size(), nullptr)
    , min(nullptr)
{
    this->_size = src._size;
    try {
        std::transform(src.trees.begin(), src.trees.end(), trees.begin(), [this, &src](Node* root) {
            if (root == nullptr)
                return root;

            std::unique_ptr<Node> clone(root->deepClone());
            if (root == src.min)
                this->min == clone.get();
            return clone.release();
        });
    } catch (...) {
        for (Node* root : trees)
            delete root;
    }
}

template <typename T, typename Compare>
BinomialHeap<T, Compare>& BinomialHeap<T, Compare>::operator=(const BinomialHeap<T, Compare>& rhs)
{
    if (this != &rhs) {
        BinomialHeap<T, Compare> temp(rhs);
        *this = std::move(temp);
    }
    return *this;
}

template <typename T, typename Compare>
BinomialHeap<T, Compare>::BinomialHeap(BinomialHeap<T, Compare>&& src) noexcept
    : BinomialHeap<T, Compare>(src.compare)
{
    *this = std::move(src);
}

template <typename T, typename Compare>
BinomialHeap<T, Compare>& BinomialHeap<T, Compare>::operator=(BinomialHeap<T, Compare>&& rhs) noexcept
{
    if (this != &rhs) {
        for (Node* root : trees) {
            delete root;
        }
        trees.clear();
        min = nullptr;
        this->_size = 0;
        std::swap(trees, rhs.trees);
        std::swap(min, rhs.min);
        std::swap(this->_size, rhs._size);
    }
    return *this;
}

template <typename T, typename Compare>
typename BinomialHeap<T, Compare>::Node* BinomialHeap<T, Compare>::add(const T& item)
{
    BinomialHeap<T, Compare> temp(this->compare);
    std::unique_ptr<Node> addNode(this->makeNode(item));
    temp._size = 1;
    temp.trees.push_back(addNode.get());
    temp.min = addNode.get();
    merge(temp);
    return addNode.release();
}

template <typename T, typename Compare> T BinomialHeap<T, Compare>::getRoot() const
{
    if (this->empty())
        throw std::underflow_error("Empty heap");
    return **min;
}

template <typename T, typename Compare> T BinomialHeap<T, Compare>::removeRoot()
{
    if (this->empty())
        throw std::underflow_error("Empty heap");

    if (this->_size == 1) {
        T oldRoot = **min;
        delete min;
        this->_size = 0;
        trees.clear();
        min = nullptr;
        return oldRoot;
    }

    T oldRoot = **min;

    // break into two heaps and merge
    std::vector<Node*> subtrees(min->children.size());
    for (Node* child : min->children)
        // the number of children determines the size of the tree: size = 2^num children
        // Because of the way binomial heaps are built, none of these will be nullptr
        subtrees[child->children.size()] = child;

    BinomialHeap<T, Compare> temp(this->compare);
    temp.trees = std::move(subtrees);
    auto oldSize = this->_size;
    auto oldMin = min;
    auto it = std::find(trees.begin(), trees.end(), min);
    try {
        // find the minimum values, since merge() will not compare all roots
        temp.min = temp.trees.empty() ? nullptr
                                      : *std::min_element(temp.trees.begin(), temp.trees.end(),
                                                          [this](Node *x, Node *y) { return this->compare(**x, **y); });

        // We can now extract the node, updating min
        *it = nullptr;
        min = *std::min_element(trees.begin(), trees.end(), [this](Node *x, Node *y) {
            // here we actually have to account for nullptr
            return x != nullptr && (y == nullptr || this->compare(**x, **y));
        });

        // We don't actually need to recalculate the sizes; as long as temp._size reflects emptiness
        // and the two values add up to new size
        temp._size = temp.trees.size();
        this->_size -= (temp._size + 1);

        merge(temp);
    } catch(...) {
        min = oldMin;
        *it = min;
        this->_size = oldSize;
        throw;
    }

    // isolate oldMin and delete (merge should have removed oldMin as the parent of the children)
    oldMin->children.clear();
    delete oldMin;
    oldMin = nullptr;
    return oldRoot;
}

template <typename T, typename Compare>
void BinomialHeap<T, Compare>::decrease(
    typename BinomialHeap<T, Compare>::Node* target, const T& newVal)
{
    if (this->compare(**target, newVal))
        throw std::invalid_argument("Increasing key");

    // use same bubble-up strategy as binary heap
    **target = newVal;
    if (this->compare(newVal, **min))
        min = target;

    while (target->parent != nullptr) {
        if (this->compare(**target, **target->parent)) {
            auto toSwap = target->parent;
            target->parent = target;
            auto it = std::find(toSwap->children.begin(), toSwap->children.end(), target);
            *it = toSwap;
            std::swap(target->parent, toSwap->parent);
            std::swap(target->children, toSwap->children);

            for (Node* child : target->children)
                child->parent = target;

            for (Node* child : toSwap->children)
                child->parent = toSwap;

            if (target->parent != nullptr) {
                // still need to account for next level up
                it = std::find(
                    target->parent->children.begin(), target->parent->children.end(), toSwap);
                *it = target;
            } else {
                auto treesIt = std::find(trees.begin(), trees.end(), toSwap);
                *treesIt = target;
            }
        } else {
            break;
        }
    }
}

template <typename T, typename Compare>
void BinomialHeap<T, Compare>::merge(BinomialHeap<T, Compare>& src)
{
    if (this == &src || src.empty())
        return;
    if (this->empty()) {
        *this = std::move(src);
        return;
    }

    uint32_t newSize = this->_size + src._size;
    uint32_t vectorSize = 0;
    for (uint32_t i = newSize; i != 0; i /= 2)
        ++vectorSize;

    trees.resize(vectorSize, nullptr);
    Node* carry = nullptr;

    // can "add" like normal arithmetic
    // Comment notation: 0 = nullptr, 1 = not nullptr
    // abc = carry is a, *it1 is b, *it2 is c
    for (auto it1 = trees.begin(), it2 = src.trees.begin(); it1 != trees.end(); ++it1) {
        // Of the 8 possibilities, 000 and 010 can be no-ops
        if (!(carry == nullptr && (it2 == src.trees.end() || *it2 == nullptr))) {
            if (carry == nullptr) {
                if (*it1 == nullptr) { // 001
                    *it1 = *it2;
                    (*it2)->parent = nullptr;
                } else { // 011
                    if (this->compare(***it1, ***it2)) {
                        std::iter_swap(it1, it2);
                    }

                    (*it2)->children.push_back(*it1);
                    (*it1)->parent = *it2;
                    carry = *it2;
                    *it1 = nullptr;
                }
            } else if (*it1 == nullptr && (it2 == src.trees.end() || *it2 == nullptr)) { // 100
                *it1 = carry;
                carry->parent = nullptr;
                carry = nullptr;
            } else if (*it1 == nullptr || it2 == src.trees.end() || *it2 == nullptr) {
                if (*it1 == nullptr) // 101
                    std::iter_swap(it1, it2);

                // 110
                if (this->compare(***it1, **carry))
                    std::swap(carry, *it1);

                carry->children.push_back(*it1);
                (*it1)->parent = carry;
                *it1 = nullptr;
            } else { // 111
                // we'll keep it1 and merge it2 into carry
                if (this->compare(***it2, **carry))
                    std::swap(*it2, carry);

                carry->children.push_back(*it2);
                (*it2)->parent = carry;
            }
        }

        if (it2 != src.trees.end())
            ++it2;
    }

    min = (min != nullptr && this->compare(**min, **src.min)) ? min : src.min;
    src.min = nullptr;
    src.trees.clear();
    src._size = 0;
    this->_size = newSize;
}
#endif // BINOMIAL_HEAP_CPP
