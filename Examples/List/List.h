#ifndef ListH
#define ListH

#include <iostream>
#include <list>

#include "Allocator.h"

class List
{
    public:

        List(int argumentCount, char **arguments)
        {
        }

        int run()
        {
            std::cout << "List allocator example" << std::endl;
            std::cout << "Version: 1.0" << std::endl;

            std::list<int, Moya::Allocator<int, 4>> memoryPoolList;

            for (int i = 0; i < 10; i++)
                memoryPoolList.push_back(i);

            for (auto &x : memoryPoolList)
                std::cout << x << std::endl;

            return 0;
        }
};

#endif
