#ifndef COURSEWORK_BINARY_SEARCH_TREE_H
#define COURSEWORK_BINARY_SEARCH_TREE_H
#include <stack>
#include "../allocators/allocator_2.h"
#include "associative_container.h"
#include <functional>
#include <vector>
#include <ctime>
#include "print_bst.h"

#include <limits>


template<typename  tkey, typename  tvalue, typename tkey_comparer>
class binary_search_tree : public associative_container<tkey, tvalue> {
protected:
    struct node{
        node() = default;
        node(tkey const &k, tvalue const &v, node* l, node* r):
                key(k), value(v), left_node(l), right_node(r){}
        tkey key;
        tvalue value;
        node* left_node;
        node* right_node;
        virtual ~node() = default;
    };

    tkey_comparer _comparator;
    node* _root;
    memory *_allocator;
    logger *_logger;

protected:
    void left_mini_rotate(node** current_node) const{
        node* right_subtree = (*current_node)->right_node;
        (*current_node)->right_node = right_subtree->left_node;
        right_subtree->left_node = (*current_node);
        *current_node = right_subtree;
    }

    void right_mini_rotate(node** current_node) const{
        node* left_subtree = (*current_node)->left_node;
        (*current_node)->left_node = left_subtree->right_node;
        left_subtree->right_node = (*current_node);
        *current_node = left_subtree;
    }
protected:

//    virtual void debug_print(void* root) const {
//        debug_tree_printing<tkey, tvalue>(root);
//    }

public:
    void bypass_tree(typename associative_container<tkey, tvalue>::bypass_detour detour ) const override {
//        if (detour == associative_container<tkey, tvalue>::bypass_detour::prefix) {
//
//            for (auto it = prefix_it_begin(); it != prefix_it_end(); ++it) {
//
//                std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
//            }
//        } else if (detour == associative_container<tkey, tvalue>::bypass_detour::postfix) {
//
//            for (auto it = postfix_it_begin(); it != postfix_it_end(); it++) {
//
//
//                std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
//            }
//
//        } else {
//            for (auto it = infix_it_begin(); it != infix_it_end(); it++) {
//
//                std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
//            }
//        }
//        debug_print(_root);
    }


private:

    /////////////class prefix//////////////////////

    class prefix_iterator final{
    private:
        node* _current_node;
        binary_search_tree<tkey, tvalue, tkey_comparer>* _tree;
        std::stack<node*> _way;
        node* _rightest_node;
    public:
        prefix_iterator(node* current_node, binary_search_tree<tkey, tvalue, tkey_comparer>* tree) {
            _tree = tree;
            node* rightest_node = _tree->_root;
            if (rightest_node != nullptr) {
                while (rightest_node->right_node != nullptr) {
                    rightest_node = rightest_node->right_node;
                }
            }
            _rightest_node=rightest_node;
            if (current_node != nullptr) {
                _current_node = current_node;
                node *ptr = _tree->_root;
                while (ptr != current_node) {
                    _way.push(ptr);
                    if (_tree->_comparator(current_node->key, ptr->key) > 0) {
                        ptr = ptr->right_node;
                    } else {
                        ptr = ptr->left_node;
                    }
                }
            } else {
                _current_node = nullptr;
                _way = std::stack<node *>();
            }
        }
    public:
        bool operator==(prefix_iterator const &other) const{
            if (_way==other._way && _current_node==other._current_node){
                return true;
            }
            return false;

        }

        bool operator!=(prefix_iterator const &other) const{
            return !(*this==other);
        }

        prefix_iterator& operator++(){
            if(_current_node ==_rightest_node ){
                _current_node= nullptr;
                _way=std::stack<node*>();
                return *this;

            }
            if(_current_node->left_node!= nullptr) {
                _way.push(_current_node);
                _current_node = _current_node->left_node;
                return *this;
            }
            if ( _current_node->left_node== nullptr && _current_node->right_node!= nullptr){
                _way.push(_current_node);
                _current_node = _current_node->right_node;
                return *this;
            }

            if (_current_node->left_node== nullptr && _current_node->right_node== nullptr) {
                while(!_way.empty() && (_way.top()->right_node == _current_node || _way.top()->right_node == nullptr)){
                    while (!_way.empty() && _way.top()->right_node == _current_node) {
                        _current_node = _way.top();
                        _way.pop();
                    }

                    while (!_way.empty() && _way.top()->right_node == nullptr) {
                        _current_node = _way.top();
                        _way.pop();
                    }
                }
                _current_node=_way.top()->right_node;
            }
            return *this;

        }

        prefix_iterator operator++(int bred){
            prefix_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const{
            return std::tuple<unsigned int, tkey const&, tvalue const&>(_way.size(),_current_node->key, _current_node->value);
        }
    };

    /////////////class infix//////////////////////

    class infix_iterator final {
    private:
        node* _current_node;
        binary_search_tree<tkey, tvalue, tkey_comparer>* _tree;
        node* _rightest_node;
        std::stack<node*> _way;
    public:
        infix_iterator(node* current_node, binary_search_tree<tkey, tvalue, tkey_comparer>* tree){
            _tree=tree;
            node* rightest_node=tree->_root;
            if (rightest_node!= nullptr){
                while(rightest_node->right_node!= nullptr){
                    rightest_node=rightest_node->right_node;
                }
            }
            _rightest_node=rightest_node;

            if (current_node!= nullptr){
                node* ptr=_tree->_root;
                tkey_comparer functor;
                _current_node=current_node;
                while (ptr!=current_node){
                    _way.push(ptr);
                    if (_tree->_comparator(current_node->key, ptr->key)>0){
                        ptr=ptr->right_node;
                    }else{
                        ptr=ptr->left_node;
                    }
                }
            }else{

                _current_node= nullptr;
                _way=std::stack<node*>();
            }
        }
        bool operator==(infix_iterator const& other) const{
            if (_way==other._way && _current_node==other._current_node){
                return true;
            }else{
                return false;
            }
        }
        bool operator!=(infix_iterator const& other) const{
            return !(*this==other);
        }
        infix_iterator& operator++(){
            if (_current_node == _rightest_node){
                _current_node= nullptr;
                _way=std::stack<node*>();
                return *this;
            }
            if (_current_node->right_node!= nullptr){
                _way.push(_current_node);
                _current_node=_current_node->right_node;
                while(_current_node->left_node!= nullptr){
                    _way.push(_current_node);
                    _current_node=_current_node->left_node;
                }
            }else {
                if (!_way.empty()) {
                    if (_way.top()->left_node == _current_node) {
                        _current_node = _way.top();
                        _way.pop();
                    } else {
                        while (!_way.empty() && _way.top()->right_node == _current_node) {
                            _current_node = _way.top();
                            _way.pop();
                        }
                        _current_node = _way.top();
                        _way.pop();
                    }
                }
            }
            return *this;
        }
        infix_iterator operator++(int bred){
            infix_iterator current=*this;
            ++(*this);
            return current;
        }

        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const{
            return std::tuple<unsigned int, tkey const&, tvalue const&>(_way.size(), _current_node->key, _current_node->value);
        }
    };

    /////////////class postfix//////////////////////

    class postfix_iterator final{
        friend class binary_search_tree<tkey, tvalue, tkey_comparer>;
    private:
        node* _current_node;
        node* _root_node;
        std::stack<node *> _way;
        binary_search_tree<tkey, tvalue, tkey_comparer>* _tree;
    public:
        postfix_iterator(node* current_node, binary_search_tree<tkey, tvalue, tkey_comparer>* tree){
            _tree=tree;
            _root_node=_tree->_root;
            if (current_node!= nullptr){
                node* ptr=_tree->_root;
                _current_node=current_node;
                while(ptr!= current_node){
                    _way.push(ptr);
                    if (tree->_comparator(current_node->key, ptr->key)>0){

                        ptr=ptr->right_node;
                    }else{

                        ptr=ptr->left_node;
                    }
                }
            }else{
                _current_node = nullptr;
                _way=std::stack<node*>();
            }

        }
        bool operator==(postfix_iterator const& other){
            if(this->_way==other._way && this->_current_node==other._current_node){
                return true;
            }else{
                return false;
            }
        }
        bool operator!=(postfix_iterator const& other){
            return !(this->operator==(other));
        }
        postfix_iterator& operator++(){
            if (_current_node== nullptr){
                return *this;
            }
            if(_way.empty()){
                _current_node= nullptr;
                return *this;
            }
            if (_way.top()->right_node==_current_node){
                _current_node=_way.top();
                _way.pop();
            }else{
                if(_way.top()->right_node!= nullptr){
                    _current_node=_way.top()->right_node;
                    while(_current_node->right_node!= nullptr || _current_node->left_node!= nullptr){
                        _way.push(_current_node);
                        if(_current_node->left_node!=nullptr){
                            _current_node=_current_node->left_node;
                        }else{
                            _current_node=_current_node->right_node;
                        }
                    }
                }else{
                    _current_node=_way.top();
                    _way.pop();
                }
            }
            return *this;
        }
        postfix_iterator operator++(int bred){
            postfix_iterator other=*this;
            this->operator++();
            return other;
        }
        std::tuple<unsigned int, tkey, tvalue> operator*(){
            return std::tuple<unsigned int, tkey, tvalue>(_way.size(), this->_current_node->key, this->_current_node->value);
        }

        node* get_node(){
            return _current_node;
        }
    };

public:
    /////////////begin/end prefix//////////////////////

    prefix_iterator prefix_it_begin()const noexcept{
        return prefix_iterator(_root, const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this));
    }

    prefix_iterator prefix_it_end()const noexcept{
        return prefix_iterator(nullptr, const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this));
    }

    /////////////begin/end infix//////////////////////

    infix_iterator infix_it_begin()const noexcept{
        node* ptr=_root;
        if(ptr!= nullptr){
            while (ptr->left_node!= nullptr){
                ptr=ptr->left_node;
            }
        }
        return infix_iterator(ptr, const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this));
    }

    infix_iterator infix_it_end()const noexcept{
        return infix_iterator(nullptr, const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this));
    }

    /////////////begin/end postfix//////////////////////


public:
    postfix_iterator postfix_it_begin()const {
        node* ptr = _root;
        if (_root == nullptr){
            return postfix_it_end();
        }
        if(_root->left_node != nullptr){
            while(ptr->left_node != nullptr ){
                ptr=ptr->left_node;
            }
        }else{
            while(ptr->left_node== nullptr && ptr->right_node != nullptr){
                ptr = ptr->right_node;
            }
            while(ptr->left_node!=nullptr){
                ptr = ptr->left_node;
            }
        }

        return postfix_iterator(ptr, const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this));
    }
    postfix_iterator postfix_it_end()const noexcept{
        return postfix_iterator(nullptr, const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this));
    }



    /////////////class find_class//////////////////////
protected:
    class find_class{
    private:
        binary_search_tree<tkey, tvalue, tkey_comparer>* _tree;
    public:
        explicit find_class(binary_search_tree<tkey, tvalue, tkey_comparer>* tree){
            _tree=tree;
        }
    public:
        bool find_inner(typename associative_container<tkey, tvalue>::key_value_pair* key_value_to_find, node** root, logger* log, std::stack<node*>* way_to_find) {
            if ((*root) == nullptr) return false;
            tkey_comparer comparator = tkey_comparer();
            if (comparator(key_value_to_find->_key, (*root)->key) == 0 && key_value_to_find->_value == (*root)->value) {
                return true;
            }
            node *current_node = *root;
            way_to_find->push(*root);
            while ((current_node->right_node != nullptr) && (comparator(current_node->key, key_value_to_find->_key) < 0) || (current_node->left_node != nullptr) && (comparator(current_node->key, key_value_to_find->_key) > 0)) {
                if (comparator(current_node->key, key_value_to_find->_key) > 0) {
                    current_node = current_node->left_node;
                    way_to_find->push(current_node);
                } else {
                    current_node = current_node->right_node;
                    way_to_find->push(current_node);
                }
                if (comparator(key_value_to_find->_key, current_node->key) == 0 &&
                    key_value_to_find->_value == current_node->value)
                    return true;
            }
            return false;
        }
        virtual void after_find(typename associative_container<tkey, tvalue>::key_value_pair* key_value_to_find, node** root, logger* log, std::stack<node*>* way_to_find){

        }
        virtual ~find_class() = default;
    };
//    class find_class{
//    public:
//        virtual void after_find(typename associative_container<tkey, tvalue>::key_value_pair* key_value_to_find, node*& current_node, std::stack<node**> way_to_find_node){
//
//        }

//        bool find_inner(typename associative_container<tkey, tvalue>::key_value_pair* key_value_to_find, node*& current_node, std::stack<node**> way_to_find_node){
//            if(current_node == nullptr){
//                return false;
//            }
//            tkey_comparer comparer;
//            int res=comparer(current_node->key, key_value_to_find->_key);
//            bool res_find=false;
//            if(res == 0){
//                return true;
//            }
//            if (current_node->right_node== nullptr && current_node->left_node== nullptr){
//                return false;
//            }
//            node* ptr=current_node;
//            if (res>0){
//                ptr=ptr->left_node;
//            }else{
//                ptr=ptr->right_node;
//            }
//            way_to_find_node.push(&ptr);
//            res_find=find_inner(key_value_to_find,  ptr, way_to_find_node);
//            way_to_find_node.pop();
//            after_find(key_value_to_find,  current_node, way_to_find_node);
//            return res_find;
//
//
//        }
//
//    public:
//        bool find(typename associative_container<tkey, tvalue>::key_value_pair* key_value_to_find, node*& current_node){
//            std::stack<node**> way_to_find_node;
//            return find_inner(key_value_to_find,  current_node, way_to_find_node);
//        }
//
//        virtual ~find_class() noexcept = default;
//    };




protected:

    ////////////////////class insert_class//////////////////////////

    class insert_class {
    private:
        binary_search_tree<tkey, tvalue, tkey_comparer>* _tree;
    public:
        explicit insert_class(binary_search_tree<tkey, tvalue, tkey_comparer>* tree){
            _tree=tree;
        }
    protected:
        virtual size_t get_size_of_node() const{
            return reinterpret_cast<size_t>(sizeof(node));
        }

        virtual void get_node_constructor(node** x, const tkey& key, const tvalue& value) const {
            new (*x) node{key, value, nullptr, nullptr};
        }

    public:
        void insert_inner(const tkey& key, const tvalue& value,node** current_node, std::stack<node*> *way_to_insert, memory* allocator, logger* logger){
            node* current_node1 = *current_node;
            int f;
            tkey_comparer comparator = tkey_comparer();
            if (*current_node == nullptr){
                *current_node = reinterpret_cast<node*>(allocator->allocate(get_size_of_node()));
                if (*current_node == nullptr) throw std::logic_error("Too few memory...");
                get_node_constructor(current_node, key, value);
                way_to_insert->push(*current_node);
                if (logger != nullptr){
                    logger->log("NEW NODE ADDED ", logger::severity::debug);
                }
                return;

            }

            while (current_node1 != nullptr){
                way_to_insert->push(current_node1);
                int comparer_res = comparator(key, current_node1->key);
                if (comparer_res == 0){
                    throw std::logic_error("SAME KEYS");
                }
                else if(comparer_res > 0){
                    f = 1;
                    current_node1 = current_node1->right_node;
                }
                else{
                    f = 0;
                    current_node1 = current_node1->left_node;
                }
            }

            node **tmp;
            if (f == 1){
                tmp = &way_to_insert->top()->right_node;
            }
            if(f == 0){
                tmp = &way_to_insert->top()->left_node;
            }
            (*tmp) = reinterpret_cast<node*>(allocator->allocate(get_size_of_node()));
            if (*tmp == nullptr) throw std::logic_error("Too few memory...");
            get_node_constructor(tmp, key, value);
            way_to_insert->push(*tmp);
            if (logger!=nullptr){
                logger->log("NEW NODE ADDED ", logger::severity::debug);
            }

        }
    public:
        virtual void insert_after(const tkey &key, const tvalue& value, node** current_node, std::stack<node *>* way_to_insert, logger* logger){
            while(way_to_insert->empty()) way_to_insert->pop();
        }
    public:
        virtual ~insert_class() = default;
    };


    ////////////////////class remove_class//////////////////////////

protected:
    class remove_class{
    private:
        binary_search_tree<tkey, tvalue, tkey_comparer>* _tree;
    public:
        explicit remove_class(binary_search_tree<tkey, tvalue, tkey_comparer> *tree){
            _tree = tree;
        }

    protected:
        virtual void destruct_node(node* node_to_destruct) const{
            node_to_destruct->~node();
        }
        virtual void additional_work(node* current, node* dad, size_t& side, size_t& additional) const {}

    public:
        virtual tvalue remove_inner(const tkey &key, node** current_node, std::stack<node*>* way_to_remove, memory* allocator, logger* logger, size_t& side, size_t& additional){
            node* current_node1 = *current_node;
            int f = 2;
            tkey_comparer comparator = tkey_comparer();

            while(current_node1 != nullptr && (comparator(key,current_node1->key) != 0)){
                way_to_remove->push(current_node1);
                if (comparator(key,current_node1->key) > 0){
                    current_node1 = current_node1->right_node;
                    f = 1;
                }
                else{
                    current_node1 = current_node1->left_node;
                    f = 0;
                }
            }
            if (current_node1 == nullptr){
                throw std::logic_error("No such key in the tree ");
            }

            if (current_node1->left_node == nullptr && current_node1->right_node == nullptr){
                if (f != 2){
                    additional_work(current_node1, way_to_remove->top(), side, additional);
                }
                if (f == 1){
                    way_to_remove->top()->right_node = nullptr;
                }
                if (f == 0){
                    way_to_remove->top()->left_node = nullptr;
                }

                tvalue val_to_remove = std::move(current_node1->value);
                destruct_node(current_node1);
                allocator->deallocate(reinterpret_cast<void*>(current_node1));

                if (f == 2){
                    *current_node = nullptr;
                }
                if (logger != nullptr){
                    logger->log("Node was deleted ", logger::severity::debug);
                }
                return val_to_remove;
            }

            else if ((current_node1->left_node != nullptr && current_node1->right_node == nullptr) || (current_node1->left_node == nullptr && current_node1->right_node != nullptr)){

                if (!way_to_remove->empty()){
                    additional_work(current_node1, way_to_remove->top(), side, additional);
                }

                if (current_node1->left_node == nullptr && current_node1->right_node != nullptr){
                    if(current_node1 == *current_node){
                        *current_node = current_node1->left_node;
                    }
                    else f == 0 ? way_to_remove->top()->left_node = current_node1->left_node : way_to_remove->top()->right_node = current_node1->left_node;
                }else{
                    if(current_node1 == *current_node){
                        *current_node = current_node1->right_node;
                    }
                    else f == 0 ? way_to_remove->top()->left_node = current_node1->right_node : way_to_remove->top()->right_node = current_node1->right_node;
                }
                tvalue val_to_remove = std::move(current_node1->value);
                destruct_node(current_node1);
                allocator->deallocate(reinterpret_cast<void*>(current_node1));

                if (logger != nullptr){
                    logger->log("Node was deleted ", logger::severity::debug);
                }
                return val_to_remove;

            }

            else if(current_node1->left_node != nullptr && current_node1->right_node != nullptr){
                node* tmp_node = current_node1;
                way_to_remove->push(current_node1);
                tmp_node = tmp_node->right_node;
                if (tmp_node->left_node == nullptr){
                    additional_work(tmp_node, current_node1,side, additional);

                    tvalue val_to_remove = std::move(current_node1->value);
                    current_node1->key = tmp_node->key;
                    current_node1->value = tmp_node->value;
                    current_node1->right_node = tmp_node->right_node;

                    destruct_node(tmp_node);
                    allocator->deallocate(reinterpret_cast<void*>(tmp_node));

                    if (logger != nullptr){
                        logger->log("Node was deleted ", logger::severity::debug);
                    }
                    return val_to_remove;

                }else{
                    while (tmp_node->left_node != nullptr){
                        way_to_remove->push(tmp_node);
                        tmp_node = tmp_node->left_node;
                    }
                    additional_work(tmp_node, way_to_remove->top(), side, additional);

                    tvalue val_to_remove = std::move(current_node1->value);
                    current_node1->key = tmp_node->key;
                    current_node1->value = tmp_node->value;
                    way_to_remove->top()->left_node = tmp_node->right_node;

                    destruct_node(current_node1);
                    allocator->deallocate(reinterpret_cast<void*>(tmp_node));


                    if (logger != nullptr){
                        logger->log("Node was deleted ", logger::severity::debug);
                    }
                    return val_to_remove;
                }

            }
            throw std::logic_error("Node wasn't deleted");
        }

        virtual void remove_after(const tkey & key, node** current_node, logger* logger, std::stack<node*> *way_to_remove, size_t& side, size_t& dop){
            while (!way_to_remove->empty()){
                way_to_remove->pop();
            }
        }
    public:
        virtual ~remove_class() = default;

    };




protected:
    find_class* _class_find;
    insert_class* _class_insert;
    remove_class* _class_remove;

public:
    bool find_key(const tkey& key_to_find){
        if ((_root) == nullptr) return false;
        tkey_comparer comparator = tkey_comparer();
        if (comparator(key_to_find, (_root)->key) == 0) {
            return true;
        }
        node *current_node = _root;
        while ((current_node->right_node != nullptr) && (comparator(current_node->key, key_to_find) < 0) || (current_node->left_node != nullptr) && (comparator(current_node->key, key_to_find) > 0)) {
            if (comparator(current_node->key, key_to_find) > 0) {
                current_node = current_node->left_node;
            } else {
                current_node = current_node->right_node;
            }
            if (comparator(key_to_find, current_node->key) == 0)
                return true;
        }
        return false;
    }

    tkey& find_ref_key(const tkey &key_to_find) const{
        if ((_root) == nullptr) {
            throw std::logic_error("Empty");
        }
        tkey_comparer comparator = tkey_comparer();
        if (comparator(key_to_find, (_root)->key) == 0) {
            return _root->key;
        }
        node *current_node = _root;
        while ((current_node->right_node != nullptr) && (comparator(current_node->key, key_to_find) < 0) || (current_node->left_node != nullptr) && (comparator(current_node->key, key_to_find) > 0)) {
            if (comparator(current_node->key, key_to_find) > 0) {
                current_node = current_node->left_node;
            } else {
                current_node = current_node->right_node;
            }
            if (comparator(key_to_find, current_node->key) == 0)
                return current_node->key;
        }
        throw std::logic_error("There is no key");
    }

    void update_key(const tkey& key_to_update, const tvalue& new_value) {
        if ((_root) == nullptr) {
            throw std::logic_error("Empty");
        }
        tkey_comparer comparator = tkey_comparer();
        if (comparator(key_to_update, (_root)->key) == 0) {
            _root->value = new_value;
        }
        node *current_node = _root;
        while ((current_node->right_node != nullptr) && (comparator(current_node->key, key_to_update) < 0) || (current_node->left_node != nullptr) && (comparator(current_node->key, key_to_update) > 0)) {
            if (comparator(current_node->key, key_to_update) > 0) {
                current_node = current_node->left_node;
            } else {
                current_node = current_node->right_node;
            }
            if (comparator(key_to_update, current_node->key) == 0)
                current_node->value = new_value;
        }
    }



public:
//    bool find(typename associative_container<tkey, tvalue>::key_value_pair* key_value_to_find)override{
//        return _class_find->find(key_value_to_find,_root);
//    }

    bool find(typename associative_container<tkey, tvalue>::key_value_pair* key_value_to_find) override{
         std::stack<node*> find_way = std::stack<node*>();
         bool result = _class_find->find_inner(key_value_to_find, &_root, this->_logger, &find_way);
        _class_find->after_find(key_value_to_find, &_root, this->_logger, &find_way);
         return result;
    }

    void insert(const tkey &key, const tvalue& value)override{
        std::stack<node*> insert_way = std::stack<node*>();
        node** current_root = &_root;
        _class_insert->insert_inner(key, value, current_root, &insert_way, this->_allocator, this->_logger);
        _class_insert->insert_after(key, value, current_root, &insert_way, this->_logger);
    }

    tvalue remove(tkey const &key)override{
        if(_root == nullptr){
            throw std::logic_error("Tree is empty!");
        }
        size_t side;
        size_t dop;
        node** current_root = &_root;
        std::stack<node*> way_to_remove = std::stack<node*>();
        tvalue val_to_remove = _class_remove->remove_inner(key, current_root, &way_to_remove, _allocator, _logger,side, dop);
        return val_to_remove;
    }

public:
    const tvalue& get(const tkey &key) const override {
        node* current = _root;
        if(current == nullptr){
            throw std::logic_error("Tree is empty!");
        }
        tkey_comparer comparer = tkey_comparer();
        if(comparer(key, current->key) == 0) return current->value;
        while((current->left_node != nullptr && comparer(key, current->key) < 0) || (current->right_node != nullptr && comparer(key, current->key) > 0)){
            if(comparer(key, current->key) > 0) current = current->right_node;
            else current = current->left_node;
            if(comparer(key, current->key) == 0) return current->value;
        }
        throw std::logic_error("Key not found!");
    }

    //void bypass_tree(typename associative_container<tkey,tvalue>::bypass_detour mode) const override{}



protected:
    binary_search_tree(memory* alloc_tree, logger* log_tree, insert_class* insert):
            _root(nullptr), _logger(log_tree), _allocator(alloc_tree), _class_insert(insert)
    {
        _class_find = new find_class(this);
        _class_remove = new remove_class(this);

    }
protected:
    void clear(){
        clear_inner(_root);

    }

    void clear_inner(node* current_node){
        if (current_node == nullptr){
            return;
        }

        auto* left = current_node->left_node;
        auto* right = current_node->right_node;
        current_node->~node();
        _allocator->deallocate(current_node);
        clear_inner(left);
        clear_inner(right);

    }

    node* copy() const{
        copy_inner(_root);

    }

    node* copy_inner(node* current_node) const{
        if (current_node == nullptr){
            return nullptr;
        }
        auto* new_node = reinterpret_cast<node*>(_allocator->allocate(node_size()));
        node_construct(new_node, current_node);
        additional_data(new_node, current_node);
        throw std::logic_error("copy_inner failed");
    }

    virtual size_t node_size() const{
        return sizeof(node);
    }

    virtual void node_construct(node* current_node_copy, node* current_node) const{
        new (current_node_copy) node {current_node->key, current_node->value, copy_inner(current_node->left_node),copy_inner(current_node->right_node)};
    }

    virtual void additional_data(node* current_node_copy, node* current_node) const{}

public:
    /////////////tree constructor//////////////////
    binary_search_tree(memory* alloc_tree, logger* log_tree){
        _root = nullptr;
        _logger = log_tree;
        _allocator = alloc_tree;
        _class_find = new find_class(this);
        _class_insert = new insert_class(this);
        _class_remove = new remove_class(this);

    }

    /////////////tree constructor2//////////////////
    binary_search_tree(memory* alloc_tree, logger* log_tree, node* root, find_class* find, insert_class* insert, remove_class* remove){
        _root = root;
        _logger = log_tree;
        _allocator = alloc_tree;
        _class_find = find;
        _class_insert = insert;
        _class_remove = remove;

    }

    /////////////tree destructor//////////////////
    ~binary_search_tree(){

        auto end_it=postfix_it_end();
        for (auto it=postfix_it_begin(); it != end_it; it++){
            _allocator->deallocate(it.get_node());
        }
        delete _class_find;
        delete _class_insert;
        delete _class_remove;

    }


    /////////////tree copy constructor//////////////////
    void copy_nodes(node* node){
        if (node != nullptr){
            _class_insert->insert(node->key, node->value);
            copy_nodes(node->left_node);
            copy_nodes(node->right_node);
        }
    }

    binary_search_tree(const binary_search_tree<tkey, tvalue, tkey_comparer> &other_tree){
        _root = nullptr;
        _allocator = other_tree._allocator;
        _logger = other_tree._logger;
        _comparator = other_tree._comparator;
        _class_find = other_tree._class_find;
        _class_insert = other_tree._class_insert;
        _class_remove = other_tree._class_remove;
        copy_nodes(other_tree._root);
    }
    binary_search_tree<tkey, tvalue, tkey_comparer>& operator=(const binary_search_tree<tkey, tvalue, tkey_comparer> &other_tree){
        if(_root!= nullptr){
            postfix_iterator it;
            auto end_it=postfix_it_end();
            for (it=postfix_it_begin(); it != end_it; it++){
                it.get_node()->~get_node();
                _allocator->deallocate(it.get_node());
            }
            _root= nullptr;
            this->copy_nodes(other_tree._root);
            return *this;
        }else{
            this->copy_nodes(other_tree._root);
            return *this;
        }
    }
    binary_search_tree(binary_search_tree<tkey, tvalue, tkey_comparer>&& other_tree){
        _allocator = other_tree._allocator;
        _logger = other_tree._logger;
        _comparator = std::move(other_tree._comparator);
        _class_find = other_tree._class_find;
        _class_insert = other_tree._class_insert;
        _class_remove = other_tree._class_remove;
        other_tree._class_find= nullptr;
        other_tree._class_remove= nullptr;
        other_tree._class_insert= nullptr;
        _root=other_tree._root;
        other_tree._root= nullptr;
    }
    binary_search_tree<tkey, tvalue, tkey_comparer>& operator=(const binary_search_tree<tkey, tvalue, tkey_comparer>&& other_tree){
        if (*this==other_tree){
            return *this;
        }
        _allocator = other_tree._allocator;
        _logger = other_tree._logger;
        _comparator = std::move(other_tree._comparator);
        other_tree._class_insert = nullptr;
        other_tree._class_remove = nullptr;
        other_tree._class_find = nullptr;
        if(_root!= nullptr){
            _root= other_tree._root;
            postfix_iterator it;
            auto end_it=other_tree.postfix_it_end();
            for (it=other_tree.postfix_it_begin(); it != other_tree.end_it; it++){
                it.get_node()->~node();
                _allocator->deallocate(it.get_node());
            }
            return *this;
        }else{

            return *this;
        }
    }
//    template <typename tekey, typename tevalue>
//    struct debug_node{
//        debug_node() = default;
//        debug_node(tekey k, tevalue v, debug_node* l, debug_node* r):
//                key(k), value(v), left(l), right(r){}
//
//        tekey key;
//        tevalue value;
//        debug_node* left;
//        debug_node* right;
//
//        virtual ~debug_node() = default;
//    };

};






#endif //COURSEWORK_BINARY_SEARCH_TREE_H
