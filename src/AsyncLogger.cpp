#include "AsyncLogger.hpp"
#include <stdexcept>
#include <utility>

AsyncLogger::AsyncLogger(const std::filesystem::path& filename) {
    m_file.open(filename, std::ios::app);
    if (!m_file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    m_thread = std::thread(&AsyncLogger::WriterLoop, this);
}

AsyncLogger::~AsyncLogger() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        finished = true;
    }

    cv.notify_one();

    m_thread.join();
}

void AsyncLogger::Log(std::string message) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (finished) {
            throw std::runtime_error("Logger is finished");
        }

        m_logQueue.push(std::move(message));
    }

    cv.notify_one();
}

void AsyncLogger::WriterLoop() {
    while (true) {
        std::unique_lock<std::mutex> lock(m_mutex);

        cv.wait(lock, [&]() {
            return !m_logQueue.empty() || finished;
        });

        if (m_logQueue.empty()) {
            return;
        }

        std::string string = std::move(m_logQueue.front());
        m_logQueue.pop();

        lock.unlock();

        m_file << string << '\n';
    }
}
