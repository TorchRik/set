#include "main.h"

bool BLACK = false, RED = true;

template<class T>
class Set {
public:
    struct Node {
        T val;
        bool color = RED;  // 0 means BLACK, 1 - RED
        Node *left = nullptr, *right = nullptr, *parent = nullptr;
        ~Node() {}
    };
    Set() {
        none = new Node;
        none->left = none->right = none->parent = none;
        root = none;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        none = new Node;
        none->left = none->right = none->parent = none;
        root = none;
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    Set(std::initializer_list<T> elems) {
        none = new Node;
        none->left = none->right = none->parent = none;
        root = none;
        for (auto& elem : elems) {
            insert(elem);
        }
    }

    Set(const Set& other) {
        none = new Node();
        none->left = none->right = none->parent = none;
        root = deep_copy(other.root);
        my_size = other.my_size;
    }
    Set& operator=(const Set&other) {
        if (this == &other) {
            return *this;
        }
        erase_all(root);
        delete none;
        none = new Node();
        none->left = none->right = none->parent = none;
        root = deep_copy(other.root);
        my_size = other.my_size;
        return *this;
    }

    bool empty() const {
        return root == none;
    }

    size_t size() const {
        return my_size;
    }

    ~Set() {
        erase_all(root);
        delete none;
    }

    void insert(const T& elem) {
        Node* insert_elem = new Node{elem, RED, none, none, none};
        my_size++;
        if (empty()) {
            root = insert_elem;
        } else {
            Node* now = root, *parent = none;
            while (now != none) {
                parent = now;
                if (!(now->val < elem) && !(elem < now->val)) {
                    my_size--;
                    delete insert_elem;
                    return;
                }
                if (now->val < elem) {
                    now = now->right;
                } else {
                    now = now->left;
                }
            }
            insert_elem->parent = parent;
            if (parent->val < elem) {
                parent->right = insert_elem;
            } else {
                parent->left = insert_elem;
            }
        }
        fix_insert(insert_elem);
    }

    void erase(const T& elem) {
        Node* node = find_node(elem);
        if (node != none) {
            my_size--;
        }
        deleteNode(node);
    }

    struct iterator {
        Node *node;
        Node *none;
        Node *root;

        iterator() {
            node = none = root = nullptr;
        }

        iterator(Node* a, Node* tree_none, Node* tree_root) {
            node = a;
            none = tree_none;
            root = tree_root;
        }
        Node* get_next_elem(Node* node, Node* none) const {
            if (node->right != none) {
                node = node->right;
                while (node->left != none) {
                    node = node->left;
                }
                return node;
            } else {
                while (true) {
                    if (node->parent == none) {
                        node = none;
                        return node;
                    }
                    if (node->parent->left == node) {
                        node = node->parent;
                        return node;
                    }
                    node = node->parent;
                }
            }
        }

        Node* get_previous(Node* node, Node* none, Node* root) const {
            if (node == none) {
                node = root;
                while (node->right != none) {
                    node = node->right;
                }
                return node;
            }
            if (node->left != none) {
                node = node->left;
                while (node->right != none) {
                    node = node->right;
                }
                return node;
            } else {
                while (true) {
                    if (node->parent == none) {
                        node = none;
                        return node;
                    }
                    if (node->parent->right == node) {
                        node = node->parent;
                        return node;
                    }
                    node = node->parent;
                }
            }
        }

        iterator operator++() {
            node = get_next_elem(node, none);
            return iterator(node, none, root);
        }

        iterator operator++(int) {
            Node* last = node;
            node = get_next_elem(node, none);
            return iterator(last, none, root);
        }
        iterator operator--() {
            node = get_previous(node, none, root);
            return iterator(node, none, root);
        }
        iterator operator--(int) {
            Node* last = node;
            node = get_previous(node, none, root);
            return iterator(last, none, node);
        }

        bool operator==(const iterator& other) const {
            return (this->node == other.node && this->root == other.root);
        }
        T operator*() const {
            return node->val;
        }
        T* operator->() const {
            return &node->val;
        }
        bool operator!=(const iterator& other)const {
            return (this->node != other.node || this->root != other.root);
        }
    };

    iterator begin() const {
        Node * now = root;
        while (now->left != none) {
            now = now->left;
        }
        return iterator(now, none, root);
    }

    iterator end() const {
        return iterator(none, none, root);
    }

    iterator find(const T& elem) const {
        return iterator(find_node(elem), none, root);
    }

    iterator lower_bound(const T& elem) const {
        return iterator(lower_bound_node(elem), none, root);
    }
    size_t contains(const T& elem) {
        return find(elem) != end();
    }
    void add(const T &elem) {
        insert(elem);
    }
private:
    Node *root;
    Node *none;
    size_t my_size = 0;

    bool is_left_child(Node* x) {
        return x->parent && x->parent->left == x;
    }

    Node* find_node(const T& elem) const {
        if (empty()) {
            return none;
        }
        Node* now = root;
        while (now != none) {
            if (now->val < elem) {
                now = now->right;
            } else if (!(now->val < elem) && !(elem < now->val)) {
                break;
            } else {
                now = now->left;
            }
        }
        return now;
    }

    Node* lower_bound_node(const T& elem) const  {
        if (empty()) {
            return none;
        }
        Node* now = root;
        Node* last = none;
        while (now != none) {
            if (now->val < elem) {
                now = now->right;
            } else if (!(now->val < elem) && !(elem < now->val)) {
                break;
            } else {
                last = now;
                now = now->left;
            }
        }
        if (now == none) {
            return last;
        }
        return now;
    }

    void left_rotate(Node* x) {
        Node *y = x->right;
        x->right = y->left;
        if (y->left != none) {
            y->left->parent = x;
        }
        if (y != none) {
            y->parent = x->parent;
        }
        if (x != root) {
            if (x == x->parent->left) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
        } else {
            root = y;
        }
        y->left = x;
        if (x != none) {
            x->parent = y;
        }
    }

    void right_rotate(Node* x) {
        Node *y = x->left;
        x->left = y->right;
        if (y->right != none) {
            y->right->parent = x;
        }
        if (y != none) {
            y->parent = x->parent;
        }
        if (x != root) {
            if (x == x->parent->left) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
        } else {
            root = y;
        }
        y->right = x;
        if (x != none) {
            x->parent = y;
        }
    }

    void fix_insert(Node* node) {
        while (node != root && node->parent->color == RED) {
            if (node->parent == node->parent->parent->left) {
                Node* uncle = node->parent->parent->right;
                if (uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        left_rotate(node);
                    }
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    right_rotate(node->parent->parent);
                }
            } else {
                Node* uncle = node->parent->parent->left;
                if (uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->left) {
                        node = node->parent;
                        right_rotate(node);
                    }
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    left_rotate(node->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void fix_delete(Node* node) {
        while (node != root && node->color == BLACK) {
            if (node == node->parent->left) {
                Node *w = node->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    node->parent->color = RED;
                    left_rotate (node->parent);
                    w = node->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    node = node->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        right_rotate (w);
                        w = node->parent->right;
                    }
                    w->color = node->parent->color;
                    node->parent->color = BLACK;
                    w->right->color = BLACK;
                    left_rotate (node->parent);
                    node = root;
                }
            } else {
                Node *w = node->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    node->parent->color = RED;
                    right_rotate (node->parent);
                    w = node->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    node = node->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        left_rotate (w);
                        w = node->parent->left;
                    }
                    w->color = node->parent->color;
                    node->parent->color = BLACK;
                    w->left->color = BLACK;
                    right_rotate (node->parent);
                    node = root;
                }
            }
        }
        node->color = BLACK;
    }

    void deleteNode(Node *node) {
        if (!node || node == none) {
            return;
        }
        Node *x = none, *y = none;
        if (node->left == none || node->right == none) {
            y = node;
        } else {
            y = node->right;
            while (y->left != none) {
                y = y->left;
            }
        }
        if (y->left != none) {
            x = y->left;
        } else {
            x = y->right;
        }
        x->parent = y->parent;
        if (y->parent != none) {
            if (y == y->parent->left) {
                y->parent->left = x;
            } else {
                y->parent->right = x;
            }
        } else {
            root = x;
        }
        if (y != node) {
            node->val = y->val;
        }
        if (y->color == BLACK) {
            fix_delete(x);
        }
        if (y != none) {
            delete y;
        }
    }

    void erase_all(Node* node) {
        if (node == none) {
            return;
        }
        erase_all(node->left);
        erase_all(node->right);
        delete node;
    };
    Node* deep_copy(Node* now) {
        if (now->left == now) {
            return none;
        }
        Node* next = new Node{now->val, now->color, none, none, none};
        Node* left = deep_copy(now->left);
        Node* right = deep_copy(now->right);
        if (left != none) {
            left->parent = next;
        }
        if (right != none) {
            right->parent = next;
        }
        next->left = left;
        next->right = right;
        return next;
    }
};
