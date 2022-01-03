#include <iostream>
#include <chrono>
#include <random>

#include <set>
#include <map>
#include <list>
#include <forward_list>

#include "Allocator.h"

const size_t growSize = 1024;
const size_t numberOfIterations = 1024;
const size_t randomRange = 1024;

template <typename Container>
class PerformanceTest
{
    virtual void testIteration(size_t newSize) = 0;

    protected:

        Container container;

        std::default_random_engine randomNumberGenerator;
        std::uniform_int_distribution<size_t> randomNumberDistribution;

    public:

        PerformanceTest() :
            randomNumberGenerator(0),
            randomNumberDistribution(0, randomRange)
        {
        }

        virtual ~PerformanceTest()
        {
        }

        double run()
        {
            auto from = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < numberOfIterations; i++)
                testIteration(randomNumberDistribution(randomNumberGenerator));

            auto to = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::duration<double>>(to - from).count();
        }
};

template <typename Container>
class PushFrontTest : public PerformanceTest<Container>
{
    virtual void testIteration(size_t newSize)
    {
        size_t size = 0;

        while (size < newSize)
            this->container.push_front(size++);

        for (; size > newSize; size--)
            this->container.pop_front();
    }
};

template <typename Container>
class PushBackTest : public PerformanceTest<Container>
{
    virtual void testIteration(size_t newSize)
    {
        size_t size = 0;

        while (size < newSize)
            this->container.push_back(size++);

        for (; size > newSize; size--)
            this->container.pop_back();
    }
};

template <typename Container>
class MapTest : public PerformanceTest<Container>
{
    virtual void testIteration(size_t newSize)
    {
        size_t size = 0;

        for (; size < newSize; size++)
            this->container.insert(typename Container::value_type(size, size));

        while (size > newSize)
            this->container.erase(--size);
    }
};

template <typename Container>
class SetTest : public PerformanceTest<Container>
{
    virtual void testIteration(size_t newSize)
    {
        size_t size = 0;

        while (size < newSize)
            this->container.insert(size++);

        while (size > newSize)
            this->container.erase(--size);
    }
};

template <typename StlAllocatorContainer, typename MoyaAllocatorContainer>
void printTestStatus(const char *name, StlAllocatorContainer &stlContainer, MoyaAllocatorContainer &fastContainer)
{
    double stlRunTime = stlContainer.run();
    double moyaRunTime = fastContainer.run();

    std::cout << std::fixed;
    std::cout << name << " - Default STL Allocator : " << stlRunTime << " seconds." << std::endl;
    std::cout << name << " - Memory Pool Allocator : " << moyaRunTime << " seconds." << std::endl;
    std::cout << name << " - Gain : x" << stlRunTime / moyaRunTime << "." << std::endl;
    std::cout << std::endl;
}

int main()
{
    typedef size_t DataType;
    typedef Moya::Allocator<DataType, growSize> MemoryPoolAllocator;
    typedef Moya::Allocator<std::map<DataType, DataType>::value_type, growSize> MapMemoryPoolAllocator;

    std::cout << "Allocator performance measurement example" << std::endl;
    std::cout << "Version: 1.0" << std::endl << std::endl;

    PushFrontTest<std::forward_list<DataType>> pushFrontForwardListTestStl;
    PushFrontTest<std::forward_list<DataType, MemoryPoolAllocator>> pushFrontForwardListTestFast;
    printTestStatus("ForwardList PushFront", pushFrontForwardListTestStl, pushFrontForwardListTestFast);

    PushFrontTest<std::list<DataType>> pushFrontListTestStl;
    PushFrontTest<std::list<DataType, MemoryPoolAllocator>> pushFrontListTestFast;
    printTestStatus("List PushFront", pushFrontListTestStl, pushFrontListTestFast);

    PushBackTest<std::list<DataType>> pushBackListTestStl;
    PushBackTest<std::list<DataType, MemoryPoolAllocator>> pushBackListTestFast;
    printTestStatus("List PushBack", pushBackListTestStl, pushBackListTestFast);

    MapTest<std::map<DataType, DataType, std::less<DataType>>> mapTestStl;
    MapTest<std::map<DataType, DataType, std::less<DataType>, MapMemoryPoolAllocator>> mapTestFast;
    printTestStatus("Map", mapTestStl, mapTestFast);

    SetTest<std::set<DataType, std::less<DataType>>> setTestStl;
    SetTest<std::set<DataType, std::less<DataType>, MemoryPoolAllocator>> setTestFast;
    printTestStatus("Set", setTestStl, setTestFast);

    return 0;
}
