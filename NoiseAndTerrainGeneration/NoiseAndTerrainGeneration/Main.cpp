#include <iostream>
#include <noise/noise.h>
#include "noiseutils.h"
#include "noiseutils.cpp"
#include <SFML/Graphics.hpp>

using namespace noise;


class TileMap : public sf::Drawable, public sf::Transformable
{
public:

	bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height)
	{
		// load the tileset texture
		if (!m_tileset.loadFromFile(tileset))
			return false;

		// resize the vertex array to fit the level size
		m_vertices.setPrimitiveType(sf::Quads);
		m_vertices.resize(width * height * 4);

		// populate the vertex array, with one quad per tile
		for (unsigned int i = 0; i < width; ++i)
			for (unsigned int j = 0; j < height; ++j)
			{
				// get the current tile number
				int tileNumber = tiles[i + j * width];

				// find its position in the tileset texture
				int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
				int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

				// get a pointer to the current tile's quad
				sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

				// define its 4 corners
				quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
				quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
				quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
				quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

				// define its 4 texture coordinates
				quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
				quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
				quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
			}

		return true;
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// apply the tileset texture
		states.texture = &m_tileset;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
};

void LoadLevel(int* level, int height, int width, NoiseMap heightMap)
{
	int ind = 0;

	for (int i = 0; i <= height - 1; ++i)
	{
		for (int j = 0; j <= width - 1; ++j)
		{
			float val = heightMap.GetValue(j, i);
			if (val >= -1.00 && val < -0.25) //deeps
			{
				*(level + ind) = 0;
			}
			else if (val >= -0.25 && val < 0.00) //shallow
			{
				*(level+ind) = 1;
			}
			else if (val >= 0.00 && val < 0.0625) //shore
			{
				*(level + ind) = 2;
			}
			else if (val >= 0.625 && val < 0.125) //sand
			{
				*(level+ind) = 3;
			}
			else if (val >= 0.125 && val < 0.375) //grass
			{
				*(level+ind) = 4;
			}
			else if (val >= 0.375 && val < 0.75) //dirt
			{
				*(level+ind) = 5;
			}
			else if (val >= 0.75 && val <= 1) //rock
			{
				*(level+ind) = 6;
			}
			++ind;
		}
	}
}

int main() {
	
	int height = 32;
	int width = 16;
	int size = height * width;
	
	module::Perlin myModule;
	noise::utils::NoiseMap heightMap;

	utils::NoiseMapBuilderPlane heightMapBuilder;
	
	heightMapBuilder.SetSourceModule(myModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(width, height);
	heightMapBuilder.SetBounds(2.0, 10.0, 1.0, 5.0);
	heightMapBuilder.Build();
	
	// create the window
	sf::RenderWindow window(sf::VideoMode(512, 256), "Tielmap");

	int *level = NULL;
	level = new int[height * width];
	memset(level, 0x00, height*width);
	LoadLevel(level, height, width, heightMap);
	
	TileMap map;
	if (!map.load("tileset.png", sf::Vector2u(32, 32), level, 16, 8))
		return -1;

	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
			myModule.SetSeed(std::rand());
			heightMapBuilder.SetSourceModule(myModule);
			heightMapBuilder.SetDestNoiseMap(heightMap);
			heightMapBuilder.SetDestSize(width, height);
			heightMapBuilder.SetBounds(2.0, 10.0, 1.0, 5.0);
			heightMapBuilder.Build();

			
			if (!map.load("tileset.png", sf::Vector2u(32, 32), level, 16, 8))
				return -1;

		}

		LoadLevel(level, height, width, heightMap);
		// draw the map
		window.clear();
		window.draw(map);
		window.display();
	}
	
	delete [] level;
	system("pause");
	return 0;
}