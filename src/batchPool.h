#pragma once

//std
#include <vector>
#include <cstdint>

template<typename Type, std::size_t BatchSize = 100>
struct BatchPool {
private:
    std::vector<std::vector<Type>> batchs;
    std::vector<Type>* activeBatch;
public:
    BatchPool() : batchs(), activeBatch(nullptr) {}
    ~BatchPool() = default;
public:
    std::size_t capacity() const { return BatchSize * batchs.size(); }
    std::size_t size() const { return batchs.size() == 0 ? 0 : ((batchs.size() - 1) * BatchSize) + activeBatch->size(); }
    static std::size_t batchSize() { return BatchSize; }
    Type* newElement() {
        if (needNewBatch()) {
            newBatch();
        }
        activeBatch->resize(activeBatch->size() + 1);
        return &activeBatch->back();
    };
    void clear() {
        batchs.clear();
        activeBatch = nullptr;
    }
private:
    void newBatch() {
        batchs.resize(batchs.size() + 1);
        activeBatch = &batchs.back();
        activeBatch->reserve(BatchSize);
    }
    bool needNewBatch() {
        return activeBatch ? activeBatch->size() == activeBatch->capacity() : true;
    }
};