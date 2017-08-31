# Allocator

Fast C++11 allocator for STL containers.


## Foreword

Presented solution is a special purpose allocator designed to increase performance of STL containers. This allocator is thread safe implementation based on growing memory pool. Released memory blocks are stored for further reuse until container is destroyed. When there is no available free block for reuse, a new group of memory blocks is allocated on heap and marked as available.


##  Performance results

Performance example provided in Examples\Performance\Performance.h shows different STL containers working with both: default STL allocator and memory pool allocator.

Below has been presented results produced by program compiled in release mode on Visual Studio 2017 Community and executed on CPU i5-2500K with RAM working @ 1866MHz:

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

Please be informed that presented results may vary on different machines and depends on CPU load as well. Before running
this project in Visual C++, please be certain to build it in release mode because compiler optimizations are crucial to
provide desired performance.


## Usage

Memory pool allocator is very simple in use. To associate it with containers like `std::list` include file `Allocator.h`
and specify it in second template parameter of that container:

```C++
std::list<int> list1;
std::list<int, Allocator<int>> list2;
std::list<int, Allocator<int, 16 * 1024>> list3;
```

Object instance `list1` is using standard STL allocator. To employ memory pool allocator, it must be specified in second
template parameter of container, as in case of `list2` object. If the number of elements used by list is known, it can
be specified in second template parameter of allocator as for `list3` object. In third case whole memory for all objects
will be allocated once. It implements lazy initialization, so first allocation of memory block by container will force
to allocate memory resources for all blocks. Just after that, block allocation and release have O(1) complexity. When
number of allocated memory block exceeds the specified number, allocator will perform memory allocation to grow by this
size. It happens every time when there is no available memory blocks. On memory allocation failure `std::bad_alloc()`
exception is thrown.

The above example will work for `std::forward_list` as well. For containers like `std::set` and `std::map` declaration
is a little bit different. Second template parameter of that container requires to provide less function first, so it
should be used this way:

```C++
std::set<int> set;
std::set<int, std::less<int>, Allocator<int>> set2;
std::set<int, std::less<int>, Allocator<int, 16 * 1024>> set3;
```

Whole memory is allocated on heap and will be released when container is destroyed.


## Design decisions

Allocator implementation uses internally a standard STL allocator. It will be used instead of memory pool, when
container tries to rebind allocator. By default, when one allocator allocates a memory block, another allocator of that
type should be able to release it. However, this is not the case for this kind of allocator. As it grows every time when
needed, it would immediately lead to consume whole memory. Therefore, each container have its own allocator with its own
memory resources. Once the container is destroyed all associated memory resources are released. For this purpose rebind
causes to use standard STL allocator in place of memory pool. Additionally copy and move of allocator has been blocked.
