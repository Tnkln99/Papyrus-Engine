#pragma once

#include <memory>
#include <vector>

#include "Buffer.h"

namespace Snd::Dx12
{
	class Buffer;
	class Texture2D;
	class Device;
	class GraphicsCommandList;
	class CommandQueue;
	class CommandAllocator;
	class Fence;

	enum class ShaderVisibility;

	struct UploadContext
	{
	public:
		explicit UploadContext(Device& device);
		~UploadContext();

		void registerBufferForUpload(const std::shared_ptr<Buffer>& buffer, void* data);
		void registerTexture2DForUpload(const std::shared_ptr<Texture2D>& texture, void* data);

		void flush();

	private:
		struct BufferToUpload
		{
			std::weak_ptr<Buffer> m_buffer;
			std::unique_ptr<Buffer> m_uploadBuffer;
			void* m_data;
		};

		struct TextureToUpload
		{
			std::weak_ptr<Texture2D> m_texture;
			std::unique_ptr<Buffer> m_uploadBuffer;
			void* m_data;
		};

	private:
		void start() const;
		void end();

		void uploadBuffer(const BufferToUpload& bufferToUpload) const;
		void uploadTexture2D(const TextureToUpload& uploadTexture) const;
	private:
		Device& m_device;

		std::unique_ptr<CommandAllocator> m_commandAllocator;
		std::unique_ptr<GraphicsCommandList> m_commandList;
		std::unique_ptr<CommandQueue> m_commandQueue;

		std::unique_ptr<Fence> m_fence;
		UINT64 m_fenceValue = 1;

		bool m_flushFlag = false;

		std::vector<TextureToUpload> m_texturesToUpload;
		std::vector<BufferToUpload> m_buffersToUpload;
	};
}


