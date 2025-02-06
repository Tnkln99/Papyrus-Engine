#pragma once

namespace Ui
{
    enum class ConvertOption
    {
        StaticModel,
        MeshletModel,
        Cloth
    };

    class ConvertOptions
    {
    public:
        ConvertOptions();

        void render();
    private:
        ConvertOption m_selectedOption;
    };
}





