#include <iostream>
#include <string>
#include <SDL.h>
#include <png.h>
#include "core/engine.h"
#include "video/renderer.h"
#include "video/imageloader.h"

using namespace Boiler;

#define COMPONENT_NAME "Image Loader"

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
	readPNG(filePath);

    SDL_Surface *surface = nullptr;

    assert(surface != nullptr);
    auto texture = Engine::getInstance().getRenderer().createTexture(filePath, Size(surface->w, surface->h), surface->pixels);

    SDL_FreeSurface(surface);

    return std::move(texture);
}

void ImageLoader::readPNG(std::string filePath) const
{
    SDL_RWops *file = SDL_RWFromFile(filePath.c_str(), "rb");
	const short headerSize = 8;
	unsigned char header[headerSize];
	if (!file)
	{
		// file couldn't be opened
	}
	else
	{
		SDL_RWread(file, header, sizeof(char), headerSize);
		if (png_sig_cmp(header, 0, headerSize))
		{
			// invalid header
		}
		else
		{
			png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
			if (!png_ptr)
			{
				// couldn't get png struct
			}
			else
			{
				png_infop info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr)
				{
					// couldn't init info struct
					png_destroy_read_struct(&png_ptr, nullptr, nullptr);

					if (setjmp(png_jmpbuf(png_ptr)))
					{
						// error
					}
					else
					{
						// setup custom read/write functions
						png_set_read_fn(png_ptr, file, user_read_data);
						png_set_write_fn(png_ptr, file, user_write_data, user_flush_data);

						// ensures we don't read the header in again
						png_set_sig_bytes(png_ptr, headerSize);

						// callback function for status
						png_set_read_status_fn(png_ptr, read_row_callback);
					}
				}
			}
		}

		// cleanup file handles
		SDL_RWclose(file);
	}
}

void user_read_data(png_structp png_ptr, png_bytep data, size_t length)
{
}

void user_write_data(png_structp png_ptr, png_bytep data, size_t length)
{
}

void user_flush_data(png_structp png_ptr)
{
}

void read_row_callback(png_structp png_ptr, png_uint_32 row, int pass)
{
	std::cout << "read read read ";
}
