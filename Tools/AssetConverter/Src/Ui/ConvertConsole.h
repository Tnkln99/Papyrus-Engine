#pragma once

namespace Ui
{
    enum class ConvertOption
    {
        StaticModel,
        ClusteredModel,
        Cloth
    };

    class ConvertConsole
    {
    public:
        ConvertConsole();

        void render();
        bool startConversion() const;
        bool showFileExplorer() const;
        bool deleteInputSelected() const;
        bool deleteOutputSelected() const;

        ConvertOption getOption() const
        {
            return m_selectedOption;
        }
    private:
        bool m_bConvertButtonClicked = false;
        bool m_bFileExplorerButtonClicked = false;
        bool m_bDeleteOutputClicked = false;
        bool m_bDeleteInputClicked = false;
        ConvertOption m_selectedOption;
    };
}





