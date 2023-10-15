#ifndef NEWCOURSEWORK_FLY_WEIGHT_H
#define NEWCOURSEWORK_FLY_WEIGHT_H
#include "../red_black_tree/red_black_tree.h"
#include "../allocators/sorted_list.h"


class fly_weight{
private:
    class comparator{
    public:
        comparator() = default;
        int operator()(std::string str1, std::string str2){
            return str1.compare(str2);
        }
    };

    red_black_tree<std::string, int, comparator>* _tree;
    memory* _allocator;

    fly_weight(){
        _allocator = new allocator_sorted_list(2000000000, allocator_sorted_list::types_of_detour::first);
        _tree = new red_black_tree<std::string, int, comparator>(_allocator);
    }
public:
    static fly_weight& get_instance(){
        static fly_weight _instance;
        return _instance;
    }

    std::string& get_fly_weight(const std::string& str_to_check){
        if (-_tree->find_key(str_to_check) == 0){
            _tree->insert(str_to_check, 1);
        }else{
            int num_of_fw;
            num_of_fw = _tree->get(str_to_check);
            num_of_fw++;
            _tree->update_key(str_to_check, num_of_fw);
        }
        return _tree->find_ref_key(str_to_check);
    }

    void remove_fly_weight(const std::string& str_to_check) {
        if (-_tree->find_key(str_to_check) == 0) return;
        int num_of_fw;
        num_of_fw = _tree->get(str_to_check);
        if (num_of_fw <= 1) {
            _tree->remove(str_to_check);
        }else{
            num_of_fw--;
            _tree->update_key(str_to_check, num_of_fw);
        }
    }

    ~fly_weight(){
        delete _tree;
        delete _allocator;
    }

    fly_weight(const fly_weight& other) = delete;
    fly_weight(const fly_weight&& other) = delete;
    fly_weight& operator=(const fly_weight& other) = delete;
    fly_weight& operator=(const fly_weight&& other) = delete;
};

#endif //NEWCOURSEWORK_FLY_WEIGHT_H
