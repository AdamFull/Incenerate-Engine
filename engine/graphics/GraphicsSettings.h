#pragma once

namespace engine
{
	namespace graphics
	{
		struct FGraphicsSettings
		{
			// Ambient occlusion
			bool bEnableAmbientOcclusion{ true };
			float fAmbientOcclusionRadius{ 0.3f };
			float fAmbientOcclusionBias{ 0.025f };
			int iAmbientOcclusionSmoothingSteps{ 2 };

			// Reflections
			bool bEnableReflections{ false };
			float fReflectionRayStep{ 0.2f };
			int iReflectionIterationCount{ 100 };
			float fReflectionDistanceBias{ 0.05f };
			float fReflectionSpecularFalloffExponent{ 3.f };
			int iReflectionSampleCount{ 4 };
			float fReflectionSamplingCoefficient{ 0.01f };
			bool bReflectionDebugDraw{ false };
			bool bReflectionBinarySearchEnabled{ true };
			bool bReflectionAdaptiveStepEnabled{ true };
			bool bReflectionExponentialStepEnabled{ false };
			bool bReflectionSamplingEnabled{ true };

			// Directional occlusion
			bool bEnableGlobalIllumination{ true };
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