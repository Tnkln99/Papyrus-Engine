#include "Snd/Pch.h"
#include "Snd/Dx12/UploadContext.h"

#include "Snd/Dx12/Utils/DxHelpers.h"
#include "Snd/Dx12/CommandAllocator.h"
#include "Snd/Dx12/GraphicsCommandList.h"
#include "Snd/Dx12/CommandQueue.h"
#include "Snd/Dx12/Fence.h"
#include "Snd/Dx12/Buffer.h"
#include "Snd/Dx12/Texture.h"


namespace Snd::Dx12
{
	UploadContext::UploadContext(Device& device) : m_device{ device }
	{
		m_commandAllocator = std::make_unique<CommandAllocator>("UploadContext CommandAllocator", m_device, GraphicsCommandListType::Copy);
		m_commandList = std::make_unique<GraphicsCommandList>("UploadContext CommandList", m_device, *m_commandAllocator, GraphicsCommandListType::Copy);
		m_commandQueue = std::make_unique<CommandQueue>("UploadContext CommandQueue", m_device, GraphicsCommandListType::Copy);

		m_commandList->close();

		m_fence = std::make_unique<Fence>("UploadContext Fence", m_device);
	}

	UploadContext::~UploadContext() = default;

	void UploadContext::registerBufferForUpload(const std::shared_ptr<Buffer> &buffer, void *data)
	{
		m_flushFlag = true;

		m_buffersToUpload.push_back( {
			buffer,
			std::make_unique<Buffer>(
				"Upload Buffer",
				m_device,
				ResourceHeapType::Upload,
				ResourceFlag::None,
				ResourceState::GenericRead,
				buffer->getElementSize(),
				buffer->getElementCount()),
			data });
	}

	void UploadContext::registerTexture2DForUpload(const std::shared_ptr<Texture2D> &texture, void *data)
	{
		m_flushFlag = true;

		const UINT64 requiredSize = texture->getRequiredIntermediateSize(0,1);

		m_texturesToUpload.push_back( { texture,
			std::make_unique<Buffer>
		(
			"Upload Buffer",
			m_device,
			ResourceHeapType::Upload,
			ResourceFlag::None,
			ResourceState::GenericRead,
			requiredSize,
			1
		), data });
	}

	void UploadContext::uploadBuffer(const BufferToUpload& bufferToUpload) const
	{
		const UINT64 bufferSize = bufferToUpload.m_buffer.lock()->getElementCount() * bufferToUpload.m_buffer.lock()->getElementSize();

        // Copy data to the upload buffer
        UINT8* mappedData = nullptr;
		bufferToUpload.m_uploadBuffer->map(0, 0, reinterpret_cast<void**>(&mappedData));
        memcpy(mappedData, bufferToUpload.m_data, bufferSize);
        bufferToUpload.m_uploadBuffer->unmap();

        // Copy data from the upload buffer to the default buffer
        m_commandList->copyBufferRegion(
            *bufferToUpload.m_uploadBuffer,
            *bufferToUpload.m_buffer.lock(),
            0,
            0,
            bufferSize);
	}

	void UploadContext::uploadTexture2D(const TextureToUpload& uploadTexture) const
	{
		const std::shared_ptr<Texture2D> texture = uploadTexture.m_texture.lock();

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = uploadTexture.m_data;
		textureData.RowPitch = texture->getWidth() * texture->getFormatByteSize();
		textureData.SlicePitch = textureData.RowPitch * texture->getHeight();

		m_commandList->updateSubResources(*uploadTexture.m_uploadBuffer, *texture, 0, 1, 0, &textureData);
	}

	void UploadContext::start() const
	{
		m_commandAllocator->reset();
		m_commandList->reset(*m_commandAllocator);
	}

	void UploadContext::end()
	{
		m_commandList->close();
		m_commandQueue->executeSingleCommandList(*m_commandList);

		m_fence->scheduleSignalCommand(*m_commandQueue, m_fenceValue);
		m_fence->waitForGpu(m_fenceValue);

		m_fenceValue++;
	}

	void UploadContext::flush()
	{
		if (!m_flushFlag)
		{
			return;
		}

		start();

		for (const auto& buffer : m_buffersToUpload)
		{
			uploadBuffer(buffer);
		}

		for (const auto& texture : m_texturesToUpload)
		{
			uploadTexture2D(texture);
		}

		end();

		m_texturesToUpload.clear();
		m_buffersToUpload.clear();

		m_flushFlag = false;
	}
}

