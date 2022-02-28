#ifndef SKYBOXIMAGES_H
#define SKYBOXIMAGES_H

#include "imagedata.h"

namespace Boiler {
class SkyBoxImages
{
	const ImageData top, bottom, left, right, front, back;
	
public:
    SkyBoxImages(ImageData &&top, ImageData &&bottom,
				 ImageData &&left, ImageData &&right,
				 ImageData &&front, ImageData &&back)
		: top(top), bottom(bottom),
		  left(left), right(right),
		  front(front), back(back)
	{
	}
};

}
#endif /* SKYBOXIMAGES_H */
