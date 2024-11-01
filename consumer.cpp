//
//
// Consumer
//
// g++ -pthread -lrt consumer.cpp -o consumer

#include <sys/mman.h>   
#include <sys/stat.h>   
#include <fcntl.h>      
#include <unistd.h>     
#include <iostream>
#include <semaphore.h>
#include <ctime>
#include <cstdlib>

int main() {

    // Setting up semaphore names
    const char* emptySemName = "/empty_slots";
    const char* fullSemName = "/full_slots";
    const char* mutexSemName = "/mutex";

    ///////////////////////////////////////////////////////////////////////////////////
    //
    // Setting Up Shared Memory
    //
    ///////////////////////////////////////////////////////////////////////////////////

    const char* sharedMemName = "/shared_table";
    const size_t sharedMemSize = sizeof(int) * 2;  // Table with 2 slots

    // Access shared memory object
    int shm_fd = shm_open(sharedMemName, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to create shared memory object" << std::endl;
        return 1;
    }

    // Map shared memory
    int* table = static_cast<int*>(mmap(0, sharedMemSize, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (table == MAP_FAILED) {
        std::cerr << "Mapping failed" << std::endl;
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////////////
    //
    // Finished Setting Up Shared Memory
    //
    ///////////////////////////////////////////////////////////////////////////////////

    // Open semaphores (created in producer.cpp)
    sem_t* empty = sem_open(emptySemName, 0);
    sem_t* full = sem_open(fullSemName, 0);
    sem_t* mutex = sem_open(mutexSemName, 0);

    while (true) {
        sem_wait(full);
        sem_wait(mutex);

        if (table[0] != 0) {
            int item = table[0];
            table[0] = 0;
            std::cout << "Consumed: " << item << std::endl;
        } else if (table[1] != 0) {
            int item = table[1];
            table[1] = 0;
            std::cout << "Consumed: " << item << std::endl;
        }

        sem_post(mutex);
        sem_post(empty);

        sleep(1); // extra delay of 1 second to not blow up console so fast
    }







    // Clean up shared memory
    munmap(table, sharedMemSize);

    // Clean up semaphores
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);

    return 0;
}
