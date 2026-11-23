#include <iostream>
#include "AsyncLogger.hpp"
#include <thread>
#include <vector>

int main() {

    AsyncLogger logger("application.log");

    for (int i = 0; i < 100; ++i) {
        logger.Log("Some message");
    }

    std::vector<std::thread> producers;

    for (int i = 0; i < 3; ++i) {
        producers.emplace_back([&logger]() {
            for (int j = 0; j < 100; ++j) {
                logger.Log("Some message from producers");
            }
        });
    }

    for (auto& t : producers)
        t.join();

    return 0;
}
