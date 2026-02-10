#pragma once

template <typename TQueue>
struct isQueue : std::false_type {};

template <typename TAccDev, typename TBlocking>
struct isQueue<alpaka::Queue<TAccDev, TBlocking>> : std::true_type {};

template <typename T>
constexpr bool isQueue_v = IsQueue<T>::value;

template <typename T>
concept Queue_c = isQueue_v<T>;

template <Queue_c TQueue>
class Qus {
    friend class QueuePool;
    static constexpr int numQueues = 4;

  public:
    using Queue = TQueue;

    Qus()                      = delete;
    Qus(const Qus&)            = delete;
    Qus& operator=(const Qus&) = delete;
    Qus(Qus&&)                 = default;
    Qus& operator=(Qus&&)      = default;

    Q getQueue() const {
        static int index = 0;
        index            = (index + 1) % numQueues;
        return m_queues[index];
    }

    void waitForQus(const Qus& other) const {
        for (const auto& q : m_queues) {
            for (const auto& otherQueue : other.m_queues) { queueWait(q, otherQueue); }
        }
    }

    template <typename... OtherQus>
    void waitForQus(const OtherQus&... others) const {
        (waitForQus(others), ...);
    }

  private:
    class QueuePool& m_pool;
    std::array<Queue, numQueues> m_queues;
};

template <typename T>
struct isQus : std::false_type {};

template <Queue_c TQueue>
struct isQus<Qus<TQueue>> : std::true_type {};

template <typename T>
constexpr bool isQus_v = isQus<T>::value;

template <typename T>
concept Qus_c = isQus_v<T>;

template <Queue_c TQueue>
class QueuePool {
  public:
    using Queue = TQueue;
    using Qus   = Qus<Queue>;

    Queue getQueue() {
        if (m_free.empty()) {
            m_busy.push_back(alpaka::createQueue<Queue>());
            return m_busy.back();
        }

        Queue q = m_free.back();
        m_free.pop_back();
        m_busy.push_back(q);
        return q;
    }

    Qus getQus() {
        std::array<Queue, Qus::numQueues> queues;
        for (int i = 0; i < Qus::numQueues; ++i) queues[i] = getQueue();

        return Qus { .m_pool = *this, .m_queues = queues; };
    }

    void enqueueWaitTo(Queue q) {
        for (const auto& queue : m_busy) {
            alpaka::wait(q, queue);
            m_free.push_back(queue);
        }
    }

  private:
    std::vector<Queue> m_busy;
    std::vector<Queue> m_free;
};
