#ifndef NEWCOURSEWORK_SPLAY_TREE_H
#define NEWCOURSEWORK_SPLAY_TREE_H
#include "../binary_search_tree/binary_search_tree.h"

template<typename tkey, typename tvalue, typename tkey_comparer>
class splay_tree final: public binary_search_tree<tkey, tvalue, tkey_comparer>{
//private:
//    splay_tree<tkey, tvalue, tkey_comparer>* _tree;
public:
    void splay(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** root, std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* way_to_splay, logger* log){
        if (log != nullptr) {
            log->log("Splay has started working", logger::severity::debug);
        }
        if (way_to_splay->empty()){
            return;
        }
        auto* cur_node = way_to_splay->top();
        way_to_splay->pop();
        if (way_to_splay->empty()){
            return;
        }
        while (!way_to_splay->empty()){
            auto* dad = way_to_splay->top();
            way_to_splay->pop();
            if(way_to_splay->empty()){
                if(dad->right_node == cur_node){
                    this->left_mini_rotate(root);
                    if (log != nullptr) log->log("left mini rotate", logger::severity::debug);
                }else{
                    this->right_mini_rotate(root);
                    if (log != nullptr) log->log("right mini rotate", logger::severity::debug);
                }
            }else{
                auto* grand_dad = way_to_splay->top();
                way_to_splay->pop();
                if (grand_dad->left_node == dad){
                    if (dad->right_node == cur_node){
                        this->left_mini_rotate(&(grand_dad->left_node));
                        if (log != nullptr) log->log("left mini rotate", logger::severity::debug);
                        if (way_to_splay->empty()){
                            this->right_mini_rotate(root);
                            if (log != nullptr) log->log("right mini rotate", logger::severity::debug);
                        }else{
                            if (way_to_splay->top()->left_node == grand_dad){
                                this->right_mini_rotate(&(way_to_splay->top()->left_node));
                                if (log != nullptr) log->log("right mini rotate", logger::severity::debug);
                            }else{
                                this->right_mini_rotate(&(way_to_splay->top()->right_node));
                                if (log != nullptr) log->log("right mini rotate", logger::severity::debug);
                            }
                        }
                    }else{
                        this->right_mini_rotate(&(grand_dad->left_node));
                        if (log != nullptr) log->log("right mini rotate", logger::severity::debug);
                        if (way_to_splay->empty()){
                            this->right_mini_rotate(root);
                            if (log != nullptr) log->log("right mini rotate", logger::severity::debug);
                        }else{
                            if (way_to_splay->top()->left_node == grand_dad){
                                this->right_mini_rotate(&(way_to_splay->top()->left_node));
                                if (log != nullptr) log->log("right mini rotate", logger::severity::debug);
                            }else{
                                this->right_mini_rotate(&(way_to_splay->top()->right_node));
                                if (log != nullptr) log->log("right mini rotate", logger::severity::debug);
                            }
                        }
                    }
                } else{ //grand->right == dad
                    if (dad->left_node == cur_node){
                        this->right_mini_rotate(&(grand_dad->right_node));
                        if (log != nullptr) log->log("right mini rotate", logger::severity::debug);
                        if (way_to_splay->empty()){
                            this->left_mini_rotate(root);
                            if (log != nullptr) log->log("left mini rotate", logger::severity::debug);
                        }else{
                            if (way_to_splay->top()->left_node == grand_dad){
                                this->left_mini_rotate(&(way_to_splay->top()->left_node));
                                if (log != nullptr) log->log("left mini rotate", logger::severity::debug);
                            }else{
                                this->left_mini_rotate(&(way_to_splay->top()->right_node));
                                if (log != nullptr) log->log("left mini rotate", logger::severity::debug);
                            }
                        }
                    }else{
                        this->left_mini_rotate(&(grand_dad->right_node));
                        if (log != nullptr) log->log("left mini rotate", logger::severity::debug);
                        if (way_to_splay->empty()){
                            this->left_mini_rotate(root);
                            if (log != nullptr) log->log("left mini rotate", logger::severity::debug);
                        }else{
                            if (way_to_splay->top()->left_node == grand_dad){
                                this->left_mini_rotate(&(way_to_splay->top()->left_node));
                                if (log != nullptr) log->log("left mini rotate", logger::severity::debug);
                            }else{
                                this->left_mini_rotate(&(way_to_splay->top()->right_node));
                                if (log != nullptr) log->log("left mini rotate", logger::severity::debug);
                            }
                        }
                    }
                }

            }
        }
        if (log != nullptr) log->log("Splay has finished working", logger::severity::debug);

    }
    //end splay
    class splay_class_insert final : public binary_search_tree<tkey, tvalue, tkey_comparer>::insert_class{
    private:
        splay_tree<tkey, tvalue, tkey_comparer>* _tree;

    public:
        explicit splay_class_insert(splay_tree<tkey, tvalue, tkey_comparer>* tree) : binary_search_tree<tkey, tvalue, tkey_comparer>::insert_class(tree){}

        void insert_after(const tkey &key, const tvalue& value, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** current_node, std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *>* way_to_insert, logger* logger) override{
            _tree->splay(current_node, way_to_insert,  logger);
        }
    };

    class splay_class_find final : public binary_search_tree<tkey, tvalue, tkey_comparer>::find_class{
    private:
        splay_tree<tkey, tvalue, tkey_comparer>* _tree;

    public:
        explicit splay_class_find(splay_tree<tkey, tvalue, tkey_comparer>* tree) : binary_search_tree<tkey, tvalue, tkey_comparer>::find_class(tree){}

        void after_find(typename associative_container<tkey, tvalue>::key_value_pair* key_value_to_find, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** root, logger* log, std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* way_to_find) override{
            _tree->splay(root, way_to_find,  log);
        }
    };

    class splay_class_remove final : public binary_search_tree<tkey, tvalue, tkey_comparer>::remove_class{
    private:
        splay_tree<tkey, tvalue, tkey_comparer>* _tree;

    public:
        explicit splay_class_remove(splay_tree<tkey, tvalue, tkey_comparer>* tree) : binary_search_tree<tkey, tvalue, tkey_comparer>::remove_class(tree){}

        tvalue remove_inner(const tkey &key, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** current_node, std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* way_to_remove, memory* allocator, logger* logger, size_t& side, size_t& additional) override {
            tkey_comparer comparator = tkey_comparer();
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *cur_node = *current_node;
            if (*current_node == nullptr) throw std::logic_error("Tree is empty!");
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *left_subtree = cur_node->left_node;
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *right_subtree = cur_node->right_node;


            if (comparator(key, cur_node->key) == 0) {
                tvalue removing_value = cur_node->value;
                cur_node->~node();
                allocator->deallocate(cur_node);
                cur_node = nullptr;
                *current_node = _tree->merge(left_subtree, right_subtree, logger);
                //_tree->_root = _tree->merge(left_subtree, right_subtree, logger);
                return removing_value;
            }
            way_to_remove->push(cur_node);
            while ((cur_node->right_node != nullptr) && (comparator(cur_node->key, key) < 0) ||
                   (cur_node->left_node != nullptr) && (comparator(cur_node->key, key) > 0)) {
                if (comparator(cur_node->key, key) > 0) {
                    cur_node = cur_node->left_node;
                    way_to_remove->push(cur_node);
                } else {
                    cur_node = cur_node->right_node;
                    way_to_remove->push(cur_node);
                }
                if (comparator(key, cur_node->key) == 0) {
                    _tree->splay(current_node, way_to_remove, logger);
                    cur_node = *current_node;
                    left_subtree = cur_node->left_node;
                    right_subtree = cur_node->right_node;
                    tvalue removing_value = cur_node->value;
//                    _tree->debug_print(cur_node);
//                    _tree->debug_print(left_subtree);
//                    _tree->debug_print(right_subtree);
                    cur_node->~node();
                    allocator->deallocate(cur_node);
                    cur_node = nullptr;
                    *current_node = _tree->merge(left_subtree, right_subtree, logger);
                    //_tree->_root = _tree->merge(left_subtree, right_subtree, logger);
                    return removing_value;

                }
            }
            throw std::logic_error("No Key for remove");
        }
        void remove_after(const tkey & key, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** current_node, logger* logger, std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*> *way_to_remove, size_t& side, size_t& dop) override{}
    };
public:
    typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* merge(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* left_tree, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* right_tree, logger* log){
        if (left_tree == nullptr && right_tree != nullptr) return right_tree;
        if (right_tree == nullptr && left_tree != nullptr) return left_tree;
        if (left_tree != nullptr && right_tree != nullptr){
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* current_node = left_tree;
            auto* way_to_merge = new std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>();
            while (current_node->right_node != nullptr){
                way_to_merge->push(current_node);
                current_node = current_node->right_node;
            }
            way_to_merge->push(current_node);
            splay(&left_tree, way_to_merge, log);
            left_tree->right_node = right_tree;
            return left_tree;
        }
        return nullptr;
    }

public:
    explicit splay_tree<tkey, tvalue, tkey_comparer>(logger* log = nullptr, memory* alloc = nullptr) : binary_search_tree<tkey, tvalue, tkey_comparer>(alloc, log, nullptr, new splay_class_find(this), new splay_class_insert(this), new splay_class_remove(this)){}

    splay_tree<tkey, tvalue, tkey_comparer>(const splay_tree<tkey, tvalue, tkey_comparer>& other){
        this->_logger = other._logger;
        this->_allocator = other._allocator;
        this->_class_find = new splay_class_find(this);
        this->_class_insert = new splay_class_insert(this);
        this->_class_remove = new splay_class_remove(this);
        this->_root = other.copy();
    }

    splay_tree<tkey, tvalue, tkey_comparer>& operator=(const splay_tree<tkey, tvalue, tkey_comparer>& other){
        if (*this != other){
            this->clear();
            this->_root = other.copy();
            return *this;
        }
        return *this;
    }

    splay_tree<tkey, tvalue, tkey_comparer>(const splay_tree<tkey, tvalue, tkey_comparer>&& other) noexcept {
        this->_logger = other._logger;
        this->_allocator = other._allocator;
        this->_class_find = new splay_class_find(this);
        this->_class_insert = new splay_class_insert(this);
        this->_class_remove = new splay_class_remove(this);
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

    splay_tree<tkey, tvalue, tkey_comparer>& operator=(const splay_tree<tkey, tvalue, tkey_comparer>&& other){
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

    ~splay_tree(){
        if (this->_logger != nullptr) this->_logger->log("Splay tree was deleted", logger::severity::debug);
    };
};


#endif //NEWCOURSEWORK_SPLAY_TREE_H
