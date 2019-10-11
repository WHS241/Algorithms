#ifndef TREE_ITERATOR_IMPL_H
#define TREE_ITERATOR_IMPL_H

#include "binary_tree_base.h"
#include <list>

namespace tree {
    template<typename T>
    class tree_iterator;

// The implementation for the Binary Tree iterator
    template<typename T>
    class tree_iterator_impl {
    public:
        tree_iterator_impl() = delete;
        explicit tree_iterator_impl(typename binary_tree<T>::node *current);
        
        virtual bool operator==(const tree_iterator_impl<T> &) const noexcept;
        virtual bool operator!=(const tree_iterator_impl<T> &) const noexcept;
        virtual const T &operator*() const noexcept;
        virtual T &operator*() noexcept;
        virtual const T *operator->() const noexcept;
        virtual T *operator->() noexcept;

        virtual tree_iterator_impl &operator++() noexcept = 0;
        virtual tree_iterator_impl &operator--() noexcept = 0;

    protected:
        typename binary_tree<T>::node *_current;

        friend class tree_iterator<T>;
    };

    template<typename T>
    class pre_order_iterator_impl : public tree_iterator_impl<T> {
    public:
        pre_order_iterator_impl(typename binary_tree<T>::node *current);
        pre_order_iterator_impl &operator++() noexcept;
        pre_order_iterator_impl &operator--() noexcept;

    private:
        static typename binary_tree<T>::node *s_pre_order_end(
                typename binary_tree<T>::node *root) noexcept;
    };

    template<typename T>
    class in_order_iterator_impl : public tree_iterator_impl<T> {
    public:
        in_order_iterator_impl(typename binary_tree<T>::node *current);
        in_order_iterator_impl &operator++() noexcept;
        in_order_iterator_impl &operator--() noexcept;

    private:
        static typename binary_tree<T>::node *s_leftmost_descendant(
                typename binary_tree<T>::node *root) noexcept;
        static typename binary_tree<T>::node *s_rightmost_descendant(
                typename binary_tree<T>::node *root) noexcept;
    };

    template<typename T>
    class post_order_iterator_impl : public tree_iterator_impl<T> {
    public:
        post_order_iterator_impl(typename binary_tree<T>::node *current);
        post_order_iterator_impl &operator++() noexcept;
        post_order_iterator_impl &operator--() noexcept;

    private:
        static typename binary_tree<T>::node *s_post_order_start(
                typename binary_tree<T>::node *root) noexcept;
    };

    template<typename T>
    class level_order_iterator_impl : public tree_iterator_impl<T> {
    public:
        level_order_iterator_impl(typename binary_tree<T>::node *current);
        level_order_iterator_impl &operator++() noexcept;
        level_order_iterator_impl &operator--() noexcept;

    private:
        std::list<typename binary_tree<T>::node *> _buffer;
        typename std::list<typename binary_tree<T>::node *>::iterator t_pos;

        friend class tree_iterator<T>;
    };
}

#include "src/structures/binary_tree_iterator_impl.cpp"

#endif // TREE_ITERATOR_IMPL_H