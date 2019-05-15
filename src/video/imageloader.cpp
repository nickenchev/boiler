#include <iostream>
#include <string>
#include <SDL.h>
#include <png.h>
#include "core/engine.h"
#include "video/renderer.h"
#include "video/imageloader.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace Boiler;

#define COMPONENT_NAME "Image Loader"

SDL_Surface *readPNG(std::string filePath);
void user_read_data(png_structp png_ptr, png_bytep data, size_t length);
void user_write_data(png_structp png_ptr, png_bytep data, size_t length);
void user_flush_data(png_structp png_ptr);
void read_row_callback(png_structp png_ptr, png_uint_32 row, int pass);

ImageLoader::ImageLoader() : logger(COMPONENT_NAME)
{
}

const std::shared_ptr<const Texture> ImageLoader::loadImage(const std::string &filePath) const
{
	logger.log("Loading image: " + filePath);

    SDL_Surface *surface = readPNG(filePath);

    assert(surface != nullptr);
    auto texture = Engine::getInstance().getRenderer().createTexture(filePath, Size(surface->w, surface->h), surface->pixels);

    SDL_FreeSurface(surface);

    return texture;
}

SDL_Surface *readPNG(std::string filePath)
{
	Logger logger("LibPNG");
	SDL_Surface *surface = nullptr;
    SDL_RWops *file = SDL_RWFromFile(filePath.c_str(), "rb");
	FILE *fp = fopen(filePath.c_str(), "rb");
	const short headerSize = 8;
	unsigned char header[headerSize];
	if (!file)
	{
		// file couldn't be opened
	}
	else
	{
		//SDL_RWread(file, header, sizeof(char), headerSize);
		fread(header, 1, headerSize, fp);
		if (png_sig_cmp(header, 0, headerSize))
		{
			logger.error("Invalid PNG file");
		}
		else
		{
			png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
			if (!png_ptr)
			{
				logger.error("Couldn't get PNG struct");
			}
			else
			{
				png_infop info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr)
				{
					logger.error("Couldn't create info struct.");
					png_destroy_read_struct(&png_ptr, nullptr, nullptr);
				}
				else
				{
					if (setjmp(png_jmpbuf(png_ptr)))
					{
						logger.error("Couldn't setjmp.");
					}
					else
					{
						// setup custom read/write functions
						//png_set_read_fn(png_ptr, file, user_read_data);
						png_init_io(png_ptr, fp);

						// ensures we don't read the header in again
						png_set_sig_bytes(png_ptr, headerSize);

						png_set_read_status_fn(png_ptr, read_row_callback);
						png_read_png(png_ptr, info_ptr, 0, nullptr);

						// get image info
						png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
						png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
						int bitDepth = png_get_bit_depth(png_ptr, info_ptr);
						int colorType = png_get_color_type(png_ptr, info_ptr);
						int colorComponents = (colorType == PNG_COLOR_TYPE_RGBA) ? 4 : 3;

						png_bytepp dataPtr = png_get_rows(png_ptr, info_ptr);

						// create the SDL surface from the pixel dataUint32 rmask, gmask, bmask, amask;
						Uint32 rmask, gmask, bmask, amask;
					#if SDL_BYTEORDER == SDL_BIG_ENDIAN
						int shift = (req_format == PNG_COLOR_TYPE_RGBA) ? 8 : 0;
						rmask = 0xff000000 >> shift;
						gmask = 0x00ff0000 >> shift;
						bmask = 0x0000ff00 >> shift;
						amask = 0x000000ff >> shift;
					#else // little endian, like x86
						rmask = 0x000000ff;
						gmask = 0x0000ff00;
						bmask = 0x00ff0000;
						amask = (colorType == PNG_COLOR_TYPE_RGB) ? 0 : 0xff000000;
					#endif
						int rowBytes = png_get_rowbytes(png_ptr, info_ptr);
						const size_t imgBytes = rowBytes * height;
						logger.log("Read PNG: " + std::to_string(imgBytes) + " bytes.");

						unsigned char *outData = (unsigned char*)malloc(imgBytes);
						png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
						for (int i = 0; i < height; i++)
						{
							memcpy(outData + (rowBytes * i), row_pointers[i], rowBytes);
						}

						surface = SDL_CreateRGBSurfaceFrom((void *)outData, width, height, bitDepth * colorComponents,
														   width * colorComponents, rmask, gmask, bmask, amask);
						if (!surface)
						{
							logger.error(SDL_GetError());
						}

						// clean up
						free(outData);
						png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
					}
				}
			}
		}

		// cleanup file handles
		SDL_RWclose(file);
		std::fclose(fp);
	}
	return surface;
}

void user_read_data(png_structp png_ptr, png_bytep data, size_t length)
{
	png_voidp io_ptr = png_get_io_ptr(png_ptr);
	SDL_RWops *file = static_cast<SDL_RWops *>(io_ptr);

	size_t objsRead = SDL_RWread(file, data, sizeof(png_byte), length);
	std::cout << objsRead * sizeof(png_bytep) << std::endl;
}

void user_write_data(png_structp png_ptr, png_bytep data, size_t length) { }
void user_flush_data(png_structp png_ptr) { }
void read_row_callback(png_structp png_ptr, png_uint_32 row, int pass) { }
