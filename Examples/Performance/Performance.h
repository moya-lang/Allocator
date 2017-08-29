#ifndef PerformanceH
#define PerformanceH

#include <iostream>
#include <chrono>
#include <random>

#include <set>
#include <map>
#include <list>
#include <forward_list>

#include "Allocator.h"

class PerformanceTimer
{
    std::chrono::high_resolution_clock::time_point from, to;

    public:

        void start()
        {
            from = std::chrono::high_resolution_clock::now();
        }

        void stop()
        {
            to = std::chrono::high_resolution_clock::now();
        }

        double toSeconds() const
        {
            return std::chrono::duration_cast<std::chrono::duration<double>>(to - from).count();
        }
};

class Performance
{
    static const std::size_t growSize = 1024;
    static const int randomRange = 16 * 1024;
    static const int numberOfIterations = 1024;

    template <typename Container>
    double pushBackTest()
    {
        int size = 0;
        Container container;

        std::default_random_engine engine(0);
        std::uniform_int_distribution<int> distribution(0, randomRange);

        PerformanceTimer timer;
        timer.start();

        for (int i = 0; i < numberOfIterations; i++) {
            int randomSize = distribution(engine);

            while (size < randomSize)
                container.push_back(size++);

            for (; size > randomSize; size--)
                container.pop_back();
        }

        timer.stop();
        return timer.toSeconds();
    }

    template <typename Container>
    double pushFrontTest()
    {
        int size = 0;
        Container container;

        std::default_random_engine engine(0);
        std::uniform_int_distribution<int> distribution(0, randomRange);

        PerformanceTimer timer;
        timer.start();

        for (int i = 0; i < numberOfIterations; i++) {
            int randomSize = distribution(engine);

            while (size < randomSize)
                container.push_front(size++);

            for (; size > randomSize; size--)
                container.pop_front();
        }

        timer.stop();
        return timer.toSeconds();
    }

    template <typename Container>
    double setTest()
    {
        int size = 0;
        Container container;

        std::default_random_engine engine(0);
        std::uniform_int_distribution<int> distribution(0, randomRange);

        PerformanceTimer timer;
        timer.start();

        for (int i = 0; i < numberOfIterations; i++) {
            int randomSize = distribution(engine);

            while (size < randomSize)
                container.insert(size++);

            while (size > randomSize)
                container.erase(--size);
        }

        timer.stop();
        return timer.toSeconds();
    }

    template <typename Container>
    double mapTest()
    {
        int size = 0;
        Container container;

        std::default_random_engine engine(0);
        std::uniform_int_distribution<int> distribution(0, randomRange);

        PerformanceTimer timer;
        timer.start();

        for (int i = 0; i < numberOfIterations; i++) {
            int randomSize = distribution(engine);

            while (size < randomSize)
                container.insert(std::pair<char, int>(size++, size));

            while (size > randomSize)
                container.erase(--size);
        }

        timer.stop();
        return timer.toSeconds();
    }


    public:

        List(int argumentCount, char **arguments)
        {
        }

        int run()
        {
            std::cout << "List allocator example" << std::endl;
            std::cout << "Version: 1.0" << std::endl << std::endl;

            std::cout << "                    Set STL : " << std::fixed << setTest<std::set<int, std::less<int>>>() << " seconds." << std::endl;
            std::cout << "                   Set Fast : " << std::fixed << setTest<std::set<int, std::less<int>, Moya::Allocator<int, growSize>>>() << " seconds." << std::endl;
            std::cout << std::endl;

            std::cout << "                    Map STL : " << std::fixed << mapTest<std::map<int, int, std::less<int>>>() << " seconds." << std::endl;
            std::cout << "                   Map Fast : " << std::fixed << mapTest<std::map<int, int, std::less<int>, Moya::Allocator<int, growSize>>>() << " seconds." << std::endl;
            std::cout << std::endl;

            std::cout << "          List PushBack STL : " << std::fixed << pushBackTest<std::list<int>>() << " seconds." << std::endl;
            std::cout << "         List PushBack Fast : " << std::fixed << pushBackTest<std::list<int, Moya::Allocator<int, growSize>>>() << " seconds." << std::endl;
            std::cout << std::endl;

            std::cout << "         List PushFront STL : " << std::fixed << pushFrontTest<std::list<int>>() << " seconds." << std::endl;
            std::cout << "        List PushFront Fast : " << std::fixed << pushFrontTest<std::list<int, Moya::Allocator<int, growSize>>>() << " seconds." << std::endl;
            std::cout << std::endl;

            std::cout << "  ForwardList PushFront STL : " << std::fixed << pushFrontTest<std::forward_list<int>>() << " seconds." << std::endl;
            std::cout << " ForwardList PushFront Fast : " << std::fixed << pushFrontTest<std::forward_list<int, Moya::Allocator<int, growSize>>>() << " seconds." << std::endl;
            std::cout << std::endl;

            return 0;
        }
};

#endif
