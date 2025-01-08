#include "Snd/Pch.h"
#include <gtest/gtest.h>

#include "Snd/Dx12/Device.h"

namespace Snd::Dx12
{
	TEST(Device, ConstructionNullCheck)
	{
		const Device device{"Test Device"};

		EXPECT_TRUE(device.getDxDevice() != nullptr);
	}
}
