#pragma once

#include <cassert>
#include <memory>

template <class T>
class IBST {
protected:
    IBST() = default;

    class IBSTItImpl {
    public:
        virtual ~IBSTItImpl() = default;

        virtual std::shared_ptr<IBSTItImpl> Clone() const = 0;

        virtual void Increment() = 0;
        virtual void Decrement() = 0;

        virtual const T Dereferencing() const = 0;
        virtual const T* Arrow() const = 0;

        virtual bool IsEqual(std::shared_ptr<IBSTItImpl> other) const = 0;
    };

    virtual std::shared_ptr<IBSTItImpl> Begin() const = 0;
    virtual std::shared_ptr<IBSTItImpl> End() const = 0;

    virtual void Make(std::initializer_list<T> list) = 0;

    virtual std::shared_ptr<IBST> Split(std::shared_ptr<IBSTItImpl> where) = 0;
    virtual void Merge(std::shared_ptr<IBST> other) = 0;

public:
    virtual ~IBST() = default;

    class iterator {
    public:
        iterator() = delete;
        iterator(const iterator& other) : pimpl_(other.pimpl_) {
        }
        ~iterator() = default;

        iterator& operator++() {
            pimpl_->Increment();
            return *this;
        }
        iterator operator++(int) {
            iterator cpy(pimpl_->Clone());
            pimpl_->Increment();
            return cpy;
        }
        iterator& operator--() {
            pimpl_->Decrement();
            return *this;
        }
        iterator operator--(int) {
            iterator cpy(pimpl_->Clone());
            pimpl_->Decrement();
            return cpy;
        }

        const T operator*() const {
            return pimpl_->Dereferencing();
        }
        const T* operator->() const {
            return pimpl_->Arrow();
        }

        bool operator==(const iterator& other) const {
            return pimpl_->IsEqual(other.pimpl_);
        }
        bool operator!=(const iterator& other) const {
            return !pimpl_->IsEqual(other.pimpl_);
        }

    private:
        explicit iterator(std::shared_ptr<IBSTItImpl> pimpl) : pimpl_(pimpl) {
        }
        std::shared_ptr<IBSTItImpl> pimpl_;
    };

    iterator begin() const {
        return iterator(Begin());
    }
    iterator end() const {
        return iterator(End());
    }

    void make(std::initializer_list<T> list) {
        Make(list);
    }

    std::shared_ptr<IBST> split(iterator where) {
        return Split(where.pimpl_);
    }

    void merge(std::shared_ptr<IBST> other) {
        Merge(other);
    }
};
