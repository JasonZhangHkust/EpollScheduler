#include <iostream>
#include "Scheduler.h"

void hello()
{
    std::cout << "Hello From Scheduler" << std::endl;
}
int main() {
    std::cout << "Hello, World!" << std::endl;
    Scheduler MyScheduler;
    MyScheduler.initialize();
    MyScheduler.runEvery(10, hello);
    MyScheduler.start();

    std::cin.get();

    return 0;
}