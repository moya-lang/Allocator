#ifndef AllocatorH
#define AllocatorH

#include <memory>

namespace Moya {

template <class T, std::size_t growSize>
class MemoryPool
{
    struct Block
    {
        Block *next;
    };

    class Buffer
    {
        static const std::size_t blockSize = sizeof(T) > sizeof(Block) ? sizeof(T) : sizeof(Block);
        uint8_t data[blockSize * growSize];

        public:

            Buffer *const next;

            Buffer(Buffer *next) :
                next(next)
            {
            }

            T *getBlock(std::size_t index)
            {
                return reinterpret_cast<T *>(&data[blockSize * index]);
            }
    };

    Block *firstFreeBlock = nullptr;
    Buffer *firstBuffer = nullptr;
    std::size_t bufferedBlocks = growSize;


    public:

        ~MemoryPool()
        {
            while (firstBuffer) {
                Buffer *buffer = firstBuffer;
                firstBuffer = buffer->next;
                delete buffer;
            }
        }

        T *allocate()
        {
            if (firstFreeBlock) {
                Block *block = firstFreeBlock;
                firstFreeBlock = block->next;
                return reinterpret_cast<T *>(block);
            }

            if (bufferedBlocks >= growSize) {
                firstBuffer = new Buffer(firstBuffer);
                bufferedBlocks = 0;
            }

            return firstBuffer->getBlock(bufferedBlocks++);
        }

        void deallocate(T *pointer)
        {
            Block *block = reinterpret_cast<Block *>(pointer);
            block->next = firstFreeBlock;
            firstFreeBlock = block;
        }
};

template <class T, std::size_t growSize>
class Allocator
{
    MemoryPool<T, growSize> memoryPool;
    std::allocator<T> *defaultAllocator = nullptr;

    Allocator(Allocator &&allocator);
    Allocator(const Allocator &allocator);
    Allocator &operator =(Allocator &&allocator);
    Allocator &operator =(const Allocator &allocator);


    public:

        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef T value_type;

        template <class U>
        struct rebind
        {
            typedef Allocator<U, growSize> other;
        };

        Allocator()
        {
        }

        template <class U>
        Allocator(const Allocator<U, growSize> &other)
        {
            if (!std::is_same<T, U>::value)
                defaultAllocator = new std::allocator<T>();
        }

        ~Allocator()
        {
            delete defaultAllocator;
        }

        pointer allocate(size_type n, std::allocator<void>::const_pointer hint = 0)
        {
            if (defaultAllocator)
                return defaultAllocator->allocate(n, hint);

            if (n != 1 || hint)
                throw std::bad_alloc();

            return memoryPool.allocate();
        }

        void deallocate(pointer p, size_type n)
        {
            if (defaultAllocator)
                defaultAllocator->deallocate(p, n);

            else
                memoryPool.deallocate(p);
        }

        void construct(pointer p, const_reference val)
        {
            new (p) T(val);
        }

        void destroy(pointer p)
        {
            p->~T();
        }
};

}

#endif
