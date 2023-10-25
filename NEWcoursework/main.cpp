//This is Server
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sstream>
#include "database/database.h"
#define SEMAPHORE_KEY 54482
using namespace std;

#ifdef __linux__
#include <sys/sem.h>

int semId;

void file_mapping(){
    int file_descriptor;
    const int file_length = 117;
    const char* path = "../file_for_IPC.txt";

    file_descriptor = open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (file_descriptor == -1){
        perror("Problem with opening file");
        exit(1);
    }

    lseek(file_descriptor, file_length - 1, SEEK_SET);
    write(file_descriptor, "", 1);

    char* adr = (char*)mmap(NULL, file_length, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);
    if (adr == MAP_FAILED){
        perror("Problem with mmap");
        close(file_descriptor);
        exit(1);
    }

    auto* builder = new logger_builder_concrete();
    logger* logger = builder->add_stream("console", logger::severity::information)->construct();

    auto* my_db = new database(logger);


    std::cout << "server started" << endl;
    semId = semget(SEMAPHORE_KEY, 2, IPC_CREAT | 0666);
    if (semId == -1) {
        perror("semget");
        exit(1);
    }
    close(file_descriptor);


    auto* semOps = new sembuf[2];
    semOps[0].sem_num = 0;
    semOps[0].sem_op = 0;
    semOps[0].sem_flg = 0;
    semOps[1].sem_num = 1;
    semOps[1].sem_op = 1;
    semOps[1].sem_flg = 0;
    semop(semId, semOps, 2);

    int num_of_clients = 0;
    while (true) {
        std::cout << "server waiting client" << endl;
        semOps[0].sem_op = -1;
        semOps[1].sem_op = 0;
        semop(semId, semOps, 2);

        std::string mess = adr;
//        if (mess == "New client was added"){
//            num_of_clients++;
//            std::cout << mess << "- quantity of all clients: "<< num_of_clients << std::endl;
//            semOps[0].sem_op = 0;
//            semOps[1].sem_op = 1;
//            semop(semId, semOps, 2);
//            continue;
//        }
//        if (mess == "Exit"){
//            num_of_clients--;
//            if (num_of_clients == 0){
//                std::cout << mess << std::endl;
//                break;
//            }
//            semOps[0].sem_op = 0;
//            semOps[1].sem_op = 1;
//            semop(semId, semOps, 2);
//            continue;
//        }


        std::string comand ;
        std::string key ;
        std::string value ;

        stringstream stream_str;
        std::string word;
        stream_str << mess;
        int count = 0;
        while (std::getline(stream_str, word)){
            if (count == 0) comand = word;
            else if (count == 1) key = word;
            else if (count == 2) value = word;
            count++;
        }

        /*std::cout << "comand: " << comand <<std::endl;
        std::cout << "key: " << key << std::endl;
        std::cout << "value: " << value << std::endl;*/

        try{
            my_db->command_parsing(comand, key, value);
        }catch(std::logic_error &err){
            logger->log(err.what(), logger::severity::warning);
        }


        std::cout << "server reading data, writing result" << endl;

        //sprintf(adr, "%s", "5 minut - polet normalniy");
        semOps[0].sem_op = 1;
        semOps[1].sem_op = 1;
        semop(semId, semOps, 2);


    }

    delete[] semOps;
    delete my_db;
    delete logger;
    delete builder;
}
void clear(){
    semctl(semId, 0, IPC_RMID, 0);
}

#endif


int main() {
    atexit(clear);
    file_mapping();
}
