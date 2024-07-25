#pragma once

#include <condition_variable>
#include <optional>
#include <queue>

namespace RAYX {

template <typename T>
class BlockingQueue {
    using Queue = std::queue<T>;

  public:
    using container_type = Queue::container_type;
    using value_type = Queue::value_type;
    using size_type = Queue::size_type;
    using reference = Queue::reference;
    using const_reference = Queue::const_reference;

    ~BlockingQueue() { releaseWaiting(); }

    void releaseWaiting() {
        const auto lock = std::lock_guard<std::mutex>(m_mutex);
        m_queueNotEmptyCv.notify_all();
        m_queue = {};  // clear queue
    }

    reference front() {
        const auto lock = std::lock_guard<std::mutex>(m_mutex);
        return m_queue.front();
    }

    const_reference front() const {
        const auto lock = std::lock_guard<std::mutex>(m_mutex);
        return m_queue.front();
    }

    bool empty() const {
        const auto lock = std::lock_guard<std::mutex>(m_mutex);
        return m_queue.empty();
    }

    size_type size() const {
        const auto lock = std::lock_guard<std::mutex>(m_mutex);
        return m_queue.size();
    }

    void push(const value_type& value) {
        const auto lock = std::lock_guard<std::mutex>(m_mutex);
        m_queueNotEmptyCv.notify_one();
        m_queue.push(value);
    }

    void push(value_type&& value) {
        const auto lock = std::lock_guard<std::mutex>(m_mutex);
        m_queueNotEmptyCv.notify_one();
        m_queue.push(std::move(value));
    }

    template <typename... Args>
    decltype(auto) emplace(Args&&... args) {
        const auto lock = std::lock_guard<std::mutex>(m_mutex);
        m_queueNotEmptyCv.notify_one();
        return m_queue.emplace(std::forward<Args>(args)...);
    }

    void swap(BlockingQueue& other) noexcept(std::is_nothrow_swappable_v<container_type>) {
        auto lock_this = std::unique_lock<std::mutex>(m_mutex, std::defer_lock);
        auto lock_other = std::unique_lock<std::mutex>(other.m_mutex, std::defer_lock);
        std::lock(lock_this, lock_other);
        m_queue.swap(other.m_queue);
    }

    std::optional<T> pop_wait() {
        auto lock = std::unique_lock<std::mutex>(m_mutex);
        if (m_queue.empty()) m_queueNotEmptyCv.wait(lock);
        if (m_queue.empty()) return std::nullopt;
        auto value = std::move(m_queue.front());
        m_queue.pop();
        return value;
    };

    template <typename Rep, typename Period>
    std::optional<T> pop_wait_for(const std::chrono::duration<Rep, Period>& duration) {
        auto lock = std::unique_lock<std::mutex>(m_mutex);
        if (m_queue.empty()) {
            const auto status = m_queueNotEmptyCv.wait_for(lock, duration);
            if (status == std::cv_status::timeout || m_queue.empty()) return std::nullopt;
        }
        auto value = std::move(m_queue.front());
        m_queue.pop();
        return value;
    }

  protected:
    Queue m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_queueNotEmptyCv;
};

}  // namespace RAYX
