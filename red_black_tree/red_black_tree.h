#ifndef COURSEWORK_RED_BLACK_TREE_H
#define COURSEWORK_RED_BLACK_TREE_H

#include "../binary_search_tree/binary_search_tree.h"


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class red_black_tree final: public binary_search_tree<tkey, tvalue, tkey_comparer> {

private:
    enum class color{
        red,
        black
    };

    struct rb_node final: public binary_search_tree<tkey, tvalue, tkey_comparer>::node {
        rb_node() = default;
        rb_node(const tkey& key, const tvalue& value, rb_node* left, rb_node* right, color col):
                binary_search_tree<tkey, tvalue,tkey_comparer>::node(key, value, left, right){
            node_color = col;
        }
        color node_color;
        virtual ~rb_node() = default;
    };

private:
////////////////////class rb_insert_class///////////////////
    class rb_insert_class final: public binary_search_tree<tkey, tvalue, tkey_comparer>::insert_class {
    private:
        red_black_tree<tkey, tvalue, tkey_comparer> *_tree;

        size_t get_size_of_node() const override{
            return sizeof(rb_node);
        };

        void get_node_constructor(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** x, const tkey& key, const tvalue& value) const override {
            new (*x) rb_node {key, value, nullptr, nullptr, color::red};
        }

    public:
        explicit rb_insert_class(red_black_tree<tkey, tvalue, tkey_comparer> *tree):
                binary_search_tree<tkey, tvalue, tkey_comparer>::insert_class(tree){}

    private:
        void insert_fixup(std::stack<rb_node *>* _way_to_insert, rb_node** root){
            rb_node* target_place = _way_to_insert->top();
            _way_to_insert->pop();
            if (_way_to_insert->empty()){
                target_place->node_color = color::black;
            }else{
                if(_way_to_insert->top()->node_color != color::black){
                    rb_node* father = _way_to_insert->top();
                    _way_to_insert->pop();
                    rb_node* uncle;
                    if (_way_to_insert->top()->right_node == father){
                        uncle = reinterpret_cast<rb_node*>(_way_to_insert->top()->left_node);
                    }
                    if (_way_to_insert->top()->left_node == father){
                        uncle = reinterpret_cast<rb_node*>(_way_to_insert->top()->right_node);
                    }

                    //uncle - red
                    if (uncle != nullptr && uncle->node_color == color::red){
                        uncle->node_color = color::black;
                        uncle->node_color = color::black;
                        _way_to_insert->top()->node_color = color::red;
                        insert_fixup(_way_to_insert, root);
                    }
                        //uncle - black
                    else{
                        rb_node* grand = _way_to_insert->top();
                        rb_node* greatgrand;
                        if (_way_to_insert->size() <= 1){
                            greatgrand = nullptr;
                        }else if (_way_to_insert->size() > 1){
                            _way_to_insert->pop();
                            greatgrand = _way_to_insert->top();
                        }

                        // first: father is right or left
                        // first: father is left
                        if (father == grand->left_node){
                            // second: target is right or left
                            // second: target is left
                            if (target_place == father->left_node){
                                if (greatgrand != nullptr){
                                    // third: grand is right or left
                                    // third: grand is left
                                    if (grand == greatgrand->left_node){
                                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgrand->left_node));
                                    }
                                        // third: grand is right
                                    else{
                                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgrand->right_node));
                                    }
                                    //end third

                                }else{//(if greatgrand == nullptr)
                                    _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(root));
                                }

                                father->node_color = color::black;
                                grand->node_color = color::red;

                            }
                                // second: target is right
                            else{
                                _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grand->left_node));

                                if (greatgrand != nullptr){
                                    // third: grand is right or left
                                    // third: grand is left
                                    if (grand == greatgrand->left_node){
                                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgrand->left_node));
                                    }
                                        // third: grand is right
                                    else{
                                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgrand->right_node));
                                    }
                                    //end third

                                }else{//(if greatgrand == nullptr)
                                    _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(root));
                                }

                                target_place->node_color = color::black;
                                grand->node_color = color::red;

                            }
                            //second end


                        }
                            // first: father is right
                        else{
                            // fourth: target is right or left
                            // fourth: target is left
                            if (target_place == father->left_node){
                                _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&grand->right_node));
                                if (greatgrand != nullptr){
                                    // fifth: grand is right or left
                                    // fifth: grand is left
                                    if (grand == greatgrand->left_node){
                                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgrand->left_node));
                                    }
                                        // fifth: grand is right
                                    else{
                                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgrand->right_node));
                                    }
                                    //end fifth

                                }else{//(if greatgrand == nullptr)
                                    _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(root));
                                }

                                target_place->node_color = color::black;
                                grand->node_color = color::red;

                            }
                                // fourth: target is right
                            else{
                                if (greatgrand != nullptr){
                                    // sixth: grand is right or left
                                    // sixth: grand is left
                                    if (grand == greatgrand->left_node){
                                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgrand->left_node));
                                    }
                                        // sixth: grand is right
                                    else{
                                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&greatgrand->right_node));
                                    }
                                    //end sixth

                                }else{//(if greatgrand == nullptr)
                                    _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(root));
                                }

                                father->node_color = color::black;
                                grand->node_color = color::red;

                            }
                            //fourth end



                        }
                    }
                }
            }


        }// end insert_fixup

    public:
        void fixup_root(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *root) override {
            if (root != nullptr) {
                reinterpret_cast<rb_node *>(root)->node_color = color::black;
            }
        }

        void insert_after(const tkey &key,const tvalue &value, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** current_node, std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>* way_to_insert, logger* logger )override{
            auto** rb_current_node = reinterpret_cast<rb_node**>(current_node);
            auto* rb_way_to_insert = reinterpret_cast<std::stack<rb_node*>*>(way_to_insert);
            if (way_to_insert->size() == 1){
                reinterpret_cast<rb_node *>(way_to_insert->top())->node_color = color::black;
                if (logger != nullptr){
                    logger->log("Tree was balanced after inserting.", logger::severity::debug);

                }
            }else{
                reinterpret_cast<rb_node *>(way_to_insert->top())->node_color = color::red;
                insert_fixup(rb_way_to_insert, rb_current_node);
                if (logger != nullptr){
                    logger->log("Tree was balanced after inserting.", logger::severity::debug);
                }
                reinterpret_cast<rb_node *>(*current_node)->node_color = color::black;
            }
        }

    };
    /////////////end rb_insert_class////////////

    /////////////class rb_remove_class////////////

    class rb_remove_class final: public binary_search_tree<tkey, tvalue, tkey_comparer>::remove_class{
    private:
        red_black_tree<tkey, tvalue, tkey_comparer>* _tree;
    public:
        explicit rb_remove_class(red_black_tree<tkey, tvalue, tkey_comparer> *tree): binary_search_tree<tkey, tvalue, tkey_comparer>::remove_class(tree){}
    private:

        void destruct_node(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* x) const override {
            auto* y = reinterpret_cast<rb_node*>(x);
            y->~rb_node();
        }

        void additional_work(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* current, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node* dad, size_t& side, size_t& additional) const override {
            auto* rb_current = reinterpret_cast<rb_node*>(current);
            auto* rb_dad = reinterpret_cast<rb_node*>(dad);
            rb_dad->left_node == rb_current ? side = 0 : side = 1;
            rb_current->node_color == color::red ? additional = 0 : additional = 1;
        }

        void fixup_root(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *root) override {
            if (root != nullptr) {
                reinterpret_cast<rb_node *>(root)->node_color = color::black;
            }
        }

        void remove_fixup(std::stack<rb_node*>* way_to_remove, size_t& side, size_t& dop, rb_node* rb_root){
            rb_node* brother;
            rb_node* father = way_to_remove->top();
            way_to_remove->pop();
            if (side == 0){
                brother = reinterpret_cast<rb_node*>(father->right_node);
            }else{
                brother = reinterpret_cast<rb_node*>(father->left_node);
            }

            if (side == 1){
                //1
                if (father->node_color == color::red && brother->node_color == color::black && (reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::black || brother->left_node == nullptr)  && (reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::black || brother->right_node == nullptr)){
                    father->node_color = color::black;
                    brother->node_color = color::red;
                }
                    //2.1
                else if (father->node_color == color::red && brother->node_color == color::black && brother->left_node != nullptr && reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::red){
                    brother->node_color = color::red;
                    reinterpret_cast<rb_node*>(brother->left_node)->node_color = color::black;
                    father->node_color = color::black;
                    //поворот направо относительно брата
                    if (!way_to_remove->empty()){
                        if(father == way_to_remove->top()->right_node){
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->right_node)));
                        }else{
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->left_node)));
                        }
                    }
                    else{
                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }

                }
                    //2.2
                else if(father->node_color == color::red && brother->node_color == color::black && brother->right_node != nullptr && reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::red){
                    brother->node_color = color::red;
                    reinterpret_cast<rb_node*>(brother->right_node)->node_color = color::black;
                    father->node_color = color::black;

                    _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(father->left_node)));

                    brother = reinterpret_cast<rb_node*>(father->left_node);
                    brother->node_color = color::red;
                    father->node_color = color::black;

                    reinterpret_cast<rb_node*>(brother->left_node)->node_color = color::black;

                    if (!way_to_remove->empty()){
                        if(father == way_to_remove->top()->right_node){
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->right_node)));
                        }else{
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->left_node)));
                        }
                    }
                    else{
                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }

                }
                    //3
                else if(father->node_color == color::black &&
                        brother->node_color == color::red &&
                        brother->right_node != nullptr &&
                        reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::black &&
                        (reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right_node)->left_node)->node_color == color::black || reinterpret_cast<rb_node*>(brother->right_node)->left_node == nullptr) &&
                        (reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right_node)->right_node)->node_color == color::black || reinterpret_cast<rb_node*>(brother->right_node)->right_node == nullptr))
                {
                    reinterpret_cast<rb_node*>(brother->right_node)->node_color= color::red;
                    brother->node_color = color::black;
                    //поворот вправо
                    if(!way_to_remove->empty()){
                        if(father == way_to_remove->top()->right_node){
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->right_node)));
                        }else{
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->left_node)));
                        }
                    }
                    else{
                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }
                    //4
                else if(father->node_color == color::black && brother->node_color == color::red && brother->right_node != nullptr && reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::black &&
                        (brother->right_node)->left_node != nullptr &&
                        reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right_node)->left_node)->node_color == color::red){
                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right_node)->left_node)->node_color = color::black;
                    //большой поворот
                    _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(father->left_node)));

                    if (!way_to_remove->empty()){
                        if(father==way_to_remove->top()->right_node){
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->right_node));
                        }
                        else{
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->left_node));
                        }
                    }else{
                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }

                }
                    //4.2
                else if(father->node_color == color::black && brother->node_color == color::red && brother->right_node != nullptr && reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::black &&
                        (brother->right_node)->right_node != nullptr &&
                        reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right_node)->right_node)->node_color == color::red){
                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right_node)->right_node)->node_color = color::black;
                    reinterpret_cast<rb_node*>(brother->right_node)->node_color = color::red;

                    _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(brother->right_node)));

                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->right_node)->left_node)->node_color = color::black;

                    _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(father->left_node)));

                    if (!way_to_remove->empty()){
                        if(father==way_to_remove->top()->right_node){
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->right_node));
                        }
                        else{
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->left_node));
                        }
                    }else{
                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }

                }
                    //5
                else if(father->node_color == color::black && brother->node_color == color::black && brother->right_node != nullptr && reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::red){
                    //внука в черный и большой поворот влево как из случая 4
                    reinterpret_cast<rb_node*>(brother->right_node)->node_color = color::black;

                    _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(father->left_node)));

                    if (!way_to_remove->empty()){
                        if(father==way_to_remove->top()->right_node){
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->right_node));
                        }
                        else{
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->left_node));
                        }
                    }else{
                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }


                }

                    //5.2
                else if(father->node_color == color::black && brother->node_color == color::black && brother->left_node != nullptr && reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::red){
                    reinterpret_cast<rb_node*>(brother->left_node)->node_color = color::black;

                    if (!way_to_remove->empty()){
                        if(father==way_to_remove->top()->right_node){
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->right_node));
                        }
                        else{
                            _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->left_node));
                        }
                    }else{
                        _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }

                }

                    //6
                else if(father->node_color == color::black && brother->node_color == color::black && (brother->right_node != nullptr || reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::black) && (brother->left_node != nullptr || reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::black))
                {
                    brother->node_color = color::red;

                    if (!way_to_remove->empty()){
                        if (father == way_to_remove->top()->right_node){
                            side = 1;
                        }else{
                            side = 0;
                        }
                        try{
                            remove_fixup(way_to_remove, side, dop, rb_root);
                        }catch(const std::logic_error& e){
                            throw e;

                        }
                    }
                }else{
                    throw std::logic_error("Invalid arguments");
                }
                //end side == 1
            }
            else{
                //1
                if (father->node_color == color::red && brother->node_color == color::black && (reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::black || brother->left_node == nullptr)  && (reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::black || brother->right_node == nullptr)){
                    father->node_color = color::black;
                    brother->node_color = color::red;
                }
                    //2.1
                else if (father->node_color == color::red && brother->node_color == color::black && brother->right_node != nullptr && reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::red){
                    brother->node_color = color::red;
                    reinterpret_cast<rb_node*>(brother->right_node)->node_color = color::black;
                    father->node_color = color::black;
                    //поворот направо относительно брата
                    if (!way_to_remove->empty()){
                        if(father == way_to_remove->top()->right_node){
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->right_node)));
                        }else{
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->left_node)));
                        }
                    }
                    else{
                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }

                }
                    //2.2
                else if(father->node_color == color::red && brother->node_color == color::black && brother->left_node != nullptr && reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::red){
                    brother->node_color = color::red;
                    reinterpret_cast<rb_node*>(brother->left_node)->node_color = color::black;
                    father->node_color = color::black;

                    _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(father->right_node)));

                    brother = reinterpret_cast<rb_node*>(father->right_node);
                    brother->node_color = color::red;
                    father->node_color = color::black;

                    reinterpret_cast<rb_node*>(brother->right_node)->node_color = color::black;

                    if (!way_to_remove->empty()){
                        if(father == way_to_remove->top()->right_node){
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->right_node)));
                        }else{
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->left_node)));
                        }
                    }
                    else{
                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }

                }
                    //3
                else if(father->node_color == color::black &&
                        brother->node_color == color::red &&
                        brother->right_node != nullptr &&
                        reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::black &&
                        (reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left_node)->left_node)->node_color == color::black || reinterpret_cast<rb_node*>(brother->left_node)->left_node == nullptr) &&
                        (reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left_node)->right_node)->node_color == color::black || reinterpret_cast<rb_node*>(brother->left_node)->right_node == nullptr))
                {
                    reinterpret_cast<rb_node*>(brother->left_node)->node_color= color::red;
                    brother->node_color = color::black;
                    //поворот влево
                    if(!way_to_remove->empty()){
                        if(father == way_to_remove->top()->right_node){
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->right_node)));
                        }else{
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(way_to_remove->top()->left_node)));
                        }
                    }
                    else{
                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }
                }
                    //4
                else if(father->node_color == color::black && brother->node_color == color::red && brother->left_node != nullptr && reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::black &&
                        (brother->left_node)->right_node != nullptr &&
                        reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left_node)->right_node)->node_color == color::red){
                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left_node)->right_node)->node_color = color::black;
                    //большой поворот
                    _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(father->right_node)));

                    if (!way_to_remove->empty()){
                        if(father==way_to_remove->top()->right_node){
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->right_node));
                        }
                        else{
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->left_node));
                        }
                    }else{
                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }

                }
                    //4.2
                else if(father->node_color == color::black && brother->node_color == color::red && brother->left_node != nullptr && reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::black &&
                        (brother->left_node)->right_node != nullptr &&
                        reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left_node)->right_node)->node_color == color::red){
                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left_node)->left_node)->node_color = color::black;
                    reinterpret_cast<rb_node*>(brother->left_node)->node_color = color::red;

                    _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(brother->left_node)));

                    reinterpret_cast<rb_node*>(reinterpret_cast<rb_node*>(brother->left_node)->right_node)->node_color = color::black;

                    _tree->right_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(father->right_node)));

                    if (!way_to_remove->empty()){
                        if(father==way_to_remove->top()->right_node){
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->right_node));
                        }
                        else{
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->left_node));
                        }
                    }else{
                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }

                }
                    //5
                else if(father->node_color == color::black && brother->node_color == color::black && brother->left_node != nullptr && reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::red){
                    //внука в черный и большой поворот влево как из случая 4
                    reinterpret_cast<rb_node*>(brother->left_node)->node_color = color::black;

                    _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&(father->right_node)));

                    if (!way_to_remove->empty()){
                        if(father==way_to_remove->top()->right_node){
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->right_node));
                        }
                        else{
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->left_node));
                        }
                    }else{
                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }


                }

                    //5.2
                else if(father->node_color == color::black && brother->node_color == color::black && brother->right_node != nullptr && reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::red){
                    reinterpret_cast<rb_node*>(brother->right_node)->node_color = color::black;

                    if (!way_to_remove->empty()){
                        if(father==way_to_remove->top()->right_node){
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->right_node));
                        }
                        else{
                            _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(&way_to_remove->top()->left_node));
                        }
                    }else{
                        _tree->left_mini_rotate(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**>(rb_root));
                    }

                }

                    //6
                else if(father->node_color == color::black && brother->node_color == color::black && (brother->right_node != nullptr || reinterpret_cast<rb_node*>(brother->right_node)->node_color == color::black) && (brother->left_node != nullptr || reinterpret_cast<rb_node*>(brother->left_node)->node_color == color::black))
                {
                    brother->node_color = color::red;

                    if (!way_to_remove->empty()){
                        if (father == way_to_remove->top()->right_node){
                            side = 1;
                        }else{
                            side = 0;
                        }
                        try{
                            remove_fixup(way_to_remove, side, dop, rb_root);
                        }catch(const std::logic_error& e){
                            throw e;

                        }
                    }
                }else{
                    throw std::logic_error("Invalid arguments");
                }
            }
        }





    public:
        void remove_after(const tkey &key, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node ** current_node, logger* logger, std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>:: node *>* way_to_remove, size_t& side, size_t& dop) override{
            auto** rb_current_node = reinterpret_cast<rb_node**>(current_node);

            auto* rb_way_to_remove = reinterpret_cast<std::stack<rb_node*>*>(way_to_remove);

            if ((*rb_current_node)!= nullptr){
                if ((*rb_current_node)->right_node == nullptr && (*rb_current_node)->left_node == nullptr){
                    (*rb_current_node)->node_color = color::black;
                }
                else{
                    if (dop != 0){
                        if(side == 0){
                            if (reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>(way_to_remove->top())->left_node != nullptr){
                                reinterpret_cast<rb_node*>(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>(way_to_remove->top())->left_node)->node_color = color::black;

                                if (logger != nullptr){
                                    logger->log("Tree was balanced (after removing)", logger::severity::debug);
                                }
                                if (*current_node != nullptr) {
                                    reinterpret_cast<rb_node *>(*current_node)->node_color = color::black;
                                }
                                return;
                            }
                            else
                            {
                                try{
                                    remove_fixup(rb_way_to_remove, side, dop, *rb_current_node);
                                }catch(const std::logic_error& e){
                                    if (logger != nullptr){
                                        logger->log(e.what(), logger::severity::warning);
                                    }
                                }
                                if (logger != nullptr){
                                    logger->log("Tree was balanced (after removing)", logger::severity::debug);
                                }
                                if (*current_node != nullptr) {
                                    reinterpret_cast<rb_node *>(*current_node)->node_color = color::black;
                                }
                                return;
                            }

                        }
                        else{
                            if (reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>(way_to_remove->top())->right_node != nullptr){
                                reinterpret_cast<rb_node*>(reinterpret_cast<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node*>(way_to_remove->top())->right_node)->node_color = color::black;

                                if (logger != nullptr){
                                    logger->log("Tree was balanced (after removing)", logger::severity::debug);
                                }
                                if (*current_node != nullptr) {
                                    reinterpret_cast<rb_node *>(*current_node)->node_color = color::black;
                                }
                                return;
                            }else{
                                try{
                                    remove_fixup(rb_way_to_remove, side, dop, *rb_current_node);
                                }catch(const std::logic_error& e){
                                    if (logger != nullptr){
                                        logger->log(e.what(), logger::severity::warning);
                                    }
                                }
                                if (logger != nullptr){
                                    logger->log("Tree was balanced (after removing)", logger::severity::debug);
                                }
                                if (*current_node != nullptr) {
                                    reinterpret_cast<rb_node *>(*current_node)->node_color = color::black;
                                }
                                return;


                            }
                        }
                    }
                }
            }


            if (logger != nullptr){
                logger->log("Tree was balanced (after removing)", logger::severity::debug);
            }
        }


    };
/////////////tree constructor///////////
public:

    explicit red_black_tree(memory* alloc = nullptr, logger* logger = nullptr): binary_search_tree<tkey, tvalue, tkey_comparer>(alloc, logger, nullptr, new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_class(this), new rb_insert_class(this), new rb_remove_class(this)){
        if (logger!= nullptr){
            logger->log("Red-black tree was created.", logger::severity::debug);
        }
    }


    //копирование
    red_black_tree(const red_black_tree<tkey, tvalue, tkey_comparer>& tree){
        this->_root = tree.copy();
        this->_allocator = tree._allocator;
        this->_logger = tree._logger;
        this->_class_find = new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_class(this);
        this->_class_insert = new rb_insert_class(this);        this->_class_remove = new rb_remove_class(this);
        if (this->_logger != nullptr){
            this->_logger->log("Red-black tree was copied.", logger::severity::debug);
        }


    }
    //перемещение по ссылке
    //NB: не забыть удалить старые данные!
    red_black_tree(red_black_tree<tkey, tvalue, tkey_comparer> && other) noexcept{
        this->_root = other._root;
        this->_allocator = other._allocator;
        this->_logger = other._logger;
        this->_class_find = new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_class(this);
        this->_class_insert = new rb_insert_class(this);        this->_class_remove = new rb_remove_class(this);

        delete other._class_find;
        delete other._class_remove;
        delete other._class_insert;
        other._allocator = nullptr;
        other._class_remove = nullptr;
        other._class_insert = nullptr;
        other._class_find = nullptr;
        other._logger = nullptr;
        other._root = nullptr;

        if (this->_logger != nullptr){
            this->_logger->log("Red-black tree was moved.", logger::severity::debug);
        }
    }

    red_black_tree<tkey, tvalue, tkey_comparer>& operator=(const red_black_tree<tkey, tvalue, tkey_comparer>& other){
        if (this!= &other){
            this->clear();
            this->_root = other.copy();
        }

        if (this->_logger != nullptr){
            this->_logger->log("Red-black tree was appropriated.", logger::severity::debug);
        }

        return *this;
    }

    red_black_tree<tkey, tvalue, tkey_comparer>& operator=(const red_black_tree<tkey, tvalue, tkey_comparer>&& other) noexcept {
        if(&other != this){
            this->clear();
            this->_root = other._root;
            this->_allocator = other._allocator;
            this->_logger = other._logger;

            delete other._class_find;
            delete other._class_remove;
            delete other._class_insert;
            other._allocator = nullptr;
            other._class_remove = nullptr;
            other._class_insert = nullptr;
            other._class_find = nullptr;
            other._logger = nullptr;
            other._root = nullptr;

        }
        if (this->_logger != nullptr){
            this->_logger->log("Red-black tree was moved.", logger::severity::debug);
        }
        return *this;
    }

    ~red_black_tree(){
        if (this->_logger != nullptr){
            this->_logger->log("Red-black tree was deleted.", logger::severity::debug);
        }
    }




};

#endif //COURSEWORK_RED_BLACK_TREE_H
