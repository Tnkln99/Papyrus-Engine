#pragma once
#include "Mrc/Data/Assets.h"

namespace Mrc
{
    class IImporter
    {
    public:
        virtual ~IImporter() = default;
        virtual void import(std::string const &filePath, AScene& outScene) = 0;
    };
}
