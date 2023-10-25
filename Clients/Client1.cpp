#include <iostream>
#include <cstdio>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "/home/dina/CLionProjects/NEWcoursework/logger/logger_builder_concrete.h"
#define SEMAPHORE_KEY 54482
#define SEMAPHORE_COUNT 2
using namespace std;

#ifdef __linux__
#include <sys/sem.h>


#endif
void dialog(logger* _logger){
    int file_descriptor;
    const int file_length = 100;
    const char* path = "/home/dina/CLionProjects/NEWcoursework/file_for_IPC.txt";

    file_descriptor = open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (file_descriptor == -1){
        perror("Problem with opening file");
        exit(1);
    }


    char* adr = (char*)(mmap(NULL, file_length, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0));
    if (adr == MAP_FAILED){
        perror("Problem with mmap");
        close(file_descriptor);
        exit(1);
    }
    close(file_descriptor);

    std::cout << "client started" << endl;
    int semId = semget(SEMAPHORE_KEY, SEMAPHORE_COUNT,  0666);
    if (semId == -1) {
        perror("semget");
        exit(1);
    }
    auto* semOps = new sembuf[2];
//    semOps[0].sem_num = 0;
//    semOps[0].sem_op = 0;
//    semOps[0].sem_flg = 0;
//    semOps[1].sem_num = 1;
//    semOps[1].sem_op = -1;
//    semOps[1].sem_flg = 0;
//    semop(semId, semOps, 2);
      std::string mess;
//    sprintf(adr, "%s", mess.c_str());
//    semOps[0].sem_op = 1;
//    semOps[1].sem_op = 0;
//    semop(semId, semOps, 2);

    _logger->log("\n\n-------- Welcome! I'm doing my best! --------\n\n", logger::severity::warning);
    std::string action;
    std::string input;
    std::string answer_from_server;

    while(action != "5"){
        semOps[0].sem_num = 0;
        semOps[0].sem_op = 0;
        semOps[0].sem_flg = 0;
        semOps[1].sem_num = 1;
        semOps[1].sem_op = -1;
        semOps[1].sem_flg = 0;
        semop(semId, semOps, 2);

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
                std::ifstream file_with_commands;
                file_with_commands.open(input);
                if(!file_with_commands.is_open()){
                    throw std::logic_error("Could not open a file!");
                }

                std::string line;
                std::string line2;
                std::string line3;
                while(getline(file_with_commands, line)){
                    if(line.find("insert: ") == 0 || line.find("update key: ") == 0 || line.find("read range: ") == 0){
                        if(!getline(file_with_commands, line2)){
                            file_with_commands.close();
                            throw std::logic_error("Wrong format of a command! (no key)");
                        }
                        if(!getline(file_with_commands, line3)){
                            file_with_commands.close();
                            throw std::logic_error("Wrong format of a command! (no value)");
                        }
                    }else if(line.find("read key: ") == 0 || line.find("remove: ") == 0 ){
                        if(!getline(file_with_commands, line2)){
                            file_with_commands.close();
                            throw std::logic_error("Wrong format of a command!(no key)");
                        }

                    }else if(line.find("add pool: ") == 0 || line.find("delete pool: ") == 0 || line.find("add scheme: ") == 0 || line.find("delete scheme: ") == 0 || line.find("add collection: ") == 0 || line.find("delete collection: ") == 0 || line.find("reset database:") == 0 || line.find("save my commands:") == 0){

                    }
                    else{
                        file_with_commands.close();
                        throw std::logic_error("Wrong format of a command!");
                    }
                    try{
                        mess = line;
                        mess += '\n';
                        mess += line2;
                        mess += '\n';
                        mess += line3;
                        //std::cout<<mess<<std::endl;
                        sprintf(adr, "%s", mess.c_str());


                        semOps[0].sem_op = 1;
                        semOps[1].sem_op = 0;
                        semop(semId, semOps, 2);

                        semOps[0].sem_op = -1;
                        semOps[1].sem_op = -1;
                        semop(semId, semOps, 2);

                        answer_from_server = adr;
                        //std::cout<<answer_from_server<<std::endl;

                    } catch(const std::logic_error& ex){
                        file_with_commands.close();
                        throw ex;
                    }
                }
                _logger->log("\n------------------------------------------\n\n~~~~~ Parsing commands finished successfully! ~~~~~n\n1) Continue\n2) Exit\n\n~~~~~ Input:", logger::severity::warning);
                std::string tmp_action;
                std::cin.clear();
                getline(std::cin, tmp_action);
                if(tmp_action == "1"){
                    semOps[0].sem_op = 0;
                    semOps[1].sem_op = 1;
                    semop(semId, semOps, 2);
                    continue;
                }else if(tmp_action == "2"){
                    /*semOps[0].sem_op = 1;
                    semOps[1].sem_op = 0;
                    semop(semId, semOps, 2);
                    sprintf(adr, "%s", "Exit");
                    semOps[0].sem_op = -1;
                    semOps[1].sem_op = -1;
                    semop(semId, semOps, 2);
                    answer_from_server = adr;*/

                    semOps[0].sem_op = 0;
                    semOps[1].sem_op = 1;
                    semop(semId, semOps, 2);
                    _logger->log("\n------------------------------------------\n\n~~~~~ Exiting... ~~~~~\n", logger::severity::warning);
                    break;
                }else{
                    semOps[0].sem_op = 0;
                    semOps[1].sem_op = 1;
                    semop(semId, semOps, 2);
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
                    semOps[0].sem_op = 0;
                    semOps[1].sem_op = 1;
                    semop(semId, semOps, 2);
                    continue;
                }else if(tmp_action == "2"){
                    _logger->log("\n------------------------------------------\n\n~~~~~ Continuing, Resetting... ~~~~~\n", logger::severity::warning);
                    //delete_database();
                    continue;
                }else if(tmp_action == "3"){
                    /*semOps[0].sem_op = 1;
                    semOps[1].sem_op = 0;
                    semop(semId, semOps, 2);
                    sprintf(adr, "%s", "Exit");
                    semOps[0].sem_op = -1;
                    semOps[1].sem_op = -1;
                    semop(semId, semOps, 2);
                    answer_from_server = adr;*/

                    semOps[0].sem_op = 0;
                    semOps[1].sem_op = 1;
                    semop(semId, semOps, 2);
                    _logger->log("\n------------------------------------------\n\n~~~~~ Exiting... ~~~~~\n", logger::severity::warning);
                    break;
                }else{
                    _logger->log("\nError: undefined action, choosing 'continue' for safety...\n", logger::severity::warning);
                    semOps[0].sem_op = 0;
                    semOps[1].sem_op = 1;
                    semop(semId, semOps, 2);
                    continue;
                }
            }

        }else if(action == "2"){

            std::string command_action;

            std::string command_line;
            std::string key_line;
            std::string value_line;

            _logger->log("\n------------------------------------------\n\n~~~~~ Choose a command ~~~~~\n\n1) insert\n2) read key\n3) read range\n4) update key\n5) remove\n6) add pool\n7) delete pool\n8) add scheme\n9) delete scheme\n10) add collection\n11) delete collection\n\n~~~> Command:", logger::severity::warning);
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
                    mess = command_line;
                    mess += '\n';
                    mess += key_line;
                    mess += '\n';
                    mess += value_line;

                    sprintf(adr, "%s", mess.c_str());
                    semOps[0].sem_op = 1;
                    semOps[1].sem_op = 0;
                    semop(semId, semOps, 2);

                    semOps[0].sem_op = -1;
                    semOps[1].sem_op = -1;
                    semop(semId, semOps, 2);

                    answer_from_server = adr;

                    semOps[0].sem_op = 0;
                    semOps[1].sem_op = 1;
                    semop(semId, semOps, 2);

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
                semOps[0].sem_op = 0;
                semOps[1].sem_op = 1;
                semop(semId, semOps, 2);
                _logger->log("\n\nError: undefined command, returning...\n", logger::severity::warning);
                continue;
            }

        }else if(action == "3"){
            semOps[0].sem_op = 1;
            semOps[1].sem_op = 0;
            semop(semId, semOps, 2);
            sprintf(adr, "%s", "reset database:");
            semOps[0].sem_op = -1;
            semOps[1].sem_op = -1;
            semop(semId, semOps, 2);
            answer_from_server = adr;

            semOps[0].sem_op = 0;
            semOps[1].sem_op = 1;
            semop(semId, semOps, 2);
            _logger->log("\n------------------------------------------\n\n~~~~~ Resetting database... ~~~~~\n", logger::severity::warning);
            //delete_database();
            continue;

        }else if(action == "4"){
            //delete_database();
            _logger->log("\n------------------------------------------\n\n~~~~~ Enter a file's path ~~~~~\n\n~~~~~ Path:", logger::severity::warning);
            std::cin.clear();
            getline(std::cin, input);
            if(input.empty()){
                _logger->log("\n\nError: empty path, returning...\n", logger::severity::warning);
                continue;
            }
            try{
                //run_file_commands(input);
                mess = "restore database:";
                mess += "\n";
                mess += input;
                std::cout << mess << std::endl;
                sprintf(adr, "%s", mess.c_str());
                semOps[0].sem_op = 1;
                semOps[1].sem_op = 0;
                semop(semId, semOps, 2);

                semOps[0].sem_op = -1;
                semOps[1].sem_op = -1;
                semop(semId, semOps, 2);
                answer_from_server = adr;

                //semOps[0].sem_op = 0;
                //semOps[1].sem_op = 1;
                //semop(semId, semOps, 2);
                _logger->log("\n------------------------------------------\n\n~~~~~ Parsing commands finished successfully! ~~~~~n\n1) Continue\n2) Exit\n\n~~~~~ Input:", logger::severity::warning);
                std::string tmp_action;
                std::cin.clear();
                getline(std::cin, tmp_action);
                if(tmp_action == "1"){
                    semOps[0].sem_op = 0;
                    semOps[1].sem_op = 1;
                    semop(semId, semOps, 2);
                    continue;
                }else if(tmp_action == "2"){
                    /*sprintf(adr, "%s", "Exit");
                    semOps[0].sem_op = 1;
                    semOps[1].sem_op = 0;
                    semop(semId, semOps, 2);

                    semOps[0].sem_op = -1;
                    semOps[1].sem_op = -1;
                    semop(semId, semOps, 2);
                    answer_from_server = adr;*/

                    semOps[0].sem_op = 0;
                    semOps[1].sem_op = 1;
                    semop(semId, semOps, 2);
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
                    //delete_database();
                    continue;
                }else if(tmp_action == "3"){
                    /*semOps[0].sem_op = 1;
                    semOps[1].sem_op = 0;
                    semop(semId, semOps, 2);
                    sprintf(adr, "%s", "Exit");
                    semOps[0].sem_op = -1;
                    semOps[1].sem_op = -1;
                    semop(semId, semOps, 2);
                    answer_from_server = adr;*/

                    semOps[0].sem_op = 0;
                    semOps[1].sem_op = 1;
                    semop(semId, semOps, 2);
                    _logger->log("\n------------------------------------------\n\n~~~~~ Exiting... ~~~~~\n", logger::severity::warning);
                    break;
                }else{
                    _logger->log("\nError: undefined action, choosing 'continue' for safety...\n", logger::severity::warning);
                    continue;
                }
            }

        }else if(action == "5"){
//            semOps[0].sem_op = 1;
//            semOps[1].sem_op = 0;
//            semop(semId, semOps, 2);
//            sprintf(adr, "%s", "Exit");
//            semOps[0].sem_op = -1;
//            semOps[1].sem_op = -1;
//            semop(semId, semOps, 2);
//            answer_from_server = adr;

            semOps[0].sem_op = 0;
            semOps[1].sem_op = 1;
            semop(semId, semOps, 2);
            _logger->log("\n------------------------------------------\n\n~~~~~ Exiting... ~~~~~\n", logger::severity::warning);
            break;

        }
        else{
            /*semOps[0].sem_op = 1;
            semOps[1].sem_op = 0;
            semop(semId, semOps, 2);
            sprintf(adr, "%s", "Exit");
            semOps[0].sem_op = -1;
            semOps[1].sem_op = -1;
            semop(semId, semOps, 2);
            answer_from_server = adr;*/
            semOps[0].sem_op = 0;
            semOps[1].sem_op = 1;
            semop(semId, semOps, 2);
            _logger->log("\n\nError: undefined action, exiting...\n", logger::severity::warning);
            break;

        }
    }

    _logger->log("\n------------ Goodbye!---------------\n", logger::severity::warning);
}


int main() {
    auto* builder = new logger_builder_concrete();
    logger* logger = builder->add_stream("console", logger::severity::information)->construct();
    dialog(logger);
}

