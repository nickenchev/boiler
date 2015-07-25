#include <string>
#include <memory>
#include <vector>
#include <map>

using namespace std;

namespace ensoft
{
    struct TmxTileSet;
    struct TmxTile;
    struct Layer;
    struct ImageLayer;
    struct ObjectGroup;
    struct Object;

    struct Property
    {
        string name, value;
    };
    struct Properties
    {
        map<string, Property> properties;

        Properties(const map<string, Property> &properties) : properties(properties) { }

        string getValue(string key) const { return properties.find(key)->second.value; }
    };

    struct TmxComponent
    {
        Properties properties;
        explicit TmxComponent(const Properties &properties) : properties(properties) { }
    };

    struct Map : TmxComponent
    {
        string version, orientation;
        int width, height, tilewidth, tileheight;
        string renderorder;

        map<int, TmxTile *> allTiles;
        vector<unique_ptr<TmxTileSet>> tilesets;
        vector<unique_ptr<Layer>> layers;
        vector<unique_ptr<ImageLayer>> imageLayers;
        vector<unique_ptr<ObjectGroup>> objectGroups;

        using TmxComponent::TmxComponent;
    };

    struct TmxTileSet : TmxComponent
    {
        int firstgid;
        string source, name;
        int tilewidth, tileheight, spacing, margin;
        vector<unique_ptr<TmxTile>> tiles;

        using TmxComponent::TmxComponent;
    };

    struct Image
    {
        string format, source, trans;
        int width, height;
    };

    struct TmxTile : TmxComponent
    {
        int id;
        string terrain;
        float probability;
        Image image;

        using TmxComponent::TmxComponent;
    };

    struct TileOffset
    {
        int x, y;
    };

    struct Layer : TmxComponent
    {
        string name;
        int x, y, width, height;
        float opacity;
        bool visible;
        vector<int> tiles;

        using TmxComponent::TmxComponent;
    };

    struct ImageLayer : TmxComponent
    {
        string name;
        int x, y, width, height;
        float opacity;
        bool visible;
        Image image;

        using TmxComponent::TmxComponent;
    };

    struct ObjectGroup : TmxComponent
    {
        string name, color;
        int x, y, width, height;
        float opacity;
        bool visible;
        vector<unique_ptr<Object>> objects;

        using TmxComponent::TmxComponent;
    };

    struct Object : TmxComponent
    {
        string id, name, type;
        int x, y, width, height;
        float rotation;
        int gid;
        bool visible;

        using TmxComponent::TmxComponent;
    };
}
