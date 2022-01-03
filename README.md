# Allocator

Fast C++11 allocator for STL containers.


## Foreword

Presented solution is a special purpose allocator designed to increase performance of STL containers. This allocator
is thread safe implementation based on growing memory pool. Released memory blocks are stored for further reuse until
associated container is destroyed. When there is no available free block for reuse, a memory for the new group of
blocks is allocated to minimize heap usage.


##  Performance results

Performance can be measured with provided application. The application shows performance of STL containers using
both: a default STL allocator and the memory pool allocator.

Results presented below has been produced by program compiled in release mode on Visual Studio 2017 Community and
executed on CPU i5-2500K with RAM working @ 1866MHz:

```
Allocator performance measurement example
Version: 1.0

ForwardList PushFront - Default STL Allocator : 0.010423 seconds.
ForwardList PushFront - Memory Pool Allocator : 0.001428 seconds.

List PushFront - Default STL Allocator : 0.010404 seconds.
List PushFront - Memory Pool Allocator : 0.002038 seconds.

List PushBack - Default STL Allocator : 0.010372 seconds.
List PushBack - Memory Pool Allocator : 0.001962 seconds.

Map - Default STL Allocator : 0.021393 seconds.
Map - Memory Pool Allocator : 0.011792 seconds.

Set - Default STL Allocator : 0.030922 seconds.
Set - Memory Pool Allocator : 0.021404 seconds.
```

Please be informed that presented results may vary on different machines and depends on CPU load as well. If the code
is used under Visual C++ please remember to build it in release mode as compiler optimizations are crucial to provide
expected performance.


## Usage

Memory pool allocator is very simple in use. To associate it with any container like `std::list` include `Allocator.h`
header file and specify the allocator in second template parameter of the container:

```C++
std::list<int> list1;
std::list<int, Allocator<int>> list2;
std::list<int, Allocator<int, 16 * 1024>> list3;
```

The `list1` in the example above is using standard STL allocator. To use memory pool allocator it must be specified
in the second template parameter of the container as shown in case of `list2` object. If the number of elements used
by container is known it can be specified in second template parameter of the allocator as for `list3` to reduce heap
allocations to only one allocation of memory big enough to store all the elements. The algorithm implements lazy
initialization, and, therefore first call to the block allocation will also allocate the heap memory. Just after that
block allocation and release have O(1) complexity. Nevertheless, when number of allocated memory block exceeds the
specified number allocator will perform another heap memory allocation for future use. This happens every time when
there is no available memory blocks. On the heap memory allocation failure `std::bad_alloc()` exception is thrown.

The above example will work for `std::forward_list` as well. For containers like `std::set` and `std::map`
declaration is a little bit different. Second template parameter of that container requires to provide less function
first, and, therefore it shall be used as in the example below:

```C++
std::set<int> set;
std::set<int, std::less<int>, Allocator<int>> set2;
std::set<int, std::less<int>, Allocator<int, 16 * 1024>> set3;
```

Whole memory is allocated on heap and will be released when container is destroyed.


## MSVC support

After a recent test under Microsoft Visual Studio Community 2022 all the test passed successfully without any
extraordinary workarounds. To enable support for older versions of MSVC please define a `ENABLE_OLD_WIN32_SUPPORT`
constant.

In older versions MS has changed twice their implementation of STL allocator support. The changes were very odd
and required some workarounds which slows down the allocator. My decision was to separate MSVC support from
the original code with use of a preprocessor.


### Workaround to the first MSVC update

In MSVC, when a container with a non-default allocator specified is created the container will do the following:

* Create a X instance of allocator
* Create a Y instance of the allocator using the rebind constructor
* Allocate some memory for internal purpose using Y
* Destroy the Y instance (without releasing the allocated memory)

After initialization the container uses X instance is for block allocation / release.
Finally, when the container is destroyed it does the following:

* Create a Z instance of the allocator using the rebind constructor
* By using Z instance release the memory previously allocated with Y instance
* Destroy the Z instance
* Destroy the X instance

The memory pool allocator cleans up all the allocated memory when container is destroyed, and, therefore it cannot be
used in the way MS proposed. To workaround it, when the rebind constructor is used the allocator will use the default
STL allocator instead to do the above without any consequences.

For this workaround the default STL allocator is used only on container create and destroy so it should not affect
the performance dramatically.


### Workaround to the second MSVC update

In MSVC, when the `std::map` is used an item deletion is even weirder. Block allocation is performed as usualy using
the X instance from the example above. For item deletion new instance of allocator is created whenever map element is
removed. The newly created instance is used to delete object previously allocated with X then the new allocator
instance is destroyed. For example the MSVC implementation of `std::map::clear()` function will create/destroy as
many allocator instances as map elements are removed.

To workaround this "feature" I found that for this very special case the allocator is created using copy constructor.
Therefore for this case a pointer to the original (copied) allocator is used.