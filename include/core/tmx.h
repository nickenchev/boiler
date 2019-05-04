#ifndef TMX_H
#define TMX_H

#include <string>
#include <memory>
#include <vector>
#include <map>

using namespace std;

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
	string name, value;
};

struct Properties
{
	map<string, Property> properties;

	Properties(const map<string, Property> &properties) : properties(properties) { }

	string getValue(string key) const { return properties.find(key)->second.value; }
};

struct Component
{
	Properties properties;
	explicit Component(const Properties &properties) : properties(properties) { }
};

struct Map : Component
{
	string version, orientation;
	int width, height, tilewidth, tileheight;
	string renderorder;

	map<int, Tile *> allTiles;
	vector<unique_ptr<TileSet>> tilesets;
	vector<shared_ptr<Layer>> layers;
	vector<unique_ptr<ImageLayer>> imageLayers;
	vector<unique_ptr<ObjectGroup>> objectGroups;

	using Component::Component;
};

struct TileSet : Component
{
	int firstgid;
	string source, name;
	int tilewidth, tileheight, spacing, margin;
	vector<unique_ptr<Tile>> tiles;

	using Component::Component;
};

struct Image
{
	string format, source, trans;
	int width, height;
};

struct Tile : Component
{
	int id;
	string terrain;
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
	string name;
	int x, y, width, height;
	float opacity;
	bool visible;
	vector<int> tiles;

	using Component::Component;
};

struct ImageLayer : Component
{
	string name;
	int x, y, width, height;
	float opacity;
	bool visible;
	Image image;

	using Component::Component;
};

struct ObjectGroup : Component
{
	string name, color;
	int x, y, width, height;
	float opacity;
	bool visible;
	vector<unique_ptr<Object>> objects;

	using Component::Component;
};

struct Object : Component
{
	string id, name, type;
	int x, y, width, height;
	float rotation;
	int gid;
	bool visible;

	using Component::Component;
};

} }

#endif /* TMX_H */
