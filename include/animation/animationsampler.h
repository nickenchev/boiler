#ifndef ANIMATIONSAMPLER_H
#define ANIMATIONSAMPLER_H

#include <vector>
#include <iostream>

class AnimationSampler
{
	const std::vector<float> keyFrameTimes;
	const std::vector<std::byte> data;
	
public:
    AnimationSampler(const std::vector<float> &&keyFrameTimes, const std::vector<std::byte> &&data)
		: keyFrameTimes(keyFrameTimes), data(data) {}

	template<typename ValueType>
	std::pair<float, ValueType> sample(float time) const
	{
		for (float t : keyFrameTimes)
		{
		}

		return std::make_pair(0, 0);
	}
};


#endif /* ANIMATIONSAMPLER_H */
