#pragma once

#include <exception>
#include <initializer_list>
#include <memory>
#include <optional>
#include <random>

template <class T>
class IBST;

template <class T>
class CartesianBST : public IBST<T> {
private:
    typedef typename IBST<T>::IBSTItImpl BaseImpl;

    class Random {
    public:
        static uint32_t Next() {
            static Random rand = Random();
            return rand.dist_(rand.gen_);
        }

    private:
        Random() {
            std::random_device device;
            gen_ = std::mt19937(device());
            dist_ =
                std::uniform_int_distribution<uint32_t>(1, std::numeric_limits<uint32_t>::max());
        }

        std::mt19937 gen_;
        std::uniform_int_distribution<uint32_t> dist_;
    };

public:
    struct Node {
        Node() {
            left_ = nullptr;
            right_ = nullptr;
            parent_ = std::weak_ptr<Node>();
            priority_ = Random::Next();
            value_ = std::nullopt;
        }
        explicit Node(const T& value) : value_(value) {
            left_ = nullptr;
            right_ = nullptr;
            parent_ = std::weak_ptr<Node>();
            priority_ = Random::Next();
        }
        Node(const Node& other) : value_(other.value_) {
            left_ = other.left_;
            right_ = other.right_;
            parent_ = other.parent_;
            priority_ = other.priority_;
        }

        std::shared_ptr<Node> left_;
        std::shared_ptr<Node> right_;
        std::weak_ptr<Node> parent_;
        uint32_t priority_;
        std::optional<T> value_;
    };

    CartesianBST() {
        root_ = std::make_shared<Node>();
        begin_ = root_;
        end_ = root_;
    }

    template <class InitIterator>
    CartesianBST(InitIterator begin, InitIterator end) : CartesianBST() {
        for (InitIterator cur(begin); cur != end; ++cur) {
            Insert(*cur);
        }
    }
    CartesianBST(std::initializer_list<T> list) : CartesianBST() {
        for (const T& value : list) {
            Insert(value);
        }
    }

    CartesianBST(const CartesianBST& other) : CartesianBST() {
        for (const T& value : other) {
            Insert(value);
        }
    }
    CartesianBST(CartesianBST&& other) noexcept : CartesianBST() {
        std::swap(root_, other.root_);
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
    }
    CartesianBST(std::shared_ptr<IBST<T>> other)
        : CartesianBST(*dynamic_cast<CartesianBST<T>*>(other.get())) {
    }
    CartesianBST& operator=(const CartesianBST& other) {
        if (root_ == other.root_) {
            return *this;
        }
        root_ = std::make_shared<Node>();
        begin_ = root_;
        end_ = root_;
        for (const T& value : other) {
            Insert(value);
        }
        return *this;
    }
    CartesianBST& operator=(CartesianBST&& other) noexcept {
        if (root_ == other.root_) {
            return *this;
        }
        std::swap(root_, other.root_);
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
        return *this;
    }

    ~CartesianBST() override {
        root_ = begin_ = end_ = nullptr;
    }

private:
    std::shared_ptr<Node> begin_;
    std::shared_ptr<Node> end_;
    std::shared_ptr<Node> root_;

    /* ---------------------------------------------------
     * --------------ITERATOR IMPLEMENTATION--------------
     * ---------------------------------------------------
     */

    class CartesianBSTItImpl : public BaseImpl {
    public:
        CartesianBSTItImpl() = delete;
        explicit CartesianBSTItImpl(std::shared_ptr<Node> pointer) : it_(pointer) {
        }
        CartesianBSTItImpl(const CartesianBSTItImpl& other) : it_(other.it_) {
        }

        std::shared_ptr<BaseImpl> Clone() const override {
            return std::make_shared<CartesianBSTItImpl>(*this);
        }
        void Increment() override {
            if (!it_->value_) {
                throw std::runtime_error("Index out of range while increasing");
            }
            if (it_->right_) {
                it_ = it_->right_;
                while (it_->left_) {
                    it_ = it_->left_;
                }
            } else {
                auto parent = it_->parent_.lock();
                while (parent->right_ == it_) {
                    it_ = parent;
                    parent = it_->parent_.lock();
                }
                it_ = parent;
            }
        }
        void Decrement() override {
            if (it_->left_) {
                it_ = it_->left_;
                while (it_->right_) {
                    it_ = it_->right_;
                }
            } else {
                auto parent = it_->parent_.lock();
                while (parent && parent->left_ == it_) {
                    it_ = parent;
                    parent = it_->parent_.lock();
                }
                if (parent) {
                    it_ = parent;
                } else {
                    throw std::runtime_error("Index out of range while decreasing");
                }
            }
        }
        const T Dereferencing() const override {
            if (!it_->value_) {
                throw std::runtime_error("Index out of range on operator*");
            }
            return *(it_->value_);
        }
        const T* Arrow() const override {
            if (!it_->value_) {
                throw std::runtime_error("Index out of range on operator->");
            }
            return &(*it_->value_);
        }
        bool IsEqual(std::shared_ptr<BaseImpl> other) const override {
            auto casted = std::dynamic_pointer_cast<CartesianBSTItImpl>(other);
            if (!casted) {
                return false;
            }
            return it_ == casted->it_;
        }

    private:
        std::shared_ptr<Node> it_;
    };

    std::shared_ptr<BaseImpl> Begin() const override {
        return std::make_shared<CartesianBSTItImpl>(begin_);
    }
    std::shared_ptr<BaseImpl> End() const override {
        return std::make_shared<CartesianBSTItImpl>(end_);
    }

    void Make(std::initializer_list<T> list) {
    }

    void MergeRight(std::shared_ptr<IBST<T>> other) override {
        auto other_cast = std::dynamic_pointer_cast<CartesianBST<T>>(other);
        end_ = other_cast.end_;
        root_ = CartesianBST<T>::Merge(root_, other_cast.root_);
    }

    static std::shared_ptr<Node> Merge(std::shared_ptr<Node> lhs, std::shared_ptr<Node> rhs) {
        if (!lhs) {
            return rhs;
        } else if (!rhs) {
            return lhs;
        } else if (lhs->priority_ < rhs->priority_) {
            lhs->right_ = Merge(lhs->right_, rhs);
            if (lhs->right_) {
                lhs->right_->parent_ = lhs;
            }
            return lhs;
        } else {
            rhs->left_ = Merge(lhs, rhs->left_);
            if (rhs->left_) {
                rhs->left_->parent_ = rhs;
            }
            return rhs;
        }
    }

    static void Split(std::shared_ptr<Node> root, const std::optional<T>& value,
                      std::shared_ptr<Node>& left_sub, std::shared_ptr<Node>& right_sub) {
        std::shared_ptr<Node> new_subtree = nullptr;
        if (value < root->value_) {
            if (!root->left_) {
                left_sub = nullptr;
            } else {
                Split(root->left_, value, left_sub, new_subtree);
                root->left_ = new_subtree;
                if (new_subtree) {
                    new_subtree->parent_ = root;
                }
                if (left_sub) {
                    left_sub->parent_ = std::weak_ptr<Node>();
                }
            }
            right_sub = root;
        } else {
            if (!root->right_) {
                right_sub = nullptr;
            } else {
                Split(root->right_, value, new_subtree, right_sub);
                root->right_ = new_subtree;
                if (new_subtree) {
                    new_subtree->parent_ = root;
                }
                if (right_sub) {
                    right_sub->parent_ = std::weak_ptr<Node>();
                }
            }
            left_sub = root;
        }
    }
};
