#ifndef NEWCOURSEWORK_AVL_TREE_H
#define NEWCOURSEWORK_AVL_TREE_H

#include "../binary_search_tree/binary_search_tree.h"

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class avl_tree : public binary_search_tree<tkey, tvalue, tkey_comparer>{
protected:
    struct avl_node: public binary_search_tree<tkey, tvalue, tkey_comparer>::node{
        size_t height;
        avl_node() = default;
        avl_node(const tkey& key, const tvalue& val, avl_node* left, avl_node* right, size_t h):
                binary_search_tree<tkey, tvalue, tkey_comparer>::node(key, val, left, right){
            height = h;
        }

        virtual ~avl_node() = default;
    };

protected:
    class avl_insert_class final: public binary_search_tree<tkey, tvalue, tkey_comparer>::insert_class{
    private:
        avl_tree<tkey, tvalue, tkey_comparer>* _tree;
    public:
        explicit avl_insert_class(avl_tree<tkey, tvalue, tkey_comparer>* tree):
                binary_search_tree<tkey, tvalue, tkey_comparer>::insert_class(tree) {

        };

    size_t get_height(avl_node* node) const{
        if (node == nullptr) return 0;
        return node->height;
    }

    int difference(avl_node* node) const{
        int res = (int)get_height(reinterpret_cast<avl_node*>(node->left_node)) - (int)get_height(reinterpret_cast<avl_node*>(node->right_node));
        return res;
    }

    void set_height(avl_node* current_node) const{
        auto* a = reinterpret_cast<avl_node*>(current_node->left_node);
        auto* b = reinterpret_cast<avl_node*>(current_node->right_node);
        current_node->height = std::max(get_height(a), get_height(b)) + 1;
    }
    private:
        size_t get_size_of_node() const override{
            return sizeof(avl_node);
        }

        void get_node_constructor(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** x, const tkey& key, const tvalue& value) const override{
            new (*(reinterpret_cast<avl_node**>(x))) avl_node{key, value, nullptr, nullptr,  1};
        }
    private:

        void insert_fixup(avl_node** root, std::stack<avl_node*>* way_to_insert, logger* log) const {
            avl_node *cur_node = way_to_insert->top();
            way_to_insert->pop();

            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **dad;
            if (!way_to_insert->empty()) {
                if (way_to_insert->top()->left_node == cur_node) {
                    dad = reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **>(&(way_to_insert->top())->left_node);
                } else {
                    dad = reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **>(&(way_to_insert->top())->right_node);
                }
            } else {
                dad = reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **>(root);
            }


            if (difference(cur_node) < -1) {
                auto* node_new_h1 = reinterpret_cast<avl_node*>(cur_node);
                auto* node_new_h2 = reinterpret_cast<avl_node*>(cur_node->right_node);

                //TODO: проверить, нужно ли делать предварительно поворот
                if (difference(node_new_h2) > 0){
                    auto* node_new_h4 = reinterpret_cast<avl_node*>(node_new_h2->left_node);
                    auto* node_new_h3 = reinterpret_cast<avl_node*>(node_new_h2);
                    _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(cur_node->right_node)));
                    set_height(node_new_h3);
                    set_height(node_new_h4);
                }
                //TODO: done

                _tree->left_mini_rotate(dad);
                set_height(node_new_h1);
                set_height(node_new_h2);

                if(!way_to_insert->empty()){
                    insert_fixup(root, way_to_insert, log);
                }

            } else if (difference(cur_node) > 1) {
                auto* node_new_h1 = reinterpret_cast<avl_node*>(cur_node);
                auto* node_new_h2 = reinterpret_cast<avl_node*>(cur_node->left_node);

                //TODO: проверить, нужно ли делать предварительно поворот
                if (difference(node_new_h2) < 0){
                    auto* node_new_h4 = reinterpret_cast<avl_node*>(node_new_h2->right_node);
                    auto* node_new_h3 = reinterpret_cast<avl_node*>(node_new_h2);
                    _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(cur_node->left_node)));
                    set_height(node_new_h3);
                    set_height(node_new_h4);
                }
                //TODO: done

                _tree->right_mini_rotate(dad);
                set_height(node_new_h1);
                set_height(node_new_h2);

                if(!way_to_insert->empty()){
                    insert_fixup(root, way_to_insert, log);
                }

            } else {
                set_height(cur_node);
                if (!way_to_insert->empty()) insert_fixup(root, way_to_insert, log);
            }
        }

    public:
        void insert_after(const tkey& key, const tvalue& value, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** root, std::stack <typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* way_to_insert, logger* log) override{
            auto** root_as_avl = reinterpret_cast<avl_node**>(root);
            auto* way_to_insert_as_avl = reinterpret_cast<std::stack<avl_node*>*>(way_to_insert);
            insert_fixup(root_as_avl, way_to_insert_as_avl, log);

            if(log != nullptr){
                log->log("Insert finished successfully!", logger::severity::debug);
            }

        }
    };



class avl_remove_class: public binary_search_tree<tkey, tvalue, tkey_comparer>::remove_class{
    private:
        avl_tree<tkey, tvalue, tkey_comparer>* _tree;
    public:
        explicit avl_remove_class(avl_tree<tkey, tvalue, tkey_comparer>* tree):
                binary_search_tree<tkey, tvalue, tkey_comparer>::remove_class(tree){}
    protected:
        void destruct_node(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* node_to_destruct) const override{
            auto* node_as_avl = reinterpret_cast<avl_node*>(node_to_destruct);
            node_as_avl->~avl_node();
        }
        void additional_work(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* current, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* dad, size_t& side, size_t& additional) const override{}

        int difference(avl_node* node) const{
            int res = (int)get_height(reinterpret_cast<avl_node*>(node->left_node)) - (int)get_height(reinterpret_cast<avl_node*>(node->right_node));
            return res;
        }

        size_t get_height(avl_node* node) const{
            if (node == nullptr) return 0;
            return node->height;
        }

        void set_height(avl_node* current_node) const{
            auto* a = reinterpret_cast<avl_node*>(current_node->left_node);
            auto* b = reinterpret_cast<avl_node*>(current_node->right_node);
            current_node->height = std::max(get_height(a), get_height(b)) + 1;
        }

    private:
        void remove_fixup(avl_node** root, std::stack<avl_node*>* way_to_remove, logger* log){
            if (way_to_remove->empty()){
                if(*root != nullptr) set_height(*root);
                return;
            }
            avl_node* cur_node = way_to_remove->top();
            way_to_remove->pop();

            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **dad;
            if (!way_to_remove->empty()) {
                if (way_to_remove->top()->left_node == cur_node) {
                    dad = reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **>(&(way_to_remove->top())->left_node);
                } else {
                    dad = reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **>(&(way_to_remove->top())->right_node);
                }
            } else {
                dad = reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **>(root);
            }

            if (difference(cur_node) < -1) {
                auto* node_new_h1 = reinterpret_cast<avl_node*>(cur_node);
                auto* node_new_h2 = reinterpret_cast<avl_node*>(cur_node->right_node);

                if (difference(node_new_h2) > 0) {
                    auto *node_new_h4 = reinterpret_cast<avl_node*>(node_new_h2->left_node);
                    auto *node_new_h3 = reinterpret_cast<avl_node*>(node_new_h2);
                    _tree->right_mini_rotate(
                            reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **>(&(cur_node->right_node)));
                    set_height(node_new_h3);
                    set_height(node_new_h4);
                }

                _tree->left_mini_rotate(dad);
                set_height(node_new_h1);
                set_height(node_new_h2);

                if(!way_to_remove->empty()){
                    remove_fixup(root, way_to_remove, log);
                }

            } else if (difference(cur_node) > 1) {
                auto* node_new_h1 = reinterpret_cast<avl_node*>(cur_node);
                auto* node_new_h2 = reinterpret_cast<avl_node*>(cur_node->left_node);

                if (difference(node_new_h2) < 0){
                    auto* node_new_h4 = reinterpret_cast<avl_node*>(node_new_h2->right_node);
                    auto* node_new_h3 = reinterpret_cast<avl_node*>(node_new_h2);
                    _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(cur_node->left_node)));
                    set_height(node_new_h3);
                    set_height(node_new_h4);
                }

                _tree->right_mini_rotate(dad);
                set_height(node_new_h1);
                set_height(node_new_h2);

                if(!way_to_remove->empty()){
                    remove_fixup(root, way_to_remove, log);
                }

            } else {
                set_height(cur_node);
                if (!way_to_remove->empty()) remove_fixup(root, way_to_remove, log);
            }
        }

    public:
        void remove_after(const tkey & key, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** root, logger* log, std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* way_to_remove,size_t& side, size_t& dop) override{
            auto** root_as_avl = reinterpret_cast<avl_node**>(root);
            auto* way_to_remove_avl = reinterpret_cast<std::stack<avl_node*>*>(way_to_remove);
            remove_fixup(root_as_avl, way_to_remove_avl, log);

            if(log != nullptr){
                log->log("Remove finished successfully!", logger::severity::debug);
            }

        }


    };


public:
    explicit avl_tree(memory* alloc_tree = nullptr, logger* log_tree = nullptr):
            binary_search_tree<tkey, tvalue, tkey_comparer>(alloc_tree, log_tree, nullptr, new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_class(this), new avl_insert_class(this), new avl_remove_class(this)){

    }
private:
    size_t node_size() const override{
        return sizeof(avl_node);
    }

    void node_construct(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* current_node_copy, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* current_node) const override{
        auto* current_node_copy_as_avl = reinterpret_cast<avl_node*>(current_node_copy);
        auto* current_node_as_avl = reinterpret_cast<avl_node*>(current_node);
        new (current_node_copy_as_avl) avl_node {current_node_as_avl->key, current_node_as_avl->value, reinterpret_cast<avl_node*>(this->copy_inner(current_node_as_avl->left_node)),reinterpret_cast<avl_node*>(this->copy_inner(current_node_as_avl->right_node)), current_node_as_avl->height};
    }

    void additional_data(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* current_node_copy, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* current_node) const override{
        auto* current_node_copy_as_avl = reinterpret_cast<avl_node*>(current_node_copy);
        auto* current_node_as_avl = reinterpret_cast<avl_node*>(current_node);
        current_node_copy_as_avl->height = current_node_as_avl->height;
    }

public:
    avl_tree<tkey, tvalue,tkey_comparer>& operator=(const avl_tree<tkey, tvalue, tkey_comparer>& other){
        if (*this != other){
            this->clear();
            this->_root = other.copy();
            return *this;
        }
        return *this;
    }

    avl_tree<tkey, tvalue,tkey_comparer>& operator=(const avl_tree<tkey, tvalue, tkey_comparer>&& other){
        if (*this != other){
            this->clear();
            this->_root = other._root();
            this->_logger = other._logger;
            this->_allocator = other._allocator;
            delete other._class_remove;
            delete other._class_insert;
            delete other._class_find;
            other._logger = nullptr;
            other._allocator = nullptr;
            other._class_find = nullptr;
            other._class_insert = nullptr;
            other._class_remove = nullptr;
            other._root = nullptr;
        }
        return *this;
    }

    avl_tree(const avl_tree<tkey, tvalue, tkey_comparer>& other){
        this->_logger = other._logger;
        this->_allocator = other._allocator;
        this->_class_find = new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_class(this);
        this->_class_insert = new avl_insert_class(this);
        this->_class_remove = new avl_remove_class(this);
        this->_root = other.copy();
    }

    avl_tree(const avl_tree<tkey, tvalue, tkey_comparer>&& other)  noexcept {
        this->_logger = other._logger;
        this->_allocator = other._allocator;
        this->_class_find = new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_class(this);
        this->_class_insert = new avl_insert_class(this);
        this->_class_remove = new avl_remove_class(this);
        delete other._class_remove;
        delete other._class_insert;
        delete other._class_find;
        this->_root = other._root;
        other._logger = nullptr;
        other._allocator = nullptr;
        other._class_find = nullptr;
        other._class_insert = nullptr;
        other._class_remove = nullptr;
    }

    ~avl_tree() = default;

};

#endif //NEWCOURSEWORK_AVL_TREE_H
