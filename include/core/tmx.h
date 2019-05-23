#ifndef TMX_H
#define TMX_H

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace Boiler { namespace tmx
{

struct TileSet;
struct Tile;
struct Layer;
struct ImageLayer;
struct ObjectGroup;
struct Object;

struct Property
{
	std::string name, value;
};

struct Properties
{
	std::map<std::string, Property> properties;

	Properties(const std::map<std::string, Property> &properties) : properties(properties) { }

	std::string getValue(std::string key) const { return properties.find(key)->second.value; }
};

struct Component
{
	Properties properties;
	explicit Component(const Properties &properties) : properties(properties) { }
};

struct Map : Component
{
	std::string version, orientation;
	int width, height, tilewidth, tileheight;
	std::string renderorder;

	std::map<int, Tile *> allTiles;
	std::vector<std::unique_ptr<TileSet>> tilesets;
	std::vector<std::shared_ptr<Layer>> layers;
	std::vector<std::unique_ptr<ImageLayer>> imageLayers;
	std::vector<std::unique_ptr<ObjectGroup>> objectGroups;

	using Component::Component;
};

struct TileSet : Component
{
	int firstgid;
	std::string source, name;
	int tilewidth, tileheight, spacing, margin;
	std::vector<std::unique_ptr<Tile>> tiles;

	using Component::Component;
};

struct Image
{
	std::string format, source, trans;
	int width, height;
};

struct Tile : Component
{
	int id;
	std::string terrain;
	float probability;
	Image image;

	using Component::Component;
};

struct TileOffset
{
	int x, y;
};

struct Layer : Component
{
	std::string name;
	int x, y, width, height;
	float opacity;
	bool visible;
	std::vector<int> tiles;

	using Component::Component;
};

struct ImageLayer : Component
{
	std::string name;
	int x, y, width, height;
	float opacity;
	bool visible;
	Image image;

	using Component::Component;
};

struct ObjectGroup : Component
{
	std::string name, color;
	int x, y, width, height;
	float opacity;
	bool visible;
	std::vector<std::unique_ptr<Object>> objects;

	using Component::Component;
};

struct Object : Component
{
	std::string id, name, type;
	int x, y, width, height;
	float rotation;
	int gid;
	bool visible;

	using Component::Component;
};

} }

#endif /* TMX_H */
