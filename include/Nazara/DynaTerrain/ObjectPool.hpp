#ifndef OBJECTPOOL_HPP
#define OBJECTPOOL_HPP

#include <deque>
#include <list>

template <typename T> class NzObjectPool
{
    public:
        NzObjectPool(std::size_t chunkSize = 10);
        ~NzObjectPool() = default;

        T* GetObjectPtr();
        std::size_t GetPoolSize() const;
        void Grow();

        void ReleaseAll();
        void ReturnObjectPtr(T* pointer);

        void SetChunkSize(std::size_t chunkSize);

    protected:
    private:
    std::deque<T> m_objects;
    std::list<T*> m_freePointers;
    std::size_t m_chunkSize;
    std::size_t m_totalSize;
};

#include "ObjectPool.inl"

#endif // OBJECTPOOL_HPP
