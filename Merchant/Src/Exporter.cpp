#include "Mrc/Exporter.h"

#include "Mrc/Exporters/AClusteredStaticModelExporter.h"
#include "Mrc/Exporters/AStaticModelExporter.h"

namespace Mrc
{
    void Exporter::exportAsStaticModel(const AStaticModel& staticModel, const std::string& exportDirectory, const std::string& name)
    {
        AStaticModelExporter exporter;
        exporter.exportAsStaticModel(staticModel, exportDirectory, name);
    }

    void Exporter::exportAsClusteredStaticModel(const AStaticModel &staticModel, const std::string &exportDirectory,
        const std::string &name)
    {
        AClusteredStaticModelExporter exporter;
        exporter.exportAsClusteredStaticModel(staticModel, exportDirectory, name);
    }
}
