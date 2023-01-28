#pragma once

namespace engine
{
	template<class _Ty>
	inline _Ty rangeToRange(_Ty input, _Ty in_min, _Ty in_max, _Ty out_min, _Ty out_max)
	{
		return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	glm::vec4 encodeIdToColor(uint32_t hex);
	uint32_t decodeIdFromColor(uint8_t x, uint8_t y, uint8_t z, uint8_t w);
}