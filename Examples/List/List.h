#ifndef ListH
#define ListH

#include <iostream>

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
            std::cout << "Varsion: 1.0" << std::endl;

            return 0;
        }
};

#endif
