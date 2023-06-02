#pragma once

namespace engine
{
	namespace graphics
	{
		struct FGraphicsSettings
		{
			bool bEnableAmbientOcclusion{ true };
			float fAmbientOcclusionRadius{ 0.3f };
			float fAmbientOcclusionBias{ 0.025f };
			int iAmbientOcclusionSmoothingSteps{ 2 };
		};

		class CGraphicsSettings : public utl::singleton<CGraphicsSettings>
		{
		public:
			FGraphicsSettings& getSettings();
		private:
			FGraphicsSettings graphicsSettings{};
		};
	}
}