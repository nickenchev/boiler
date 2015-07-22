#include <string>
#include <vector>
#include <map>

using namespace std;

namespace ensoft
{
    struct TileSet;
    struct Tile;
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

        vector<TileSet> tilesets;
        vector<Layer> layers;
        vector<ImageLayer> imageLayers;
        vector<ObjectGroup> objectGroups;

        using TmxComponent::TmxComponent;
    };

    struct TileSet : TmxComponent
    {
        int firstgid;
        string source, name;
        int tilewidth, tileheight, spacing, margin;

        using TmxComponent::TmxComponent;
    };

    struct Tile : TmxComponent
    {
        int id;
        string terrain;
        float probability;

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

        using TmxComponent::TmxComponent;
    };

    struct ImageLayer : TmxComponent
    {
        string name;
        int x, y, width, height;
        float opacity;
        bool visible;

        using TmxComponent::TmxComponent;
    };

    struct ObjectGroup : TmxComponent
    {
        string name, color;
        int x, y, width, height;
        float opacity;
        bool visible;
        vector<ensoft::Object> objects;

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
