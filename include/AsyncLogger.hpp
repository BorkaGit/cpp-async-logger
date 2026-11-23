#pragma once

#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class AsyncLogger {
public:
    explicit AsyncLogger(const std::filesystem::path& filename);

    ~AsyncLogger();

    void Log(std::string message);

    AsyncLogger(const AsyncLogger&) = delete;
    AsyncLogger& operator=(const AsyncLogger&) = delete;

private:
    void WriterLoop();

    std::queue<std::string> m_logQueue;

    std::mutex m_mutex;
    std::condition_variable cv;

    bool finished = false;

    std::ofstream m_file;

    std::thread m_thread;
};