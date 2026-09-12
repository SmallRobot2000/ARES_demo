#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <tinyxml2.h>
#include <fstream>

using namespace tinyxml2;

#define MAP_HEADER_SIZE 8
#define MAP_HEADER_MAGIC_OFF 0
#define MAP_HEADER_LAYER_CNT_OFF 3 // One byte
#define MAP_LAYER_TABLE_OFF MAP_HEADER_SIZE
#define MAP_MAX_LAYERS 16
#define MAP_LAYER_TABLE_ENTRY_SIZE 16
#define MAP_LAYER_TABLE_SIZE (MAP_MAX_LAYERS * MAP_LAYER_TABLE_ENTRY_SIZE)
#define MAP_LAYER_TABLE_NAME_OFF 0
#define MAP_MAX_NAME_LEN MAP_LAYER_TABLE_ENTRY_SIZE - 1
#define MAP_LAYER_TABLE_ID_OFF MAP_LAYER_TABLE_ENTRY_SIZE - 1 // Layer table entry is the name of the layer(max MAP_LAYER_TABLE_ENTRY_SIZE-1) and ID 0-255
#define MAP_LAYER_DATA_OFF (MAP_LAYER_TABLE_OFF + (MAP_MAX_LAYERS * MAP_LAYER_TABLE_ENTRY_SIZE))
#define MAP_LAYER_DATA_ENTRY_SIZE 128 * 128 * 2 // One layer data size
int main(int argc, char *argv[])
{
    XMLDocument doc;

    int size = 0;
    if (argc != 3)
    {
        printf("Usage: %s <input_file.tmx> <output_file.map>\n", argv[0]);
        return 1;
    }

    if (doc.LoadFile(argv[1]) != XML_SUCCESS)
    {
        printf("Failed to load %s\n", argv[1]);
        return 1;
    }
    std::ofstream file(argv[2], std::ios::out | std::ios::binary);

    if (!file)
    {
        printf("Failed to open %s\n", argv[2]);
        return 1;
    }

    XMLElement *map = doc.FirstChildElement("map");

    if (!map)
        return 1;

    int width = map->IntAttribute("width");
    int height = map->IntAttribute("height");
    int tileWidth = map->IntAttribute("tilewidth");
    int tileHeight = map->IntAttribute("tileheight");

    printf("Map: %d x %d\n", width, height);
    printf("Tile: %d x %d\n", tileWidth, tileHeight);

    int layer_cnt = 0;

    uint16_t layer_data[(MAP_LAYER_DATA_ENTRY_SIZE * MAP_MAX_LAYERS) / 2] = {};
    uint8_t layer_table[MAP_LAYER_TABLE_SIZE] = {};
    char real_name[MAP_MAX_NAME_LEN + 1];
    for (XMLElement *layer = map->FirstChildElement("layer");
         layer;
         layer = layer->NextSiblingElement("layer"))
    {
        const char *name = layer->Attribute("name");
        int layer_id = layer->IntAttribute("id");

        int layerWidth = layer->IntAttribute("width");
        int layerHeight = layer->IntAttribute("height");

        // printf("Layer: %s (%d x %d) ID: %d\n",
        //        name ? name : "(unnamed)",
        //        layerWidth,
        //        layerHeight,
        //        layer_id);

        XMLElement *data = layer->FirstChildElement("data");

        if (!data)
            continue;

        const char *encoding = data->Attribute("encoding");

        if (!encoding || strcmp(encoding, "csv") != 0)
        {
            printf("Only CSV encoding supported\n");
            continue;
        }

        if (layerHeight != 128 || layerWidth != 128)
        {
            printf("Only 128x128 tile layers supported\n");
            continue;
        }
        const char *text = data->GetText();

        if (!text)
            continue;

        const char *p = text;

        memset(real_name, '\0', MAP_MAX_NAME_LEN + 1);
        if (name != NULL)
        {
            strlcpy(real_name, name, sizeof(real_name));
            if (strlen(name) > MAP_MAX_NAME_LEN)
            {
                printf("Layer name %s is too long it will be renamed to %s", name, real_name);
            }
        }
        else
        {
            strcpy(real_name, "(unnamed)");
        }

        // Make layer table entry

        memset(&layer_table[layer_cnt], 0, MAP_LAYER_TABLE_ENTRY_SIZE);                                             // Clear
        strlcpy((char *)&layer_table[layer_cnt + MAP_LAYER_TABLE_NAME_OFF], real_name, MAP_LAYER_TABLE_ENTRY_SIZE); // Leading zero if on last place will be replaced with ID
        layer_table[layer_cnt + MAP_LAYER_TABLE_ID_OFF] = (uint8_t)layer_id;

        for (int i = 0; i < layerHeight * layerWidth; i++)
        {

            while (*p == ' ' ||
                   *p == '\n' ||
                   *p == '\r' ||
                   *p == '\t' ||
                   *p == ',')
            {
                p++;
            }

            uint32_t gid = strtoul(p, (char **)&p, 10);

            layer_data[i + layer_cnt * MAP_LAYER_DATA_ENTRY_SIZE] = (uint16_t)gid - 1; // In tiled ids start from 1
        }

        layer_cnt++;

        if (layer_cnt >= MAP_MAX_LAYERS)
        {
            printf("Maximum of %d layers\n", MAP_MAX_LAYERS);
            return 1;
        }

        printf("Layer %d: ID: %d Name: %s\n", layer_cnt - 1, layer_id, real_name);
    }
    uint8_t header[MAP_HEADER_SIZE] = {};

    memcpy(&header[MAP_HEADER_MAGIC_OFF], "MAP", 3);
    header[MAP_HEADER_LAYER_CNT_OFF] = layer_cnt;
    file.write(reinterpret_cast<char *>(header), sizeof(header));
    file.write(reinterpret_cast<char *>(layer_table), sizeof(layer_table));
    file.write(reinterpret_cast<char *>(layer_data), MAP_LAYER_DATA_ENTRY_SIZE * layer_cnt);
    if (!file)
    {
        printf("Failed to write %s\n", argv[2]);
        return 1;
    }

    file.close();

    return 0;
}