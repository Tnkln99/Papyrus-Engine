#include "Nmd/Asserter.h"

#include <iostream>

namespace Nmd
{
    void Asserter::assertation(AssertType type, const bool condition, const std::string &message)
    {
#ifdef _DEBUG
        switch (type)
        {
            case AssertType::Expect:
            case AssertType::Assert:
            if (!condition)
            {
                std::cout << message;
                __debugbreak();
            }
            break;
        }
#endif
    }
}
