#pragma once
#pragma pack(16)

#include <list>
#include <vector>

#define DEBUG_BUF_DOUBLE_COUNT 16

namespace RAYX {
template <typename T>
class DebugBufList {
  public:
    DebugBufList();
    ~DebugBufList();
    void insertVector(std::vector<T>&& inDebugVector);
    void clean();

    std::list<std::vector<T>>::iterator begin();
    std::list<std::vector<T>>::iterator end();

    std::vector<T> back();
    std::size_t size();
    int getAmount() const;

  private:
    std::list<std::vector<T>> m_List;
};
}  // namespace RAYX