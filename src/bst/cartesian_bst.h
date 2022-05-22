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
        size_ = 0;
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
        std::swap(size_, other.size_);
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
        size_ = 0;
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
        std::swap(size_, other.size_);
        return *this;
    }

    ~CartesianBST() override {
        root_ = begin_ = end_ = nullptr;
    }

private:
    std::shared_ptr<Node> begin_;
    std::shared_ptr<Node> end_;
    std::shared_ptr<Node> root_;
    size_t size_{};

    /* ---------------------------------------------------
     * --------------ITERATOR IMPLEMENTATION--------------
     * ---------------------------------------------------
     */

    class CartesianTreeItImpl : public BaseImpl {
    public:
        CartesianTreeItImpl() = delete;
        explicit CartesianTreeItImpl(std::shared_ptr<Node> pointer) : it_(pointer) {
        }
        CartesianTreeItImpl(const CartesianTreeItImpl& other) : it_(other.it_) {
        }

        std::shared_ptr<BaseImpl> Clone() const override {
            return std::make_shared<CartesianTreeItImpl>(*this);
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
            auto casted = std::dynamic_pointer_cast<CartesianTreeItImpl>(other);
            if (!casted) {
                return false;
            }
            return it_ == casted->it_;
        }

    private:
        std::shared_ptr<Node> it_;
    };

    std::shared_ptr<BaseImpl> Begin() const override {
        return std::make_shared<CartesianTreeItImpl>(begin_);
    }
    std::shared_ptr<BaseImpl> End() const override {
        return std::make_shared<CartesianTreeItImpl>(end_);
    }

    /* ---------------------------------------------------
     * ----------------PRIVATE FUNCTIONS------------------
     * ---------------------------------------------------
     */

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

    static bool InsertRecursive(std::shared_ptr<Node>& from, std::shared_ptr<Node> new_node) {
        if (!from) {
            from = new_node;
            return true;
        } else if (from->priority_ >= new_node->priority_) {
            std::shared_ptr<Node> left_sub = nullptr, right_sub = nullptr;
            Split(from, new_node->value_, left_sub, right_sub);
            if (left_sub) {
                std::shared_ptr<Node> max_v = left_sub;
                while (max_v->right_) {
                    max_v = max_v->right_;
                }
                if (max_v->value_ < new_node->value_) {
                    from = Merge(Merge(left_sub, new_node), right_sub);
                    return true;
                } else if (new_node->value_ < max_v->value_) {
                    throw std::runtime_error("Error in function Split()");
                } else {
                    from = Merge(left_sub, right_sub);
                    return false;
                }
            } else {
                new_node->right_ = right_sub;
                if (right_sub) {
                    right_sub->parent_ = new_node;
                }
                from = new_node;
                return true;
            }
        } else if ((from->value_ < new_node->value_) || (new_node->value_ < from->value_)) {
            bool result;
            if (new_node->value_ < from->value_) {
                result = InsertRecursive(from->left_, new_node);
                if (from->left_) {
                    from->left_->parent_ = from;
                }
            } else {
                result = InsertRecursive(from->right_, new_node);
                if (from->right_) {
                    from->right_->parent_ = from;
                }
            }
            return result;
        }
        return false;
    }
    static bool EraseRecursive(std::shared_ptr<Node>& from, const std::optional<T>& value) {
        bool result;
        if (!from) {
            return false;
        } else if (value < from->value_) {
            result = EraseRecursive(from->left_, value);
            if (from->left_) {
                from->left_->parent_ = from;
            }
        } else if (from->value_ < value) {
            result = EraseRecursive(from->right_, value);
            if (from->right_) {
                from->right_->parent_ = from;
            }
        } else {
            from = Merge(from->left_, from->right_);
            return true;
        }
        return result;
    }

    void RecalcBegin() {
        std::shared_ptr<Node> cur_node = root_;
        while (cur_node->left_) {
            cur_node = cur_node->left_;
        }
        begin_ = cur_node;
    }
};
