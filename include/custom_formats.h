//   SPR file

#define SPR_HEADER_SIZE 32
#define SPR_MAGIC "SPR"
#define SPR_MAGIC_OFF 0                            //"SPR"
#define SPR_PAL_COLORS_OFF 3                       // 0-255 (total colors - 1)
#define SPR_SPRCNT_OFF 4                           // 0-n
#define SPR_SPRSIZE_OFF 5                          // 16 or 32
#define SPR_PAL_START 32                           // palette start
#define SPR_SPRITE_START SPR_PAL_START + (256 * 2) // 256 posible 16bit ARGB4444 colors

//   B0 file
#define B0_HEADER_SIZE 8
#define B0_MAGIC "B0L"
#define B0_MAGIC_OFF 0 //"B0L"
#define B0_WIDTH_OFF 4
#define B0_HEIGHT_OFF 6
#define B0_IMAGE_OFF 8

//   TIL file
#define TIL_SIZE 8
#define TIL_HEADER_SIZE 8
#define TIL_MAGIC "TIL"
#define TIL_MAGIC_OFF 0                         //"til"
#define TIL_PAL_COLORS_OFF 3                    // 0-255 (total colors - 1)
#define TIL_TILCNT_OFF 4                        // 1-1024 (16 bit)
#define TIL_PAL_START 8                         // palette start
#define TIL_TIL_START TIL_PAL_START + (256 * 2) // 256 posible 16bit ARGB4444 colors

//   MAP file
#define MAP_HEADER_SIZE 8
#define MAP_HEADER_MAGIC "MAP"
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