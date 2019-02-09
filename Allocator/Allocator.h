#ifndef AllocatorH
#define AllocatorH

#include <memory>

#include "MemoryPool.h"

namespace Moya {

template <class T, std::size_t growSize = 1024>
class Allocator : private MemoryPool<T, growSize>
{
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

        pointer allocate(size_type n, const void *hint = 0)
        {
            if (n != 1 || hint)
                throw std::bad_alloc();

            return MemoryPool<T, growSize>::allocate();
        }

        void deallocate(pointer p, size_type n)
        {
            MemoryPool<T, growSize>::deallocate(p);
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
