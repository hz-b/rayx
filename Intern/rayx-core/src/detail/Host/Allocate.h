#pragma once

template <typename TAccDev>
class BufPool {
  public:
  private:
    std::vector<alpaka::Buf<TAccDev, void*, alpaka::DimInt<1>, int>> m_free;
};

template <typename T>
class Allocation {
    friend class Allocator;

  public:
    ~Allocation() {
        if (m_p && m_allocator && size > 0) m_allocator->deallocate(*this);
    }

    size_t size() const { return m_size; }

    T* get() { return m_p; }
    const T* get() const { return m_p; }
    T& operator*() { return m_p; }
    const T& operator*() const { return m_p; }

  private:
    Allocator* m_allocator;
    size_t m_size;
    T* m_p;
};

// Simple memory allocator that reuses freed blocks. Can be replaced by a Red-Black tree or similar for better performance.
class Allocator {
  public:
    template <typename T>
    Allocation<T> allocate(size_t size) {
        if (auto freeBlockIndex = findFreeBlockIndex(size); freeBlockIndex) {
            Block block = m_free[*freeBlockIndex];
            m_free.erase(m_free.begin() + *freeBlockIndex);
            return Allocation<T>{.m_allocator = this, .m_size = block.size, .m_p = static_cast<T*>(block.p)};
        } else {
            auto* p = new T[size];
            return Allocation<T>{this, p};
        }
    }

    template <typename T>
    void deallocate(Allocation<T>& allocation) {
        m_free.push_back(.p = Block{.size = allocation.m_size, .p = allocation.m_p});
        allocation.m_p    = nullptr;
        allocation.m_size = 0;
    }

    template <typename T>
    queueDeallocate(Allocation<T>& allocation) {
        // TODO: use cuda callback to free
    }

  private:
    struct Block {
        size_t size = 0;
        void* p;
    };

    std::optional<int> findFreeBlockIndex(size_t size) {
        for (size_t i = 0; i < m_free.size(); ++i) {
            if (m_free[i].size >= size) return static_cast<int>(i);
        }
        return std::nullopt;
    }

    std::vector<Block> m_free;
};
