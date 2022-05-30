#pragma once

#include <exception>
#include <initializer_list>
#include <memory>
#include <optional>
#include <random>
#include <vector>

template <class T>
class IBST;

template <class T>
class CartesianBST : public IBST<T> {
private:
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

    struct Node {
        Node(std::optional<T> value, uint32_t priority) : priority_(priority), value_(value) {
            left_ = nullptr;
            right_ = nullptr;
            parent_ = std::weak_ptr<Node>();
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

    typedef typename IBST<T>::IBSTItImpl BaseItImpl;

    class CartesianBSTItImpl : public BaseItImpl {
    public:
        CartesianBSTItImpl() = delete;
        explicit CartesianBSTItImpl(std::shared_ptr<Node> pointer, bool is_end = false) : it_(pointer), is_end_(is_end) {
        }
        CartesianBSTItImpl(const CartesianBSTItImpl& other) : it_(other.it_), is_end_(other.is_end_) {
        }

        std::shared_ptr<BaseItImpl> Clone() const override {
            return std::make_shared<CartesianBSTItImpl>(*this);
        }

        void Increment() override {
            if (is_end_) {
                throw std::runtime_error("Index out of range while increasing");
            }
            if (it_->right_) {
                it_ = it_->right_;
                while (it_->left_) {
                    it_ = it_->left_;
                }
            } else {
                auto parent = it_->parent_.lock();
                while (parent && parent->right_ == it_) {
                    it_ = parent;
                    parent = it_->parent_.lock();
                }
                if (parent) {
                    it_ = parent;
                } else {
                    is_end_ = true;
                }
            }
        }
        void Decrement() override {
            if (is_end_) {
                is_end_ = false;
                return;
            }
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
            if (is_end_) {
                throw std::runtime_error("Index out of range in operator*");
            }
            return *(it_->value_);
        }
        const T* Arrow() const override {
            if (is_end_) {
                throw std::runtime_error("Index out of range in operator->");
            }
            return &(*it_->value_);
        }

        bool IsEqual(std::shared_ptr<BaseItImpl> other) const override {
            auto casted = std::dynamic_pointer_cast<CartesianBSTItImpl>(other);
            if (!casted) {
                return false;
            }
            return it_ == casted->it_ && is_end_ == casted->is_end_;
        }

    private:
        std::pair<std::shared_ptr<Node>, bool> Get() const {
            return std::make_pair(it_, is_end_);
        }

        std::shared_ptr<Node> it_;
        bool is_end_;
    };

public:
    CartesianBST() = delete;

    template <class InitIterator>
    CartesianBST(InitIterator begin, InitIterator end) {
        if (begin == end) {
            begin_ = end_ = root_ = nullptr;
            return;
        }
        std::vector<uint32_t> priorities;
        for (InitIterator cur(begin); cur != end; ++cur) {
            priorities.emplace_back(Random::Next());
        }
        MakeRecursive(root_, begin, end, priorities.begin(), priorities.end());
        RecalcBeginEnd();
    }

    CartesianBST(const CartesianBST& other) : CartesianBST() {
        throw std::logic_error("Not implemented");
        // for (const T& value : other) {
        //     Insert(value);
        // }
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
        throw std::logic_error("Not implemented");
        // if (root_ == other.root_) {
        //     return *this;
        // }
        // root_ = std::make_shared<Node>();
        // begin_ = root_;
        // end_ = root_;
        // for (const T& value : other) {
        //     Insert(value);
        // }
        // return *this;
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

    CartesianBST(std::shared_ptr<Node> root) : root_(root) {
        RecalcBeginEnd();
    }

    std::shared_ptr<IBST<T>> Split(std::shared_ptr<BaseItImpl> where) override {
        auto node = std::dynamic_pointer_cast<CartesianBSTItImpl>(where)->Get();
        if (node.second) {
            throw std::logic_error("Cannot split in empty parts");
        }
        root_ = node.first;
        std::shared_ptr<Node> other = root_->right_;
        root_->right_ = nullptr;
        other->parent_ = std::weak_ptr<Node>();
        SplitRecursive(root_->parent_.lock(), root_, other);
        RecalcBeginEnd();
        return std::make_shared<CartesianBST<T>>(other);
    }

    void Merge(std::shared_ptr<IBST<T>> other) override {
        auto other_cast = std::dynamic_pointer_cast<CartesianBST<T>>(other);
        end_ = other_cast->end_;
        root_ = CartesianBST<T>::MergeRecursive(root_, other_cast->root_);
    }

    void RecalcBeginEnd() {
        std::shared_ptr<Node> cur_node = root_;
        while (cur_node->left_) {
            cur_node = cur_node->left_;
        }
        begin_ = cur_node;
        cur_node = root_;
        while (cur_node->right_) {
            cur_node = cur_node->right_;
        }
        end_ = cur_node;
    }

    std::shared_ptr<BaseItImpl> Begin() const override {
        return std::make_shared<CartesianBSTItImpl>(begin_);
    }
    std::shared_ptr<BaseItImpl> End() const override {
        return std::make_shared<CartesianBSTItImpl>(end_, true);
    }

    /* ---------------------------------------------------
     * ------------------STATIC METHODS-------------------
     * ---------------------------------------------------
     */

    template <class InitIterator, class PriorityIterator>
    static void MakeRecursive(std::shared_ptr<Node>& from, InitIterator begin, InitIterator end,
                              PriorityIterator pbegin, PriorityIterator pend) {
        if (begin == end) {
            return;
        }
        PriorityIterator pmax(pbegin);
        InitIterator cur(begin), vmax(begin);
        for (PriorityIterator pcur(pbegin); pcur != pend; ++pcur, ++cur) {
            if (*pcur > *pmax) {
                pmax = pcur;
                vmax = cur;
            }
        }
        from = std::make_shared<Node>(*vmax, *pmax);
        if (cur == begin && ++cur == end) {
            return;
        }
        MakeRecursive(from->left_, begin, vmax, pbegin, pmax);
        MakeRecursive(from->right_, ++vmax, end, ++pmax, pend);
        if (from->left_) {
            from->left_->parent_ = from;
        }
        if (from->right_) {
            from->right_->parent_ = from;
        }
    }

    static std::shared_ptr<Node> MergeRecursive(std::shared_ptr<Node> lhs,
                                                std::shared_ptr<Node> rhs) {
        if (!lhs) {
            return rhs;
        } else if (!rhs) {
            return lhs;
        } else if (lhs->priority_ < rhs->priority_) {
            lhs->right_ = MergeRecursive(lhs->right_, rhs);
            if (lhs->right_) {
                lhs->right_->parent_ = lhs;
            }
            return lhs;
        } else {
            rhs->left_ = MergeRecursive(lhs, rhs->left_);
            if (rhs->left_) {
                rhs->left_->parent_ = rhs;
            }
            return rhs;
        }
    }

    static void SplitRecursive(std::shared_ptr<Node> from,
                               std::shared_ptr<Node>& left_sub, std::shared_ptr<Node>& right_sub) {
        if (!from) {
            return;
        }
        if (from->right_ == left_sub) {
            left_sub = from;
        } else if (from->right_ == right_sub) {
            from->right_ = left_sub;
            if (left_sub) {
                left_sub->parent_ = from;
            }
            left_sub = from;
            if (right_sub) {
                right_sub->parent_ = std::weak_ptr<Node>();
            }
        } else if (from->left_ == right_sub) {
            right_sub = from;
        } else if (from->left_ == left_sub) {
            from->left_ = right_sub;
            if (right_sub) {
                right_sub->parent_ = from;
            }
            right_sub = from;
            if (left_sub) {
                left_sub->parent_ = std::weak_ptr<Node>();
            }
        }
        SplitRecursive(from->parent_.lock(), left_sub, right_sub);
    }
};
