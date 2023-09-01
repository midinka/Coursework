#include <iostream>
#include "database.h"


int main(int argc, char* argv[]){
    auto* builder = new logger_builder_concrete();
    logger* logger = builder->add_stream("console", logger::severity::information)->construct();

    auto* my_db = new database(logger);

    try{
        my_db->dialog();
    } catch(const std::logic_error& ex){
        std::cout << ex.what() << std::endl;
    }

    delete my_db;
    delete logger;
    delete builder;


}
