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
        virtual void NextWithLevelEdges() = 0;

        virtual const T Dereferencing() const = 0;
        virtual const T* Arrow() const = 0;

        virtual bool IsEqual(std::shared_ptr<IBSTItImpl> other) const = 0;

        virtual std::shared_ptr<IBSTItImpl> FindRoot() const = 0;

        virtual std::pair<std::shared_ptr<IBST<T>>, std::shared_ptr<IBST<T>>> Split() const = 0;

        virtual void SetHasLevelEdges(bool has_level_edges) const = 0;
        virtual void SetIsVertex(bool is_vertex) const = 0;
    };

    virtual std::shared_ptr<IBSTItImpl> Begin(bool with_level_edges) const = 0;
    virtual std::shared_ptr<IBSTItImpl> End() const = 0;

    virtual void Merge(std::shared_ptr<IBST<T>> other) = 0;

    virtual bool IsEmpty() const = 0;
    virtual size_t Size() const = 0;

public:
    virtual ~IBST() = default;

    class iterator {
    public:
        iterator() = default;
        iterator(const iterator& other) : pimpl_(other.pimpl_->Clone()) {
        }
        explicit iterator(std::shared_ptr<IBSTItImpl> pimpl) : pimpl_(pimpl) {
        }

        ~iterator() = default;

        iterator& operator=(const iterator& other) {
            pimpl_ = other.pimpl_->Clone();
            return *this;
        }

        iterator& operator++() {
            pimpl_->Increment();
            return *this;
        }
        iterator operator++(int) {
            iterator cpy(pimpl_->Clone());
            pimpl_->Increment();
            return cpy;
        }
        iterator& next_with_level_edges() {
            pimpl_->NextWithLevelEdges();
            return *this;
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

        const std::shared_ptr<IBSTItImpl> get() const {
            return pimpl_;
        }

        iterator get_root() const {
            return iterator(pimpl_->FindRoot());
        }

        std::pair<std::shared_ptr<IBST<T>>, std::shared_ptr<IBST<T>>> split() const {
            return pimpl_->Split();
        }

        void set_has_level_edges(bool has_level_edges) const {
            pimpl_->SetHasLevelEdges(has_level_edges);
        }

        void set_is_vertex(bool is_vertex) const {
            pimpl_->SetIsVertex(is_vertex);
        }

    private:
        std::shared_ptr<IBSTItImpl> pimpl_;
    };

    iterator begin(bool with_level_edges = false) const {
        return iterator(Begin(with_level_edges));
    }
    iterator end() const {
        return iterator(End());
    }

    void merge(std::shared_ptr<IBST> other) {
        Merge(other);
    }

    bool empty() const {
        return IsEmpty();
    }

    size_t size() const {
        return Size();
    }
};
