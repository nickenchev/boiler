#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <string>

class SpriteLoader
{
private:
    SpriteLoader() { }
public:
    SpriteLoader(const SpriteLoader &spriteLoader) = delete;
    void operator=(const SpriteLoader &spriteLoader) = delete;

    static SpriteLoader &getInstance()
    {
        static SpriteLoader instance;
        return instance;
    }

    //sprite handling methods
    void loadSheet(std::string filename);
};

#endif // SPRITELOADER_H
