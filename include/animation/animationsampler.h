#ifndef ANIMATIONSAMPLER_H
#define ANIMATIONSAMPLER_H

#include <vector>
#include <iostream>
#include <core/logger.h>

namespace Boiler {

static Logger logger("Animation Sampler");

class AnimationSampler
{
	const std::vector<float> keyFrameTimes;
	const std::vector<std::byte> data;
	
public:
    AnimationSampler(const std::vector<float> &&keyFrameTimes, const std::vector<std::byte> &&data)
		: keyFrameTimes(keyFrameTimes), data(data) {}

	template<typename ValueType>
	ValueType sample(float time) const
	{
		ValueType result;
		
		// binary search for time
		bool found = false;
		int begin = 0;
		int end = keyFrameTimes.size() - 1;
		unsigned int prevIdx = 0;
		unsigned int nextIdx = 0;

		do
		{
			int mid = (begin + end) / 2;
			if (time > keyFrameTimes[mid])
			{
				// check upper half
				if (mid < keyFrameTimes.size() - 1 &&
					time < keyFrameTimes[mid + 1])
				{
					found = true;
					prevIdx = mid;
					nextIdx = mid + 1;
				}
				begin = mid;
			}
			else
			{
				// check lower half
				if (mid > 0 && time > keyFrameTimes[mid - 1])
				{
					found = true;
					prevIdx = mid - 1;
					nextIdx = mid;
				}
				end = mid;
			}
		} while (end - begin > 1 && !found);

		if (found)
		{
			float prevTime = keyFrameTimes[prevIdx];
			float nextTime = keyFrameTimes[nextIdx];
			float interp = (time - prevTime) / (nextTime - prevTime);
			logger.log("Interpolate {}-{} > {}", prevTime, nextTime, interp);

			const ValueType *prevPtr = reinterpret_cast<const ValueType *>(data.data() + (sizeof(ValueType) * prevIdx));
			const ValueType *nextPtr = reinterpret_cast<const ValueType *>(data.data() + (sizeof(ValueType) * nextIdx));

			result = *prevPtr + interp * (*nextPtr - *prevPtr);
		}
		return result;
	}
};

};

#endif /* ANIMATIONSAMPLER_H */
