#include "database.hpp"
#include "scenarios.hpp"
#include <exception>
#include <iostream>

int main() {
    try {
        initializeDatabase();
        seedDatabase();

        std::cout << "VIDEO SURVEILLANCE - LAB 8, VARIANT 12\n";
        std::cout << "Seed: cameras=" << getStorage().count<Camera>()
            << ", events=" << getStorage().count<RegisteredEvent>() << "\n";

        runSchemaDemo();        // Завдання 3
        runCompareDemo();       // Завдання 2
        runCrudDemo();          // Завдання 4
        runQueryDemo();         // Завдання 5
        runTransactionDemo();   // Завдання 6

        std::cout << "\nPROGRAM FINISHED\n";
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
    return 0;
}