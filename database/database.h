#ifndef COURSEWORK_DATABASE_H
#define COURSEWORK_DATABASE_H

#include <iosfwd>
#include "../red_black_tree/red_black_tree.h"
#include "../binary_search_tree/associative_container.h"
#include "../allocators/allocator_2.h"
#include "../allocators/border_descriptor.h"
#include "../allocators/buddies.h"
#include "../allocators/sorted_list.h"
#include "../pattern_fw/fly_weight.h"
#include "../avl_tree/avl_tree.h"
#include "../splay_tree/splay_tree.h"

#include <queue>

class database final{

public:
    enum class type_of_allocator{
        simple,
        border_descriptor,
        buddies,
        sorted_list
    };

    enum class type_of_allocator_detour{
        none,
        best,
        worst,
        first
    };

    enum class type_of_tree{
        red_black,
        avl,
        splay
    };


private:
    struct player_key final{
        int player_id;
        std::string& game_zone;

        player_key(const int id, const std::string& zone):
        player_id(id),
        game_zone(fly_weight::get_instance().get_fly_weight(zone))
        {}

        player_key& operator=(const player_key& other){
            this->player_id = other.player_id;
            this->game_zone = other.game_zone;
            return *this;
        }



    };

    struct player_value final{
        std::string& nickname;
        std::string& status;
        std::string& valute;
        std::string& premium_valute;
        std::string& experience;
        std::string& date_of_registration;
        std::string& time_in_game;

        player_value(const std::string& nick,  const std::string& stat, const std::string& v, const std::string& pr_v, const std::string& exp, const std::string& date, const std::string& time):
            nickname(fly_weight::get_instance().get_fly_weight(nick)),
            status(fly_weight::get_instance().get_fly_weight(stat)),
            valute(fly_weight::get_instance().get_fly_weight(v)),
            premium_valute(fly_weight::get_instance().get_fly_weight(pr_v)),
            experience(fly_weight::get_instance().get_fly_weight(exp)),
            date_of_registration(fly_weight::get_instance().get_fly_weight(date)),
            time_in_game(fly_weight::get_instance().get_fly_weight(time))
        {}

        player_value& operator=(const player_value& other){
            this->nickname = other.nickname;
            this->status = other.status;
            this->valute = other.valute;
            this->premium_valute = other.premium_valute;
            this->experience = other.experience;
            this->date_of_registration = other.date_of_registration;
            this->time_in_game = other.time_in_game;
            return *this;
        }

        bool operator==(const player_value& other) const{
            if (this->nickname != other.nickname) return false;
            if (this->status != other.status) return false;
            if (this->valute != other.valute) return false;
            if (this->premium_valute != other.premium_valute) return false;
            if (this->experience != other.experience) return false;
            if (this->date_of_registration != other.date_of_registration) return false;
            if (this->time_in_game != other.time_in_game) return false;
            return true;
        }

        bool operator!=(const player_value& other) const{
            return !(*this == other);
        }

    };

    class comparer_string final{
    public:
        comparer_string() = default;

        int operator()(const std::string& player1, const std::string& player2) const{
            return player1.compare(player2);
        }

    };

    class comparer_player_key final{
    public:
        comparer_player_key() = default;

        int operator()(const player_key& key1, const player_key& key2) const{
            int res = key1.player_id - key2.player_id;

            if (res != 0){
                return res;
            }else{
                return key1.game_zone.compare(key2.game_zone);
            }
        }
    };

private:
    red_black_tree<std::string, red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>*, comparer_string>*, comparer_string>* _database;

    logger* _logger;

    memory* _allocator;

    red_black_tree<std::string, memory*, comparer_string>* _pool_with_allocators;

//////////////constructor//////////////
public:
    explicit database(logger* logger):_logger(logger){
        _allocator = new allocator();
        _pool_with_allocators = new red_black_tree<std::string, memory*, comparer_string>(_allocator, _logger);
        _database = new red_black_tree<std::string, red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>*, comparer_string>*, comparer_string>(_allocator, _logger);

        _logger->log("Database was created!", logger::severity::debug);
    }

    database& operator=(const database& other) = delete;
    database& operator=(database&& other) = delete;

    database(const database& other) = delete;
    database(database&& other) = delete;


//////////////destructor //////////////
public:
    ~database(){
        delete_database();
        delete _pool_with_allocators;
        delete _database;
        delete _allocator;

        _logger->log("Database was deleted!", logger::severity::debug);
    }


private:
    red_black_tree<std::string, red_black_tree<std::string,associative_container<player_key, player_value>*, comparer_string>*, comparer_string>* pool_from_str(const std::string& is_pool){

        red_black_tree<std::string, red_black_tree<std::string,associative_container<player_key, player_value>*, comparer_string>*, comparer_string>* pool;

        try{
            pool = _database->get(is_pool);
        }catch(const std::logic_error& e){
            std::string message = "Error getting pool: there is no " + is_pool;
            throw std::logic_error(message);
        }
        return pool;

    }

    static red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* scheme_from_str(const std::string& is_scheme, const red_black_tree<std::string, red_black_tree<std::string,associative_container<player_key, player_value>*, comparer_string>*, comparer_string>* pool){

        red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* scheme;

        try{
            scheme = pool->get(is_scheme);
        }catch(const std::exception& e){
            std::string message = "Error getting scheme: there is no " + is_scheme + " !";
            throw std::logic_error(message);
        }
        return scheme;
    }

    static associative_container<player_key, player_value>* collection_from_str(const std::string& is_collection, const red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* scheme){
        associative_container<player_key, player_value>* collection;

        try{
            collection = scheme->get(is_collection);
        }
        catch(const std::exception& e){
            std::string message = "Error getting collection: there is no " + is_collection + " ! ";
            throw std::logic_error(message);
        }
        return collection;
    }

private:
    void insert(const player_key& key, const player_value& value, const std::string& is_pool_str, const std::string& is_scheme_str, const std::string& is_collection_str){
        red_black_tree<std::string, red_black_tree<std::string,  associative_container<player_key, player_value>*, comparer_string>*, comparer_string>* pool;

        red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* scheme;

        associative_container<player_key, player_value>* collection;

        try {
            pool = pool_from_str(is_pool_str);
            scheme = scheme_from_str(is_scheme_str, pool);
            collection = collection_from_str(is_collection_str, scheme);
        }catch(const std::logic_error& e){
            fly_weight::get_instance().remove_fly_weight(key.game_zone);
            fly_weight::get_instance().remove_fly_weight(value.nickname);
            fly_weight::get_instance().remove_fly_weight(value.status);
            fly_weight::get_instance().remove_fly_weight(value.valute);
            fly_weight::get_instance().remove_fly_weight(value.premium_valute);
            fly_weight::get_instance().remove_fly_weight(value.experience);
            fly_weight::get_instance().remove_fly_weight(value.date_of_registration);
            fly_weight::get_instance().remove_fly_weight(value.time_in_game);
            throw e;
        }

        try{
            collection->insert(key, value);
        }catch(const std::logic_error& e){
            fly_weight::get_instance().remove_fly_weight(key.game_zone);
            fly_weight::get_instance().remove_fly_weight(value.nickname);
            fly_weight::get_instance().remove_fly_weight(value.status);
            fly_weight::get_instance().remove_fly_weight(value.valute);
            fly_weight::get_instance().remove_fly_weight(value.premium_valute);
            fly_weight::get_instance().remove_fly_weight(value.experience);
            fly_weight::get_instance().remove_fly_weight(value.date_of_registration);
            fly_weight::get_instance().remove_fly_weight(value.time_in_game);
            std::string message = "A problem in the collection ";
            message += is_collection_str + " : " + e.what();
            throw std::logic_error(message);
        }

        _logger->log("Done inserting. ", logger::severity::information);
    }

    player_value get_value_from_key(const player_key& key, const std::string& is_pool, const std::string& is_scheme, const std::string& is_collection){

        red_black_tree<std::string, red_black_tree<std::string,  associative_container<player_key, player_value>*, comparer_string>*, comparer_string>* pool;

        red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* scheme;

        associative_container<player_key, player_value>* collection;
        try {
            pool = pool_from_str(is_pool);
            scheme = scheme_from_str(is_scheme, pool);
            collection = collection_from_str(is_collection, scheme);
        }catch (const std::logic_error& err){
            fly_weight::get_instance().remove_fly_weight(key.game_zone);
            throw err;
        }


        try{
            player_value value = collection->get(key);
            _logger->log("Value by key was read. ", logger::severity::information);
            return value;
        }
        catch(const std::logic_error& e){
            fly_weight::get_instance().remove_fly_weight(key.game_zone);
            std::string message = "A problem in collection: there is no key in " + is_collection;
            throw std::logic_error(message);
        }

    }


    std::queue<std::tuple<player_key, player_value>> read_range(const player_key* key1, const player_key* key2, const std::string& is_pool, const std::string& is_scheme, const std::string& is_collection){

        if (_logger != nullptr) _logger->log("~~~~~Start reading range~~~~~", logger::severity::debug);

        red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>*, comparer_string>* pool;
        red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* scheme;
        associative_container<player_key, player_value>* collection;

        try{
            pool = pool_from_str(is_pool);
            scheme = scheme_from_str(is_scheme, pool);
            collection = collection_from_str(is_collection, scheme);
        }catch (const std::logic_error& e){
            fly_weight::get_instance().remove_fly_weight(key1->game_zone);
            fly_weight::get_instance().remove_fly_weight(key2->game_zone);
            throw e;
        }
        bool f = true;
        std::queue<std::tuple<player_key, player_value>> my_queue = std::queue<std::tuple<player_key, player_value>>();
        auto* collection_bst = dynamic_cast<binary_search_tree<player_key, player_value, comparer_player_key>*>(collection);
        auto iterator = collection_bst->infix_it_begin();
        auto comparator = comparer_player_key();


        if (key1 != nullptr){

            while (comparator(std::get<1>(*iterator), collection_bst->find_for_read_range(*key1)) != 0 && iterator != dynamic_cast<binary_search_tree<player_key, player_value, comparer_player_key>*>(collection)->infix_it_end()){
                iterator++;
            }
            if (iterator == collection_bst->infix_it_end()){
                f = false;
            }
        }
        if (f){
            if (key2 != nullptr){
                while (iterator != collection_bst->infix_it_end() && comparator(std::get<1>(*iterator), *key2) < 0){
                    my_queue.emplace(std::get<1>(*iterator), std::get<2>(*iterator));
                    iterator++;
                }
                if (iterator != collection_bst->infix_it_end() && comparator(std::get<1>(*iterator), *key2) == 0){
                    my_queue.emplace(std::get<1>(*iterator), std::get<2>(*iterator));
                }
            }else{
                for (; iterator != collection_bst->infix_it_end(); ++iterator){
                    my_queue.emplace(std::get<1>(*iterator), std::get<2>(*iterator));
                }
            }

        }
        if (_logger != nullptr) _logger->log("~~~~~Finish reading range~~~~~", logger::severity::debug);

        return my_queue;
    }


    void remove(const player_key& key, const std::string& is_pool, const std::string& is_scheme, const std::string& is_collection){

        red_black_tree<std::string, red_black_tree<std::string,  associative_container<player_key, player_value>*, comparer_string>*, comparer_string>* pool;

        red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* scheme;

        associative_container<player_key, player_value>* collection;

        try{
            pool = pool_from_str(is_pool);
            scheme = scheme_from_str(is_scheme, pool);
            collection = collection_from_str(is_collection, scheme);
        }catch(const std::logic_error& e){
            fly_weight::get_instance().remove_fly_weight(key.game_zone);
            throw e;
        }

        try{
            player_value value = collection->remove(key);
            fly_weight::get_instance().remove_fly_weight(key.game_zone);
            fly_weight::get_instance().remove_fly_weight(value.nickname);
            fly_weight::get_instance().remove_fly_weight(value.status);
            fly_weight::get_instance().remove_fly_weight(value.valute);
            fly_weight::get_instance().remove_fly_weight(value.premium_valute);
            fly_weight::get_instance().remove_fly_weight(value.experience);
            fly_weight::get_instance().remove_fly_weight(value.date_of_registration);
            fly_weight::get_instance().remove_fly_weight(value.time_in_game);

        }
        catch(const std::logic_error& e){
            fly_weight::get_instance().remove_fly_weight(key.game_zone);
            std::string message = "A problem in collection: there is no key in ";
            message += is_collection;
            throw std::logic_error(message);
        }

        _logger->log("Done removing. ", logger::severity::information);


    }

    void update_key(const player_key& key, const player_value& value, const std::string& is_pool, const std::string& is_scheme, const std::string& is_collection){
        try {
            this->remove(key, is_pool, is_scheme, is_collection);
            this->insert(key, value, is_pool, is_scheme, is_collection);
        }catch(const std::logic_error &err){
            fly_weight::get_instance().remove_fly_weight(key.game_zone);
            fly_weight::get_instance().remove_fly_weight(value.nickname);
            fly_weight::get_instance().remove_fly_weight(value.status);
            fly_weight::get_instance().remove_fly_weight(value.valute);
            fly_weight::get_instance().remove_fly_weight(value.premium_valute);
            fly_weight::get_instance().remove_fly_weight(value.experience);
            fly_weight::get_instance().remove_fly_weight(value.date_of_registration);
            fly_weight::get_instance().remove_fly_weight(value.time_in_game);
            throw err;
        }

        _logger->log("Done updating. ", logger::severity::information);
    }

///////////////add and delete POOL///////////////
    void add_pool(const std::string& is_pool, type_of_allocator is_allocator, size_t size_for_alloc, type_of_allocator_detour detour = type_of_allocator_detour::none){

        bool pool_not_exists = false;
        try{
            _database->get(is_pool);
        }
        catch(const std::logic_error& err){
            pool_not_exists = true;
        }

        if (!pool_not_exists){
            std::string message = "Pool '" + is_pool + "' already exists.";
            throw std::logic_error(message);
        }

        memory* alloc;
        if (is_allocator == type_of_allocator::simple){
            alloc = new allocator(_logger);
        }
        else if(is_allocator == type_of_allocator::border_descriptor){

            if (detour == type_of_allocator_detour::best){
                alloc = new allocator_bord(size_for_alloc, _logger, nullptr,allocator_bord::detour_type::best);
            }
            else if (detour == type_of_allocator_detour::first){
                alloc = new allocator_bord(size_for_alloc, _logger, nullptr,allocator_bord::detour_type::first);
            }
            else if (detour == type_of_allocator_detour::worst){
                alloc = new allocator_bord(size_for_alloc, _logger, nullptr,allocator_bord::detour_type::worst);
            }
        }
        else if(is_allocator == type_of_allocator::buddies){
            alloc = new allocator_buddies(size_for_alloc, _logger, nullptr);
        }
        else if(is_allocator == type_of_allocator::sorted_list){
            if (detour == type_of_allocator_detour::best){
                alloc = new allocator_sorted_list(size_for_alloc, allocator_sorted_list::types_of_detour::best, _logger,
                                                  nullptr);
            }
            else if (detour == type_of_allocator_detour::first){
                alloc = new allocator_sorted_list(size_for_alloc, allocator_sorted_list::types_of_detour::first, _logger,
                                                  nullptr);
            }
            else if (detour == type_of_allocator_detour::worst){
                alloc = new allocator_sorted_list(size_for_alloc, allocator_sorted_list::types_of_detour::worst, _logger,
                                                  nullptr);
            }
        }

        auto* pool_tree = reinterpret_cast<red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* , comparer_string>*>(alloc->allocate(reinterpret_cast<size_t>(sizeof(red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>*, comparer_string>))));
        if(pool_tree == nullptr) throw std::logic_error("Too few memory...");
        new (pool_tree) red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* , comparer_string>(alloc, _logger);

        _database->insert(is_pool, pool_tree);
        _pool_with_allocators->insert(is_pool, alloc);
        _logger->log("Pool was added", logger::severity::information);


    }

    void delete_pool(const std::string& is_pool){
        red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* , comparer_string>* pool = pool_from_str(is_pool);

        auto with = pool->postfix_it_begin();
        auto until = pool->postfix_it_begin();

        for (; with!=until; ++with){
            auto is_scheme = std::get<1>(*with);
            delete_scheme(is_scheme, is_pool);
        }

        auto* alloc = _pool_with_allocators->get(is_pool);
        pool->~red_black_tree();
        alloc->deallocate(reinterpret_cast<void*>(pool));
        _pool_with_allocators->remove(is_pool);
        delete alloc;
        _database->remove(is_pool);

        _logger->log("Pool was deleted", logger::severity::information);
    }


////////////////add and delete SCHEME////////////////
    void add_scheme (const std::string& is_scheme, const std::string& is_pool){
        red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* , comparer_string>* pool = pool_from_str(is_pool);
        bool okey = false;
        try{
            pool->get(is_scheme);
        }
        catch(const std::logic_error& e){
            okey = true;
        }
        if (!okey){
            std::string message = "Scheme '" + is_scheme + " already exists.";
            throw std::logic_error(message);
        }

        auto* alloc = _pool_with_allocators->get(is_pool);
        auto* tree_scheme = reinterpret_cast<red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>*>(alloc->allocate(reinterpret_cast<size_t>(sizeof(red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>))));
        if(tree_scheme == nullptr) throw std::logic_error("Too few memory...");
        new (tree_scheme) red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>(alloc, _logger);
        pool->insert(is_scheme, tree_scheme);

        _logger->log("Scheme was added.", logger::severity::information);



    }

    void delete_scheme(const std::string& is_scheme, const std::string& is_pool){
        red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* , comparer_string>* pool = pool_from_str(is_pool);

        red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* scheme = scheme_from_str(is_scheme , pool );

        auto with = scheme->postfix_it_begin();
        auto until = scheme->postfix_it_begin();

        for (; with!=until; ++with){
            auto is_collection = std::get<1>(*with);
            delete_collection(is_collection,is_scheme, is_pool);
        }

        auto* alloc = _pool_with_allocators->get(is_pool);
        scheme->~red_black_tree();
        alloc->deallocate(reinterpret_cast<void*>(scheme));
        _pool_with_allocators->remove(is_scheme);
        delete alloc;
        _database->remove(is_scheme);

        _logger->log("Scheme was deleted", logger::severity::information);


    }

    ////////////////////////add and delete COLLECTION///////////
    void add_collection(const std::string& is_collection, const std::string& is_pool, const std::string& is_scheme, type_of_tree tree_type){
        red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* , comparer_string>* pool = pool_from_str(is_pool);

        red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* scheme = scheme_from_str(is_scheme , pool );

        bool okey = false;
        try{
            scheme->get(is_collection);
        }
        catch(const std::logic_error& e){
            okey = true;
        }
        if (!okey){
            std::string message = "Collection '" + is_collection + " already exists.";
            throw std::logic_error(message);
        }

        auto* alloc = _pool_with_allocators->get(is_pool);
        if (tree_type == type_of_tree::red_black){
            auto* tree_collection = reinterpret_cast<red_black_tree<player_key, player_value, comparer_player_key>*>(alloc->allocate(reinterpret_cast<size_t>(sizeof(red_black_tree<player_key, player_value, comparer_player_key>))));
            if(tree_collection == nullptr) throw std::logic_error("Too few memory...");
            new (tree_collection) red_black_tree<player_key, player_value, comparer_player_key>(alloc, _logger);
            scheme->insert(is_collection, tree_collection);
        }else if(tree_type == type_of_tree::avl){
            auto* tree_collection = reinterpret_cast<avl_tree<player_key, player_value, comparer_player_key>*>(alloc->allocate(reinterpret_cast<size_t>(sizeof(avl_tree<player_key, player_value, comparer_player_key>))));
            if(tree_collection == nullptr) throw std::logic_error("Too few memory...");
            new (tree_collection) avl_tree<player_key, player_value, comparer_player_key>(alloc, _logger);
            scheme->insert(is_collection, tree_collection);
        }else if(tree_type == type_of_tree::splay){
            auto* tree_collection = reinterpret_cast<splay_tree<player_key, player_value, comparer_player_key>*>(alloc->allocate(reinterpret_cast<size_t>(sizeof(splay_tree<player_key, player_value, comparer_player_key>))));
            if(tree_collection == nullptr) throw std::logic_error("Too few memory...");
            new (tree_collection) splay_tree<player_key, player_value, comparer_player_key>(_logger, alloc);
            scheme->insert(is_collection, tree_collection);
        }

        _logger->log("Collection was added.", logger::severity::information);



    }

    void delete_collection(const std::string& is_collection,const std::string& is_scheme, const std::string& is_pool){
        red_black_tree<std::string, red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* , comparer_string>* pool = pool_from_str(is_pool);

        red_black_tree<std::string, associative_container<player_key, player_value>*, comparer_string>* scheme = scheme_from_str(is_scheme , pool );

        associative_container<player_key, player_value>* collection = collection_from_str(is_collection, scheme);

        auto begin = dynamic_cast<binary_search_tree<player_key, player_value, comparer_player_key>*>(collection)->infix_it_begin();
        auto end = dynamic_cast<binary_search_tree<player_key, player_value, comparer_player_key>*>(collection)->infix_it_end();
        for(; begin != end; begin++){
            auto& value_to_remove = std::get<2>(*begin);
            auto& key_to_remove = std::get<1>(*begin);
            fly_weight::get_instance().remove_fly_weight(key_to_remove.game_zone);
            fly_weight::get_instance().remove_fly_weight(value_to_remove.nickname);
            fly_weight::get_instance().remove_fly_weight(value_to_remove.status);
            fly_weight::get_instance().remove_fly_weight(value_to_remove.valute);
            fly_weight::get_instance().remove_fly_weight(value_to_remove.premium_valute);
            fly_weight::get_instance().remove_fly_weight(value_to_remove.experience);
            fly_weight::get_instance().remove_fly_weight(value_to_remove.date_of_registration);
            fly_weight::get_instance().remove_fly_weight(value_to_remove.time_in_game);
        }

        auto* alloc = _pool_with_allocators->get(is_pool);
        collection->~associative_container();
        alloc->deallocate(reinterpret_cast<void*>(collection));
        scheme->remove(is_collection);
        _logger->log("Collection was deleted", logger::severity::information);
    }

    void delete_database(){
        auto with = _database->postfix_it_begin();
        auto until = _database->postfix_it_end();

        for(; with!=until; ++with){
            auto is_pool = std::get<1>(*with);
            delete_pool(is_pool);
        }

        _logger->log("Database was deleted.", logger::severity::information);


    }

    ///////////////parsing////////////
private:
    static player_key key_parsing(const std::string& key){
        size_t l;
        size_t r;
        if (key.find("key: ")!=0){
            throw std::logic_error("Invalid key!");
        }
        std::string key_in_str = key.substr(5);
        l = key_in_str.find('[');
        r = key_in_str.find(']');
        if (l != 0 || r == std::string::npos){
            throw std::logic_error("Invalid key!");
        }
        std::string id_in_str = key_in_str.substr(1, r-1);

        //TODO: проверка на валидность введенного ключа

        if (id_in_str.empty()){
            throw std::logic_error("Invalid Player ID.");
        }
        for (char c : id_in_str){
            if (!isdigit(c)) throw std::logic_error("Invalid Player ID.");
        }
        ///////////////
        int id = stoi(id_in_str);


        key_in_str = key_in_str.substr(r+2);
        l = key_in_str.find('[');
        r = key_in_str.find(']');
        if (l != 0 || r == std::string::npos){
            throw std::logic_error("Invalid key!");
        }
        std::string gamezone_in_str = key_in_str.substr(1, r-1);

        if (gamezone_in_str.empty()){
            throw std::logic_error("Invalid GameZone.");
        }

        player_key res = {id, gamezone_in_str};
        return res;
    }

    static player_value value_parsing(const std::string& value){
        if (value.find("value: ") != 0){
            throw std::logic_error("Invalid value!");
        }
        std::string value_in_string = value.substr(7);

        std::string nickname_in_str;
        std::string status_in_str;
        std::string valute_in_str;
        std::string premium_valute_in_str;
        std::string experience_in_str;
        std::string date_of_registration_in_str;
        std::string time_in_game_in_str;
        try{
            nickname_in_str = field_pasing(value_in_string, "nickname");
            status_in_str = field_pasing(value_in_string, "status");
            valute_in_str = field_pasing(value_in_string, "valute");
            premium_valute_in_str = field_pasing(value_in_string, "premium value");
            experience_in_str = field_pasing(value_in_string, "experience");
            date_of_registration_in_str = field_pasing(value_in_string, "date of");
        }catch (const std::logic_error& e) {throw e;}

        size_t l = value_in_string.find('[');
        size_t r = value_in_string.find(']');
        if(l!=0 ) throw std::logic_error("Invalid format of value");

        time_in_game_in_str = value_in_string.substr(1, r-1);
        if (time_in_game_in_str.empty()) throw std::logic_error("Invalid format of time in game");

        if(status_in_str.compare("moderator") == 0 || status_in_str.compare("premium") == 0 || status_in_str.compare("simple") == 0 || status_in_str.compare("admin") == 0){

        }else{
            throw std::logic_error("Invalid status!");
        }



        player_value res = {nickname_in_str, status_in_str,valute_in_str, premium_valute_in_str, experience_in_str, date_of_registration_in_str, time_in_game_in_str};

        return res;


    }

    static std::string field_pasing(std::string& value_in_str, const std::string& name){
        size_t l = value_in_str.find('[');
        size_t r = value_in_str.find(']');
        if (l != 0 || r == std::string::npos){
            throw std::logic_error("Invalid enter!");
        }
        std::string res = value_in_str.substr(1, r - 1);
        if (res.empty()){
            std::string message = "Invalid " + name;
            throw std::logic_error(message);
        }

        value_in_str = value_in_str.substr(r+2);
        return res;


    }

    static std::string value_to_output(const player_value& value){
        std::string res = "\n Information about player:\n  nickname: " + value.nickname;
        res += "\n  status: " + value.status;
        res += "\n  valute: " + value.valute;
        res += "\n  premium valute: " + value.premium_valute;
        res += "\n  experience: " + value.experience;
        res += "\n  date of registration: " + value.date_of_registration;
        res += "\n  time in game: " + value.time_in_game;
        return res;

    }


    static std::string key_and_value_print(const std::pair<player_key, player_value>& key_value_pair){
        player_key key = std::get<0>(key_value_pair);
        player_value value = std::get<1>(key_value_pair);
        std::string res = "\nKey:\n player_ID: " + std::to_string(key.player_id) + "\n  game zone: " + key.game_zone;

        res += value_to_output(value);
        return res;

    }
    static void coll_sch_pool_parsing(std::string& is_pool, std::string& is_scheme, std::string& is_collection, std::string& command_in_str){

        is_pool = field_pasing(command_in_str, "pool name");
        is_scheme = field_pasing(command_in_str, "scheme name");


        size_t l = command_in_str.find('[');
        size_t r = command_in_str.find(']');
        if(l != 0 || r == std::string::npos) {
            throw std::logic_error("Invalid enter");
        }
        is_collection = command_in_str.substr(1, r - 1);
        if(is_collection.empty()){
            throw std::logic_error("Invalid collection");
        }
    }
public:
    void command_parsing(const std::string& command, std::string& key, std::string& value){
        std::string command_str;
        size_t l;
        size_t r;

        std::fstream fin_write;
        fin_write.open("/home/dina/CLionProjects/NEWcoursework/database/writing.txt", std::ios::app);
        if(!fin_write.is_open()){
            throw std::logic_error("Could not open a file for writing!");
        }
        if(command.find("insert: ") == 0){
            command_str = command.substr(8);
            std::string is_pool;
            std::string is_scheme;
            std::string is_collection;

            try{
                coll_sch_pool_parsing(is_pool, is_scheme, is_collection, command_str);
            } catch(const std::logic_error& ex){
                throw ex;
            }



            try{
                player_key k = std::move(key_parsing(key));
                player_value v = std::move(value_parsing(value));
                insert(k, v, is_pool, is_scheme, is_collection);

                fin_write<<"insert: ["<<is_pool<<"]"<<" ["<<is_scheme<<"] "<<"["<<is_collection<<"]"<<std::endl;
                fin_write<<"key: ["<<k.player_id<<"] ["<<k.game_zone<<"]"<<std::endl;
                fin_write<<"value: ["<<v.nickname<<"] ["<<v.status<<"] ["<<v.valute<<"] ["<<v.premium_valute<<"] ["<<v.experience<<"] ["<<v.date_of_registration<<"] ["<<v.time_in_game<<"]"<<std::endl;

                std::string message = " ~~Insert finished!\n--key: [id: " + std::to_string(k.player_id) + ", game zone: " + k.game_zone + "] ";
                _logger->log(message, logger::severity::warning);
            }catch(const std::logic_error& err) {
                throw err;
            }


        }
        else if(command.find("read key: ") == 0){
            command_str = command.substr(10);

            std::string is_pool;
            std::string is_scheme;
            std::string is_collection;

            try{ coll_sch_pool_parsing(is_pool, is_scheme, is_collection, command_str); } catch(const std::logic_error& ex){ throw ex; }



            try{
                player_key k = std::move(key_parsing(key));
                fly_weight::get_instance().remove_fly_weight(k.game_zone);
                player_value v = get_value_from_key(k, is_pool, is_scheme, is_collection);

                fin_write<<"read key: ["<<is_pool<<"]"<<" ["<<is_scheme<<"] "<<"["<<is_collection<<"]"<<std::endl;
                fin_write<<"key: ["<<k.player_id<<"] ["<<k.game_zone<<"]"<<std::endl;

                std::string message = " ~~Reading value from key finished!!\n--key: [id: " + std::to_string(k.player_id) + ", game zone: " + k.game_zone + "] " + value_to_output(v);
                _logger->log(message, logger::severity::warning);
            }catch(const std::logic_error& ex) {throw ex;}



        }
        else if(command.find("update key: ") == 0){
            command_str = command.substr(12);

            std::string is_pool;
            std::string is_scheme;
            std::string is_collection;

            try{ coll_sch_pool_parsing(is_pool, is_scheme, is_collection, command_str); } catch(const std::logic_error& ex){ throw ex; }

            try{
                player_key k = std::move(key_parsing(key));
                player_value v = std::move(value_parsing(value));
                update_key(k, v, is_pool, is_scheme, is_collection);

                fin_write<<"update key: ["<<is_pool<<"] "<<"["<<is_scheme<<"] "<<"["<<is_collection<<"]"<<std::endl;
                fin_write<<"key: ["<<k.player_id<<"] ["<<k.game_zone<<"]"<<std::endl;
                fin_write<<"value: ["<<v.nickname<<"] ["<<v.status<<"] ["<<v.valute<<"] ["<<v.premium_valute<<"] ["<<v.experience<<"] ["<<v.date_of_registration<<"] ["<<v.time_in_game<<"]"<<std::endl;

                std::string message = "~~Update key finished!\n--key: [id: " + std::to_string(k.player_id) +  ", game zone: " + k.game_zone + "] ";
                _logger->log(message, logger::severity::warning);
            }catch(const std::logic_error& ex) {throw ex;}



        }
        else if(command.find("remove: ") == 0){
            command_str = command.substr(8);

            std::string is_pool;
            std::string is_scheme;
            std::string is_collection;

            try{ coll_sch_pool_parsing(is_pool, is_scheme, is_collection, command_str); } catch(const std::logic_error& ex){ throw ex; }


            try{
                player_key k = std::move(key_parsing(key));

                fly_weight::get_instance().remove_fly_weight(k.game_zone);
                remove(k, is_pool, is_scheme, is_collection);

                fin_write<<"remove: ["<<is_pool<<"] ["<<is_scheme<<"] ["<<is_collection<<"]"<<std::endl;
                fin_write<<"key: ["<<k.player_id<<"] ["<<k.game_zone<<"]"<<std::endl;

                std::string message = " ~~Remove finished!\n--key: [id: " + std::to_string(k.player_id) +  ", game zone: " + k.game_zone + "] ";
                _logger->log(message, logger::severity::warning);
            }catch(const std::logic_error& ex) {throw ex;}


        }



            //////////////add pool command
        else if(command.find("add pool: ") == 0){
            command_str = command.substr(10);
            std::string pool_name;
            type_of_allocator alloc_name;
            size_t size;
            type_of_allocator_detour alloc_mode;

            try{ pool_name = field_pasing(command_str, "pool name");} catch(const std::logic_error& ex){ throw ex; }

            l = command_str.find("{ ");
            if(l != 0) throw std::logic_error("Wrong format of a command!");
            command_str = command_str.substr(2);

            l = command_str.find('[');
            if(l != 0) throw std::logic_error("Wrong format of a command!");
            r = command_str.find("] ");
            if(r == std::string::npos) throw std::logic_error("Wrong format of a command!");
            std::string alloc_name_str = command_str.substr(1, r - 1);
            if(alloc_name_str.empty()) throw std::logic_error("Invalid name of allocator :/");

            if(alloc_name_str == "border_descriptor") alloc_name = type_of_allocator::border_descriptor;
            else if(alloc_name_str == "simple") alloc_name = type_of_allocator::simple;
            else if(alloc_name_str == "buddies") alloc_name = type_of_allocator::buddies;
            else if(alloc_name_str == "sorted_list") alloc_name = type_of_allocator::sorted_list;

            else throw std::logic_error("Error: invalid allocator name");
            command_str = command_str.substr(r + 2);

            if(alloc_name != type_of_allocator::simple){
                l = command_str.find('[');
                if(l != 0) throw std::logic_error("Wrong format of a command!");
                r = command_str.find("] ");
                if(r == std::string::npos) throw std::logic_error("Wrong format of a command!");
                std::string size_str = command_str.substr(1, r - 1);
                if(size_str.empty()) throw std::logic_error("Invalid size of allocator!");
                for(char c : size_str) if(!isdigit(c)) throw std::logic_error("Invalid size of allocator!");
                size = (size_t)(stoi(size_str));
                command_str = command_str.substr(r + 2);
            }else{
                size = 1;
                l = command_str.find('}');
                if(l != 0) throw std::logic_error("Wrong format of a command!");
            }

            std::string alloc_mode_in_str;
            if(alloc_name == type_of_allocator::border_descriptor || alloc_name == type_of_allocator::sorted_list){
                l = command_str.find('[');
                if(l != 0) throw std::logic_error("Wrong format of a command!");
                r = command_str.find("] ");
                if(r == std::string::npos) throw std::logic_error("Wrong format of a command!");
                std::string detour_str = command_str.substr(1, r - 1);
                if(detour_str.empty()) throw std::logic_error("Invalid type of allocator!");

                if(detour_str == "first") {
                    alloc_mode = type_of_allocator_detour::first;
                    alloc_mode_in_str = detour_str;
                }

                else if(detour_str == "best") {
                    alloc_mode = type_of_allocator_detour::best;
                    alloc_mode_in_str = detour_str;
                }

                else if(detour_str == "worst") {
                    alloc_mode = type_of_allocator_detour::worst;
                    alloc_mode_in_str = detour_str;
                }

                else throw std::logic_error("Error: invalid mode");

                command_str = command_str.substr(r + 2);
                l = command_str.find('}');
                if(l != 0) throw std::logic_error("Wrong format of a command!");
            }else{
                alloc_mode = type_of_allocator_detour::none;
                l = command_str.find('}');
                if(l != 0) throw std::logic_error("Wrong format of a command!");
            }

            try{ add_pool(pool_name, alloc_name, size, alloc_mode);
                fin_write<<"add pool: ["<<pool_name<<"] { ["<<alloc_name_str<<"] ["<<std::to_string(size)<<"] ["<<alloc_mode_in_str<<"] }"<<std::endl;

            } catch(const std::logic_error& ex){ throw ex; }

            std::string message = "***** Pool " +  pool_name + " was added succesfully!";
            _logger->log(message, logger::severity::warning);
        }


            ///////////delete pool command
        else if(command.find("delete pool: ") == 0){
            command_str = command.substr(13);

            std::string pool_name;

            l = command_str.find('[');
            if(l != 0) throw std::logic_error("Wrong format of a command!");
            r = command_str.find(']');
            if(r == std::string::npos) throw std::logic_error("Wrong format of a command!");
            pool_name = command_str.substr(1, r - 1);
            if(pool_name.empty()) throw std::logic_error("Invalid pool name!");

            try{
                delete_pool(pool_name);
                fin_write<<"delete pool: ["<<pool_name<<"]"<<std::endl;
            } catch(const std::logic_error& ex){ throw ex; }

            std::string message = "***** Pool " + pool_name + " was deleted successfully!";
            _logger->log(message, logger::severity::warning);
        }

            ////////////add scheme command
        else if(command.find("add scheme: ") == 0){
            command_str = command.substr(12);

            std::string pool_name;
            std::string scheme_name;

            try{ pool_name =  field_pasing(command_str, "pool name"); } catch(const std::logic_error& ex){ throw ex; }

            l = command_str.find('[');
            if(l != 0) throw std::logic_error("Wrong format of a command!");
            r = command_str.find(']');
            if(r == std::string::npos) throw std::logic_error("Wrong format of a command!");
            scheme_name = command_str.substr(1, r - 1);
            if(scheme_name.empty()) throw std::logic_error("Invalid scheme name!");

            try{ add_scheme(scheme_name, pool_name);
                fin_write<<"add scheme: ["<<pool_name<<"] ["<<scheme_name<<"]"<<std::endl;
            } catch(const std::logic_error& ex){ throw ex; }

            std::string message = "***** Scheme '" + scheme_name + "was added in " + pool_name + "!";

            _logger->log(message, logger::severity::warning);
        }
            ///////////delete scheme command
        else if(command.find("delete scheme: ") == 0){
            command_str = command.substr(15);

            std::string pool_name;
            std::string scheme_name;

            try{ pool_name = field_pasing(command_str, "pool name"); } catch(const std::logic_error& ex){ throw ex; }

            l = command_str.find('[');
            if(l != 0) throw std::logic_error("Wrong format of a command!");
            r = command_str.find(']');
            if(r == std::string::npos) throw std::logic_error("Wrong format of a command!");
            scheme_name = command_str.substr(1, r - 1);
            if(scheme_name.empty()) throw std::logic_error("Invalid scheme name!");

            try{
                delete_scheme(scheme_name, pool_name);
                fin_write<<"delete scheme: ["<<pool_name<<"] ["<<scheme_name<<"]"<<std::endl;
            } catch(const std::logic_error& ex){ throw ex; }

            std::string message = "***** Scheme " + scheme_name + " was deleted from " + pool_name + ".";

            _logger->log(message, logger::severity::warning);
        }

            /////////////add collection command
        else if(command.find("add collection: ") == 0){
            command_str = command.substr(16);

            std::string pool_name;
            std::string scheme_name;
            std::string collection_name;
            type_of_tree tree_name;

            try{
                pool_name = field_pasing(command_str, "pool name");
                scheme_name = field_pasing(command_str, "scheme name");
                collection_name = field_pasing(command_str, "collection name");
            }catch(const std::logic_error& ex) { throw ex; }

            l = command_str.find('{');
            if(l != 0) throw std::logic_error("Wrong format of a command!");
            r = command_str.find('}');
            if(r == std::string::npos) throw std::logic_error("Wrong format of a command!");
            std::string tree_name_str = command_str.substr(1, r - 1);
            if(tree_name_str.empty()) throw std::logic_error("Invalid tree name!");

            else if(tree_name_str == "red black tree") tree_name = type_of_tree::red_black;
            else if(tree_name_str == "avl tree") tree_name = type_of_tree::avl;
            else if(tree_name_str == "splay tree") tree_name = type_of_tree::splay;

            else throw std::logic_error("Invalid tree name!");

            try{ add_collection(collection_name, pool_name, scheme_name, tree_name);
                fin_write<<"add collection: ["<<pool_name<<"] ["<<scheme_name<<"] ["<<collection_name<<"] {"<<tree_name_str<<"}"<<std::endl;
            } catch(const std::logic_error& ex){ throw ex; }

            std::string message = "***** Collection " + collection_name + " was added in pool " + pool_name + " in scheme " + scheme_name + "!";
            _logger->log(message, logger::severity::warning);
        }
            ////////////delete collection command
        else if(command.find("delete collection: ") == 0){
            command_str = command.substr(19);

            std::string pool_name;
            std::string scheme_name;
            std::string collection_name;

            try{
                coll_sch_pool_parsing(pool_name, scheme_name, collection_name, command_str);
                delete_collection(collection_name, scheme_name, pool_name);
                fin_write<<"add collection: ["<<pool_name<<"] ["<<scheme_name<<"] ["<<collection_name<<"]"<<std::endl;

            }catch(const std::logic_error& ex) { throw ex; }

            std::string message = "***** Collection " + collection_name + "was deleted from pool " + pool_name + " in scheme " + scheme_name + "!";

            _logger->log(message, logger::severity::warning);
        }
        else if (command.find("read range: ") == 0){
            command_str = command.substr(12);
            std::string pool_name;
            std::string scheme_name;
            std::string collection_name;
            try{
                coll_sch_pool_parsing(pool_name, scheme_name, collection_name, command_str);
            }catch(const std::logic_error& er){
                throw er;
            }
            player_key* key1_ptr = nullptr;
            player_key* key2_ptr = nullptr;
            l = key.find('{');
            if (l != 0){
                throw std::logic_error("Incorrect input: not enough '[' !");
            }

            r = key.find('}');
            if (r == std::string::npos){
                throw std::logic_error("Incorrect input: not enough '[' !");
            }
            key = key.substr(l + 1, r -1);

            l = value.find('{');
            if (l != 0){
                throw std::logic_error("Incorrect input: not enough '[' !");
            }

            r = value.find('}');
            if (r == std::string::npos){
                throw std::logic_error("Incorrect input: not enough '[' !");
            }
            value = value.substr(l + 1, r -1);

            try{
                if (!value.empty() && !key.empty()){
                    player_key key2 = key_parsing(value);
                    fly_weight::get_instance().remove_fly_weight(key2.game_zone);
                    key2_ptr = &key2;
                    player_key key1 = key_parsing(key);
                    fly_weight::get_instance().remove_fly_weight(key1.game_zone);
                    key1_ptr = &key1;
                    std::queue<std::tuple<player_key, player_value>> result;
                    result = read_range(key1_ptr, key2_ptr, pool_name, scheme_name, collection_name);
                    fin_write<<"read range: ["<<pool_name<<"] ["<<scheme_name<<"] ["<<collection_name<<"]"<<std::endl;
                    fin_write<<"{key: ["<<key1_ptr->player_id<<"] ["<<key1_ptr->game_zone<<"]}"<<std::endl;
                    fin_write<<"{key: ["<<key2_ptr->player_id<<"] ["<<key2_ptr->game_zone<<"]}"<<std::endl;
                    std::string message;
                    if (result.empty()){
                        message = "Problem: there are no such data!";
                    }else{
                        std::cout << "----------Reading range----------" << std::endl;
                        while (!result.empty()){

                            message += key_and_value_print(std::make_pair(std::get<0>(result.front()), std::get<1>(result.front())));

                            std::cout << message << std::endl;
                            result.pop();
                            message = "";
                        }
                    }
                }else if (!key.empty() && value.empty()){
                    key2_ptr = nullptr;
                    player_key key1 = key_parsing(key);
                    fly_weight::get_instance().remove_fly_weight(key1.game_zone);
                    key1_ptr = &key1;
                    std::queue<std::tuple<player_key, player_value>> result;
                    result = read_range(key1_ptr, key2_ptr, pool_name, scheme_name, collection_name);
//                    fin_write<<"read range: ["<<pool_name<<"] ["<<scheme_name<<"] ["<<collection_name<<"]"<<std::endl;
//                    fin_write<<"{key: ["<<key1_ptr->player_id<<"] ["<<key1_ptr->game_zone<<"]}"<<std::endl;
//                    fin_write<<"{key: ["<<key2_ptr->player_id<<"] ["<<key2_ptr->game_zone<<"]}"<<std::endl;
                    std::string message;
                    if (result.empty()){
                        message = "Problem: there are no such data!";
                    }else{
                        std::cout << "----------Reading range----------" << std::endl;
                        while (!result.empty()){

                            message += key_and_value_print(std::make_pair(std::get<0>(result.front()), std::get<1>(result.front())));

                            std::cout << message << std::endl;
                            result.pop();
                            message = "";
                        }
                    }
                }else if (key.empty() && !value.empty()){
                    player_key key2 = key_parsing(value);
                    fly_weight::get_instance().remove_fly_weight(key2.game_zone);
                    key2_ptr = &key2;
                    key1_ptr = nullptr;
                    std::queue<std::tuple<player_key, player_value>> result;
                    result = read_range(key1_ptr, key2_ptr, pool_name, scheme_name, collection_name);
//                    fin_write<<"read range: ["<<pool_name<<"] ["<<scheme_name<<"] ["<<collection_name<<"]"<<std::endl;
//                    fin_write<<"{key: ["<<key1_ptr->player_id<<"] ["<<key1_ptr->game_zone<<"]}"<<std::endl;
//                    fin_write<<"{key: ["<<key2_ptr->player_id<<"] ["<<key2_ptr->game_zone<<"]}"<<std::endl;
                    std::string message;
                    if (result.empty()){
                        message = "Problem: there are no such data!";
                    }else{
                        std::cout << "----------Reading range----------" << std::endl;
                        while (!result.empty()){

                            message += key_and_value_print(std::make_pair(std::get<0>(result.front()), std::get<1>(result.front())));

                            std::cout << message << std::endl;
                            result.pop();
                            message = "";
                        }
                    }
                }else{
                    key1_ptr = nullptr;
                    key2_ptr = nullptr;
                    std::queue<std::tuple<player_key, player_value>> result;
                    result = read_range(key1_ptr, key2_ptr, pool_name, scheme_name, collection_name);
//                    fin_write<<"read range: ["<<pool_name<<"] ["<<scheme_name<<"] ["<<collection_name<<"]"<<std::endl;
//                    fin_write<<"{key: ["<<key1_ptr->player_id<<"] ["<<key1_ptr->game_zone<<"]}"<<std::endl;
//                    fin_write<<"{key: ["<<key2_ptr->player_id<<"] ["<<key2_ptr->game_zone<<"]}"<<std::endl;
                    std::string message;
                    if (result.empty()){
                        message = "Problem: there are no such data!";
                    }else{
                        std::cout << "----------Reading range----------" << std::endl;
                        while (!result.empty()){

                            message += key_and_value_print(std::make_pair(std::get<0>(result.front()), std::get<1>(result.front())));

                            std::cout << message << std::endl;
                            result.pop();
                            message = "";
                        }
                    }
                }

            }catch (const std::logic_error& er){
                throw er;
            }

            /*std::queue<std::tuple<player_key, player_value>> result;
            try{
                result = read_range(key1_ptr, key2_ptr, pool_name, scheme_name, collection_name);
                fin_write<<"read range: ["<<pool_name<<"] ["<<scheme_name<<"] ["<<collection_name<<"]"<<std::endl;
                fin_write<<"{key: ["<<key1_ptr->player_id<<"] ["<<key1_ptr->game_zone<<"]}"<<std::endl;
                fin_write<<"{key: ["<<key2_ptr->player_id<<"] ["<<key2_ptr->game_zone<<"]}"<<std::endl;

            }catch (const std::logic_error& er){
                throw er;
            }*/
           /* std::string message;
            if (result.empty()){
                message = "Problem: there are no such data!";
            }else{
                std::cout << "----------Reading range----------" << std::endl;
                while (!result.empty()){

                    message += key_and_value_print(std::make_pair(std::get<0>(result.front()), std::get<1>(result.front())));

                    std::cout << message << std::endl;
                    result.pop();
                    message = "";
                }
            }*/
        }
        else if(command.find("reset database:") == 0){
            delete_database();
            fin_write<<"reset database:"<<std::endl;
            _logger->log("~~~~~~~~~~~Database was deleted!~~~~~~~~~~~", logger::severity::warning);

        }
        else if(command.find("save my commands:") == 0){
            _logger->log("Saving database....\n", logger::severity::warning);
            command_str = command.substr(18);
            std::ofstream file_for_client;
            file_for_client.open(command_str, std::ios::app);
            fin_write.close();
            std::fstream fin_write;
            fin_write.open("/home/dina/CLionProjects/NEWcoursework/database/writing.txt");
            std::string s;
            std::getline(fin_write, s);
            while(!fin_write.eof()){
                file_for_client<<s<<std::endl;
                std::getline(fin_write, s);
            }
            file_for_client.close();
            fin_write.close();
            fin_write.open("/home/dina/CLionProjects/NEWcoursework/database/writing.txt");

        }
        else if(command.find("restore database:") == 0){
            delete_database();
            _logger->log("Restoring database....\n", logger::severity::warning);
            std::cout << key << std::endl;
            run_file_commands(key);


        }
        else throw std::logic_error("Invalid format of command!");
        fin_write.close();
    }
private:
    static std::string name_of_file(const std::string& path){
        size_t tmp = path.find('\\');
        size_t tmp2;
        do{
            tmp2 = tmp;
            tmp = path.find('\\', tmp2 + 1);
        }while(tmp != std::string::npos);
        std::string name = path.substr(tmp2 + 1);
        return name;
    }

public:

    void run_file_commands(const std::string& path){
        std::string message = "~~~~Parsing file << " + name_of_file(path) + ">> ...";
        _logger->log(message, logger::severity::warning);


        std::ifstream fin;
        fin.open(path);
        if(!fin.is_open()){
            throw std::logic_error("Could not open a file!");
        }

        std::string line;
        std::string line2;
        std::string line3;
        while(getline(fin, line)){

            if(line.find("insert: ") == 0 || line.find("update key: ") == 0 || line.find("read range: ") == 0){
                if(!getline(fin, line2)){
                    fin.close();
                    throw std::logic_error("Wrong format of a command! (no key)");
                }
                if(!getline(fin, line3)){
                    fin.close();
                    throw std::logic_error("Wrong format of a command! (no value)");
                }
            }else if(line.find("read key: ") == 0 || line.find("remove: ") == 0 ){
                if(!getline(fin, line2)){
                    fin.close();
                    throw std::logic_error("Wrong format of a command!(no key)");
                }
            }else if(line.find("add pool: ") == 0 || line.find("delete pool: ") == 0 || line.find("add scheme: ") == 0 || line.find("delete scheme: ") == 0 || line.find("add collection: ") == 0 || line.find("delete collection: ") == 0 || line.find("reset database:") == 0 || line.find("save my commands:") == 0){
                try{ command_parsing(line, line2, line3); } catch(const std::logic_error& ex){
                    fin.close();
                    throw ex;
                }
                continue;
            }
            else{
                fin.close();
                throw std::logic_error("Wrong format of a command!");
            }

            try{ command_parsing(line, line2, line3); } catch(const std::logic_error& ex){
                fin.close();
                throw ex;
            }
        }
        fin.close();
        _logger->log("~~~~Parsing file finished!", logger::severity::warning);
    }

    /////////////////////dialog with user/////////
public:

    void dialog(){
        _logger->log("\n\n-------- Welcome! I'm doing my best! --------\n\n", logger::severity::warning);
        std::string action;
        std::string input;

        while(action != "5"){
            _logger->log("\n------------------------------------------\n\n ~~~~~Choose an action ~~~~~\n\n1) Run commands in a file\n2) Enter a command manually\n3) Reset database\n4) Restore database\n5) Exit\n\n~~~~~ Enter:", logger::severity::warning);
            std::cin.clear();
            getline(std::cin, action);

            if(action == "1"){

                _logger->log("\n------------------------------------------\n\n~~~~~ Enter a file's path ~~~~~\n\n~~~~~ Path:", logger::severity::warning);
                std::cin.clear();
                getline(std::cin, input);
                if(input.empty()){
                    _logger->log("\n\nError: empty path, returning...\n", logger::severity::warning);
                    continue;
                }
                try{
                    run_file_commands(input);
                    _logger->log("\n------------------------------------------\n\n~~~~~ Parsing commands finished successfully! ~~~~~\n1) Continue\n2) Exit\n\n~~~~~ Input:", logger::severity::warning);
                    std::string tmp_action;
                    std::cin.clear();
                    getline(std::cin, tmp_action);
                    if(tmp_action == "1"){
                        continue;
                    }else if(tmp_action == "2"){
                        _logger->log("\n------------------------------------------\n\n~~~~~ Exiting... ~~~~~\n", logger::severity::warning);
                        break;
                    }else{
                        _logger->log("\n\nError: undefined action, choosing 'continue' for safety...\n", logger::severity::warning);
                        continue;
                    }
                }catch(const std::logic_error& ex){
                    std::string tmp_action;
                    std::string message = "\n\n";
                    message += ex.what();
                    message += "\n";
                    _logger->log(message, logger::severity::warning);
                    _logger->log("\n------------------------------------------\n\n~~~~~ Parsing commands failed! ~~~~~\n\n1) Continue\n2) Reset and continue\n3) Exit\n\n~~~> Input:", logger::severity::warning);
                    std::cin.clear();
                    getline(std::cin, tmp_action);
                    if(tmp_action == "1") {
                        continue;
                    }else if(tmp_action == "2"){
                        _logger->log("\n------------------------------------------\n\n~~~~~ Continuing, Resetting... ~~~~~\n", logger::severity::warning);
                        delete_database();
                        continue;
                    }else if(tmp_action == "3"){
                        _logger->log("\n------------------------------------------\n\n~~~~~ Exiting... ~~~~~\n", logger::severity::warning);
                        break;
                    }else{
                        _logger->log("\nError: undefined action, choosing 'continue' for safety...\n", logger::severity::warning);
                        continue;
                    }
                }

            }else if(action == "2"){
                std::string command_action;

                std::string command_line;
                std::string key_line;
                std::string value_line;

                _logger->log("\n------------------------------------------\n\n~~~~~ Choose a command ~~~~~\n\n1) insert\n2) read key\n3) read range\n4) update key\n5) remove\n6) add pool\n7) delete pool\n8) add scheme\n9) delete scheme\n10) add collection\n11) delete collection\n12) save commands\n~~~> Command:", logger::severity::warning);
                std::cin.clear();
                getline(std::cin, command_action);

                if(command_action == "1" || command_action == "2" || command_action == "3" || command_action == "4" || command_action == "5" || command_action == "6" || command_action == "7" || command_action == "8" || command_action == "9" || command_action == "10" || command_action == "11" || command_action == "12"){
                    if(command_action == "1"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: insert format:\ninsert: [pool_name] [scheme_name] [collection_name]\nkey: [id] [game_zone]\nvalue: [nickname] [status] [valute] [premium valute] [experience] [date of registration] [time in game (min)]\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "2"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: read key format:\nread key: [pool_name] [scheme_name] [collection_name]\nkey: [id] [game zone]\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "3"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: read range format:\nread range: [pool_name] [scheme_name] [collection_name]\nkeys: [id, game zone](optional) [id, game zone](optional)\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "4"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: update key format:\nupdate key: [pool_name] [scheme_name] [collection_name]\nkey: [id] [game zone]\nvalue: [nickname] [status] [valute] [premium valute] [experience] [date of registration] [time in game (min)\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "5"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: remove format:\nremove: [pool_name] [scheme_name] [collection_name]\nkey: [id] [game zone]\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "6"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: add pool format:\nadd pool: [pool_name] { [alloc_name] [size(only if needed)] [alloc_mode(only if exists)] }\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "7"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: delete pool format:\ndelete pool: [pool_name]\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "8"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: add scheme format:\nadd scheme: [pool_name] [scheme_name]\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "9"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: delete scheme format:\ndelete scheme: [pool_name] [scheme_name]\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "10"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: add collection format:\nadd collection: [pool_name] [scheme_name] [collection_name] {tree_name}\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "11"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: delete collection format:\ndelete collection: [pool_name] [scheme_name] [collection_name]\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }else if(command_action == "12"){
                        _logger->log("\n------------------------------------------\n\n(?) Format: save my commands:\nsave my commands: <file_path_to_save>\n\n~~~~~ Enter a command line ~~~~~\n\n~~~> Command line:", logger::severity::warning);
                    }
                    std::cin.clear();
                    getline(std::cin, command_line);

                    if(command_action == "1" || command_action == "4" || command_action == "3"){
                        _logger->log("\n\n~~~~~ Enter a key line ~~~~~\n\n~~~> Key line:", logger::severity::warning);
                        std::cin.clear();
                        getline(std::cin, key_line);
                        _logger->log("\n\n~~~~~ Enter a value line ~~~~~\n\n~~~> Value line(for read range enter second key):", logger::severity::warning);
                        std::cin.clear();
                        getline(std::cin, value_line);
                    }
                    else if(command_action == "2" || command_action == "5"){
                        _logger->log("\n\n~~~~~ Enter a key line ~~~~~\n\n~~~> Key line:", logger::severity::warning);
                        std::cin.clear();
                        getline(std::cin, key_line);
                        value_line = "";
                    }else{
                        key_line = "";
                        value_line = "";
                    }

                    try{
                        command_parsing(command_line, key_line, value_line);
                        _logger->log("\n------------------------------------------\n\n~~~~~ Command execution finished successfully! ~~~~~\n\nReturning...\n", logger::severity::warning);
                        continue;
                    }catch(const std::logic_error& ex){
                        std::string message = "\n\n";
                        message += ex.what();
                        message += "\n";
                        _logger->log(message, logger::severity::warning);
                        _logger->log("\n------------------------------------------\n\n~~~~~ Command execution failed! ~~~~~\n\nReturning...\n", logger::severity::warning);
                        continue;
                    }
                }else{
                    _logger->log("\n\nError: undefined command, returning...\n", logger::severity::warning);
                    continue;
                }

            }else if(action == "3"){

                _logger->log("\n------------------------------------------\n\n~~~~~ Resetting database... ~~~~~\n", logger::severity::warning);
                delete_database();
                continue;

            }else if(action == "4"){
                delete_database();
                _logger->log("\n------------------------------------------\n\n~~~~~ Enter a file's path ~~~~~\n\n~~~~~ Path:", logger::severity::warning);
                std::cin.clear();
                getline(std::cin, input);
                if(input.empty()){
                    _logger->log("\n\nError: empty path, returning...\n", logger::severity::warning);
                    continue;
                }
                try{
                    run_file_commands(input);
                    _logger->log("\n------------------------------------------\n\n~~~~~ Parsing commands finished successfully! ~~~~~\n1) Continue\n2) Exit\n\n~~~~~ Input:", logger::severity::warning);
                    std::string tmp_action;
                    std::cin.clear();
                    getline(std::cin, tmp_action);
                    if(tmp_action == "1"){
                        continue;
                    }else if(tmp_action == "2"){
                        _logger->log("\n------------------------------------------\n\n~~~~~ Exiting... ~~~~~\n", logger::severity::warning);
                        break;
                    }else{
                        _logger->log("\n\nError: undefined action, choosing 'continue' for safety...\n", logger::severity::warning);
                        continue;
                    }
                }catch(const std::logic_error& ex){
                    std::string tmp_action;
                    std::string message = "\n\n";
                    message += ex.what();
                    message += "\n";
                    _logger->log(message, logger::severity::warning);
                    _logger->log("\n------------------------------------------\n\n~~~~~ Parsing commands failed! ~~~~~\n\n1) Continue\n2) Reset and continue\n3) Exit\n\n~~~> Input:", logger::severity::warning);
                    std::cin.clear();
                    getline(std::cin, tmp_action);
                    if(tmp_action == "1") {
                        continue;
                    }else if(tmp_action == "2"){
                        _logger->log("\n------------------------------------------\n\n~~~~~ Continuing, Resetting... ~~~~~\n", logger::severity::warning);
                        delete_database();
                        continue;
                    }else if(tmp_action == "3"){
                        _logger->log("\n------------------------------------------\n\n~~~~~ Exiting... ~~~~~\n", logger::severity::warning);
                        break;
                    }else{
                        _logger->log("\nError: undefined action, choosing 'continue' for safety...\n", logger::severity::warning);
                        continue;
                    }
                }

            }else if(action == "5"){

                _logger->log("\n------------------------------------------\n\n~~~~~ Exiting... ~~~~~\n", logger::severity::warning);
                break;

            }
            else{

                _logger->log("\n\nError: undefined action, exiting...\n", logger::severity::warning);
                break;

            }
        }

        _logger->log("\n------------ Goodbye!---------------\n", logger::severity::warning);
    }


};



#endif //COURSEWORK_DATABASE_H
