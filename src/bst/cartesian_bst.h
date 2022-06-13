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
        Node() = delete;
        Node(std::optional<T> value, uint32_t priority) : priority_(priority), value_(value) {
            left_ = nullptr;
            right_ = nullptr;
            parent_ = std::weak_ptr<Node>();
            child_count_ = 0;
            child_with_level_edges_count_ = 0;
        }

        std::shared_ptr<Node> left_;
        std::shared_ptr<Node> right_;
        std::weak_ptr<Node> parent_;
        uint32_t priority_;
        uint32_t child_count_;
        uint32_t child_with_level_edges_count_;
        std::optional<T> value_;
    };

    typedef typename IBST<T>::IBSTItImpl BaseItImpl;

    class CartesianBSTItImpl : public BaseItImpl {
    public:
        CartesianBSTItImpl() = delete;
        CartesianBSTItImpl(std::shared_ptr<Node> pointer, bool is_end = false)
            : it_(pointer), is_end_(is_end) {
        }
        CartesianBSTItImpl(const CartesianBSTItImpl& other)
            : it_(other.it_), is_end_(other.is_end_) {
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
                auto parent = it_->parent_.lock(), start = it_;
                while (parent && parent->right_ == it_) {
                    it_ = parent;
                    parent = it_->parent_.lock();
                }
                if (parent) {
                    it_ = parent;
                } else {
                    it_ = start;
                    is_end_ = true;
                }
            }
        }
        void Decrement() override {
            if (is_end_) {
                if (!it_) {
                    throw std::runtime_error("Index out of range while decreasing");
                }
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
        void NextWithLevelEdges() override {
            if (is_end_) {
                throw std::runtime_error("Index out of range while increasing");
            }
            if (it_->right_ && it_->right_->child_with_level_edges_count_) {
                it_ = it_->right_;
                while (it_->left_ && it_->left_->child_with_level_edges_count_) {
                    it_ = it_->left_;
                }
                if (!it_->left_) {
                    
                }
            } else {
                auto parent = it_->parent_.lock(), start = it_;
                while (parent && parent->right_ == it_) {
                    it_ = parent;
                    parent = it_->parent_.lock();
                }
                if (parent) {
                    it_ = parent;
                } else {
                    it_ = start;
                    is_end_ = true;
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

        std::shared_ptr<BaseItImpl> FindRoot() const override {
            auto it = it_;
            while (!it->parent_.expired()) {
                it = it->parent_.lock();
            }
            return std::make_shared<CartesianBSTItImpl>(it);
        }

        std::pair<std::shared_ptr<IBST<T>>, std::shared_ptr<IBST<T>>> Split() const override {
            if (is_end_) {
                throw std::logic_error("Cannot split in empty parts");
            }
            std::shared_ptr<Node> rhs = it_;
            std::shared_ptr<Node> lhs = rhs->left_;
            if (lhs) {
                rhs->child_count_ -= lhs->child_count_;
                rhs->child_with_level_edges_count_ -= lhs->child_with_level_edges_count_;
                lhs->parent_ = std::weak_ptr<Node>();
            }
            rhs->left_ = nullptr;
            std::shared_ptr<Node> from = rhs->parent_.lock();
            while (from) {
                // The order here is important. We should look at rhs first
                if (from->right_ == rhs) {
                    if (rhs) {
                        from->child_count_ -= rhs->child_count_;
                        from->child_with_level_edges_count_ -= rhs->child_with_level_edges_count_;
                        rhs->parent_ = std::weak_ptr<Node>();
                    }
                    from->right_ = lhs;
                    if (lhs) {
                        from->child_count_ += lhs->child_count_;
                        from->child_with_level_edges_count_ += lhs->child_with_level_edges_count_;
                        lhs->parent_ = from;
                    }
                    lhs = from;
                } else if (from->left_ == rhs) {
                    rhs = from;
                } else if (from->right_ == lhs) {
                    lhs = from;
                } else if (from->left_ == lhs) {
                    if (lhs) {
                        from->child_count_ -= lhs->child_count_;
                        from->child_with_level_edges_count_ -= lhs->child_with_level_edges_count_;
                        lhs->parent_ = std::weak_ptr<Node>();
                    }
                    from->left_ = rhs;
                    if (rhs) {
                        from->child_count_ += rhs->child_count_;
                        from->child_with_level_edges_count_ += rhs->child_with_level_edges_count_;
                        rhs->parent_ = from;
                    }
                    rhs = from;
                } else {
                    throw std::logic_error("Impossible behaviour");
                }
                from = from->parent_.lock();
            }
            return std::make_pair(std::make_shared<CartesianBST<T>>(lhs),
                                  std::make_shared<CartesianBST<T>>(rhs));
        }

        void SetHasLevelEdges(bool has_level_edges) const override {
            size_t child_level_edges = (it_->left_ ? it_->left_->child_with_level_edges_count_ : 0) + (it_->right_ ? it_->right_->child_with_level_edges_count_ : 0);
            int add = static_cast<int>(has_level_edges) - static_cast<int>(it_->child_with_level_edges_count_ - child_level_edges);
            std::shared_ptr<Node> from = it_;
            while(from) {
                from->child_with_level_edges_count_ += add;
                from = from->parent_.lock();
            }
        }

        void SetIsVertex(bool is_vertex) const override {
            size_t children_child_count = (it_->left_ ? it_->left_->child_count_ : 0) + (it_->right_ ? it_->right_->child_count_ : 0);
            int add = static_cast<int>(is_vertex) - static_cast<int>(it_->child_count_ - children_child_count);
            std::shared_ptr<Node> from = it_;
            while(from) {
                from->child_count_ += add;
                from = from->parent_.lock();
            }
        }

    private:
        std::shared_ptr<Node> it_;
        bool is_end_;
    };

public:
    CartesianBST() = delete;

    explicit CartesianBST(std::shared_ptr<Node> root) : root_(root) {
        RecalcBeginEnd();
    }

    template <class InitIterator>
    CartesianBST(InitIterator begin, InitIterator end) {
        if (begin == end) {
            begin_ = end_ = root_ = nullptr;
            is_empty_ = true;
            return;
        }
        std::vector<uint32_t> priorities;
        for (InitIterator cur(begin); cur != end; ++cur) {
            priorities.emplace_back(Random::Next());
        }
        MakeRecursive(root_, begin, end, priorities.begin(), priorities.end());
        RecalcBeginEnd();
    }

    CartesianBST(CartesianBST&& other) noexcept : CartesianBST() {
        std::swap(root_, other.root_);
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
        std::swap(is_empty_, other.is_empty_);
    }
    CartesianBST(std::shared_ptr<IBST<T>> other)
        : CartesianBST(*std::dynamic_pointer_cast<CartesianBST<T>>(other)) {
    }

    CartesianBST& operator=(CartesianBST&& other) noexcept {
        if (root_ == other.root_) {
            return *this;
        }
        std::swap(root_, other.root_);
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
        std::swap(is_empty_, other.is_empty_);
        return *this;
    }

    ~CartesianBST() override {
        root_ = begin_ = end_ = nullptr;
    }

private:
    std::shared_ptr<Node> begin_;
    std::shared_ptr<Node> end_;
    std::shared_ptr<Node> root_;
    bool is_empty_;

    void Merge(std::shared_ptr<IBST<T>> other) override {
        if (other->empty()) {
            return;
        }
        auto other_cast = std::dynamic_pointer_cast<CartesianBST<T>>(other);
        if (is_empty_) {
            *this = std::move(*other_cast);
            return;
        }
        end_ = other_cast->end_;
        root_ = CartesianBST<T>::MergeRecursive(root_, other_cast->root_);
    }

    bool IsEmpty() const override {
        return is_empty_;
    }

    size_t Size() const override {
        return root_->child_count_;
    }

    std::shared_ptr<BaseItImpl> Begin() const override {
        if (is_empty_) {
            return std::make_shared<CartesianBSTItImpl>(end_, true);
        }
        return std::make_shared<CartesianBSTItImpl>(begin_);
    }
    std::shared_ptr<BaseItImpl> End() const override {
        return std::make_shared<CartesianBSTItImpl>(end_, true);
    }

    void RecalcBeginEnd() {
        if (!root_) {
            begin_ = end_ = root_;
            is_empty_ = true;
            return;
        }
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
        is_empty_ = false;
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
            from->child_count_ += from->left_->child_count_;
            from->child_with_level_edges_count_ += from->left_->child_with_level_edges_count_;
        }
        if (from->right_) {
            from->right_->parent_ = from;
            from->child_count_ += from->right_->child_count_;
            from->child_with_level_edges_count_ += from->right_->child_with_level_edges_count_;
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
                lhs->child_count_ += lhs->right_->child_count_;
                lhs->child_with_level_edges_count_ += lhs->right_->child_with_level_edges_count_;
            }
            return lhs;
        } else {
            rhs->left_ = MergeRecursive(lhs, rhs->left_);
            if (rhs->left_) {
                rhs->left_->parent_ = rhs;
                rhs->child_count_ += rhs->left_->child_count_;
                rhs->child_with_level_edges_count_ += rhs->left_->child_with_level_edges_count_;
            }
            return rhs;
        }
    }
};
