#pragma once

#include <string>
#include <functional>

namespace Os
{
    enum class MessageType
    {
        Paint,
        Resize,
        KeyDown,
        Close
    };

    struct Message
    {
        MessageType m_type;
        union
        {
            struct
            {
                int m_width;
                int m_height;
            } m_resize;

            uint8_t m_keyId;
        };
    };

    using MessageCallback = std::function<void(const Message&)>;

    class IWindow
    {
    public:
        IWindow();

        virtual ~IWindow();

        virtual bool init(const std::string &title, int width, int height) = 0;
        virtual void show() = 0;
        virtual bool shouldClose() = 0;
        virtual void processMessage() = 0;

        virtual void* getHandler() = 0;

        void setMessageCallback(const MessageCallback &callback) { m_messageCallback = callback; }
    protected:
        MessageCallback m_messageCallback = nullptr;

        int m_width{};
        int m_height{};
    };
}
