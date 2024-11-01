//
//
// Producer
//
// g++ -pthread -lrt producer.cpp -o producer

#include <sys/mman.h>   
#include <sys/stat.h>   
#include <fcntl.h>      
#include <unistd.h>     
#include <iostream>
#include <semaphore.h>
#include <ctime>
#include <cstdlib>

int main() {
    // Set seed
    srand(time(nullptr));

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

    // Create shared memory object
    int shm_fd = shm_open(sharedMemName, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to create shared memory object" << std::endl;
        return 1;
    }

    // Set size of shared memory
    if (ftruncate(shm_fd, sharedMemSize) == -1) {
        std::cerr << "Failed to set size" << std::endl;
        return 1;
    }

    // Map shared memory
    int* table = static_cast<int*>(mmap(0, sharedMemSize, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (table == MAP_FAILED) {
        std::cerr << "Mapping failed" << std::endl;
        return 1;
    }

    // Initialize shared memory
    table[0] = 0;  // First item slot
    table[1] = 0;  // Second item slot

    ///////////////////////////////////////////////////////////////////////////////////
    //
    // Finished Setting Up Shared Memory
    //
    ///////////////////////////////////////////////////////////////////////////////////

    // Initialize Semaphores
    sem_t* empty = sem_open(emptySemName, O_CREAT, 0666, 2);  // 2 empty slots 
    sem_t* full = sem_open(fullSemName, O_CREAT, 0666, 0);    // No full slots
    sem_t* mutex = sem_open(mutexSemName, O_CREAT, 0666, 1);  // Init to 1 bc producer access SM first

    int item = 0;
    while(true) {
        item = (rand() % 100) + 1; //generates a random num between 1 and 100
        sem_wait(empty);
        sem_wait(mutex);

        if (table[0] == 0) {
            table[0] = item;
            std::cout << "Produced: " << item << std::endl;
        } else if (table[1] == 0) {
            table[1] = item;
            std::cout << "Produced: " << item << std::endl;
        }

        sem_post(mutex);
        sem_post(full);
    }







    // Clean up shared memory
    munmap(table, sharedMemSize);
    shm_unlink(sharedMemName);

    // Clean up semaphores
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    sem_unlink(emptySemName);
    sem_unlink(fullSemName);
    sem_unlink(mutexSemName);

    return 0;
}
