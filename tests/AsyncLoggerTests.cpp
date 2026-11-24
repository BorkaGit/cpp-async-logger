#include <gtest/gtest.h>
#include "AsyncLogger.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <stdexcept>
#include <utility>

static std::vector<std::string> ReadLines(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open log file");
    }

    std::string line;
    std::vector<std::string> lines;

    while (std::getline(file, line)) {
        lines.push_back(std::move(line));
    }

    return lines;
}

TEST(AsyncLoggerTests, WritesSingleMessage) {

    const std::string expected_line = "single_message_line";

    const auto path = std::filesystem::temp_directory_path()
              / "async_logger_single_test.log";

    std::filesystem::remove(path);
    {
        AsyncLogger logger(path);

        logger.Log(expected_line);
    }

    const std::vector<std::string> lines = ReadLines(path);

    ASSERT_EQ(lines.size(), 1);
    EXPECT_EQ(lines[0], expected_line);
}

TEST(AsyncLoggerTests, WritesMultipleMessages)
{
    const auto path = std::filesystem::temp_directory_path()
            / "async_logger_multiple_test.log";

    std::filesystem::remove(path);

    {
        AsyncLogger logger(path);

        for (int i = 0; i < 100; ++i) {
            logger.Log("Hello world");
        }
    }

    const std::vector<std::string> lines = ReadLines(path);

    EXPECT_EQ(lines.size(), 100);
}

TEST(AsyncLoggerTests, ConcurrentLogging)
{
    const auto path = std::filesystem::temp_directory_path()
          / "async_logger_concurrent_test.log";

    std::filesystem::remove(path);
    {
        AsyncLogger logger(path);

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
    }

    const std::vector<std::string> lines = ReadLines(path);

    EXPECT_EQ(lines.size(), 300);
}

TEST(AsyncLoggerTests, DrainsQueueOnDestruction)
{
    const auto path = std::filesystem::temp_directory_path()
         / "async_logger_drains_queue_on_destruction_test.log";

    std::filesystem::remove(path);

    {
        AsyncLogger logger(path);

        for (int i = 0; i < 10000; ++i) {
            logger.Log("Hello world");
        }
    }

    const std::vector<std::string> lines = ReadLines(path);

    EXPECT_EQ(lines.size(), 10000);
}