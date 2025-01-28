#pragma once
#include "Mrc/Data/AStaticModel.h"

namespace Mrc
{
    class IImporter
    {
    public:
        virtual ~IImporter() = default;
        virtual void import(std::string const &filePath, AStaticModel& outModel) = 0;
    };
}
