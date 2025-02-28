#include "Mrc/Exporter.h"

#include "Mrc/Exporters/AStaticModelExporter.h"
#include "Nmd/Logger.h"


namespace Mrc
{
    void Exporter::exportAsStaticModel(const Arf::StaticModel& staticModel, const std::string& exportDirectory, const std::string& name)
    {
        AStaticModelExporter exporter;
        exporter.exportAsStaticModel(staticModel, exportDirectory, name);
    }

}
