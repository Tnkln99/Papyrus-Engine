#include "Mrc/Exporter.h"

#include "Mrc/Exporters/AStaticModelExporter.h"

namespace Mrc
{
    void Exporter::exportAsStaticModel(const AStaticModel& staticModel, const std::string& exportDirectory, const std::string& name)
    {
        AStaticModelExporter exporter;
        exporter.exportStaticModel(staticModel, exportDirectory, name);
    }
}
