#include <string>
#include <vector>

using namespace std;

struct TileSet;
struct Layer;

struct Map
{
    string version, orientation;
    int width, height, tilewidth, tileheight;
    string renderorder;

    vector<TileSet> tilesets;
    vector<Layer> layers;
};

struct TileSet
{
    int firstgid;
    string source, name;
    int tilewidth, tileheight, spacing, margin;
};

struct TileOffset
{
    int x, y;
};

struct Layer
{
    string name;
    int x, y, width, height;
    float opacity;
    int visible;

    Layer() : x(0), y(0), opacity(1), visible(1) { }
};
