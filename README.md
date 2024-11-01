# Producer Consumer Project

This project consists of two programs. One program, producer.cpp, produces a random number
into shared memory. The other program, consumer.cpp, consumes the number out of shared memory
by printing the number to the console and then setting it to 0 in shared memory. The consumer 
also has a simulated 1 second delay in order to keep the console output manageable. The programs
use three semaphores to ensure synchrony. One semaphore for mutual exclusion, one for tracking
the full slots in shared memory, and one for tracking the empty slots in shared memory.

## Example Output

Produced: 59
Consumed: 59
Produced: 52
Consumed: 52
Produced: 51
Consumed: 51
Produced: 83
Consumed: 83

## Commands to Compile and Run

g++ -pthread -lrt producer.cpp -o producer

g++ -pthread -lrt consumer.cpp -o consumer

./producer & ./consumer &

## Commands to Stop the Programs

pkill producer

pkill consumer

