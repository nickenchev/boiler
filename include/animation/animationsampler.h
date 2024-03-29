#ifndef ANIMATIONSAMPLER_H
#define ANIMATIONSAMPLER_H

#include <vector>
#include <iostream>

#include "core/common.h"

namespace Boiler {

using SamplerId = size_t;

class AnimationSampler
{
	const std::vector<float> keyFrameTimes;
	const std::vector<std::byte> data;
	
public:
    AnimationSampler(const std::vector<float> &&keyFrameTimes, const std::vector<std::byte> &&data)
		: keyFrameTimes(keyFrameTimes), data(data) {}

	float getMinTime() const { return keyFrameTimes[0]; }
	float getMaxTime() const { return keyFrameTimes[keyFrameTimes.size() - 1]; }

	template<typename ValueType>
	ValueType sample(Time time) const
	{
		ValueType result;
		
		// binary search for time
		unsigned int prevIdx = 0;
		unsigned int nextIdx = 0;

		if (time <= getMinTime())
		{
			result = *reinterpret_cast<const ValueType *>(data.data());
		}
		else if (time >= getMaxTime())
		{
			result = *reinterpret_cast<const ValueType *>(data.data() + sizeof(ValueType) * (keyFrameTimes.size() - 1));
		}
		else
		{
			int i = 0;
			while (time > keyFrameTimes[i])
			{
				i++;
			}
			prevIdx = i - 1;
			nextIdx = i;

			float prevTime = keyFrameTimes[prevIdx];
			float nextTime = keyFrameTimes[nextIdx];
			float interp = (time - prevTime) / (nextTime - prevTime);

			const ValueType *prevPtr = reinterpret_cast<const ValueType *>(data.data() + (sizeof(ValueType) * prevIdx));
			const ValueType *nextPtr = reinterpret_cast<const ValueType *>(data.data() + (sizeof(ValueType) * nextIdx));
			result = *prevPtr + interp * (*nextPtr - *prevPtr);
		}
		return result;
	}
};

};

#endif /* ANIMATIONSAMPLER_H */
