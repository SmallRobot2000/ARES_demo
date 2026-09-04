# ARES VDP Memory Interface

The simulator deliberately exposes exactly the same memory-style interface that the demo uses on the FPGA/Linux system. The demo does not need to know whether these pointers refer to `mmap()`'d UIO/DDR memory on ARES or ordinary host memory allocated by the simulator.

All eight pointers are shared with the VDP renderer. The application writes registers, palettes, tile maps, tile graphics, sprite attributes, sprite graphics, and bitmap pixels through these pointers; the simulator continuously reads them and generates the 640×480 display.

## Important Pixel Format

Except for B0 in Linux-compatibility mode, colors used by the VDP are **ARGB4444**:

```text
 15          12 11           8 7            4 3            0
+--------------+--------------+--------------+--------------+
|   A[3:0]     |   R[3:0]     |   G[3:0]     |   B[3:0]     |
+--------------+--------------+--------------+--------------+
```

`A == 0` means transparent to the final compositor. Any non-zero alpha nibble is considered visible. The final hardware VGA output uses only RGB444; the alpha nibble is used for layer transparency/priority.

Final layer priority, highest first:

```text
T1 > S0 > T0 > B0 > background color
```

A layer only wins if it is enabled and its selected ARGB4444 pixel has a non-zero alpha nibble.

---

## `r0` — Registers and Palettes

```c
volatile uint32_t *r0;
```

| Property | Value |
|---|---|
| Element size | 32 bits |
| Simulator size | `0x2000` bytes |
| Indexing | `r0[]` uses 32-bit **word indices**, not byte offsets |

The first 256 words are VDP registers.

### Important Register Indices

#### `r0[0x000]` — `CTRL_0`

| Bit | Meaning |
|---:|---|
| 0 | T0 enable |
| 1 | T1 enable |
| 2 | S0 enable |
| 3 | B0 enable |
| 4 | B0 Linux compatibility / RGB565 mode |

#### `r0[0x001]` — `STAT_0`

| Bit | Meaning |
|---:|---|
| 0 | Horizontal blank |
| 1 | Vertical blank |

Other important registers:

| Register | Meaning |
|---|---|
| `r0[0x002]` | `LINE` — current/generated display line |
| `r0[0x003]` | `BCK_GND` — background color, low 12 bits = RGB444 |
| `r0[0x004]` | `T0_X_OFF` — T0 horizontal scroll, low 10 bits |
| `r0[0x005]` | `T0_Y_OFF` — T0 vertical scroll, low 10 bits |
| `r0[0x008]` | `T1_X_OFF` — T1 horizontal scroll, low 10 bits |
| `r0[0x009]` | `T1_Y_OFF` — T1 vertical scroll, low 10 bits |
| `r0[0x00C]` | `B0_X_OFF` — B0 horizontal offset, low 10 bits |
| `r0[0x00D]` | `B0_Y_OFF` — B0 vertical offset, low 10 bits |

### Palette Memory

`r0` also contains the three palette memories. Each palette entry is stored in the low 16 bits of one `r0[]` word and contains an ARGB4444 color:

```c
r0[0x100 + index]              // T0 palette, 256 entries
r0[0x200 + index]              // T1 palette, 256 entries
r0[0x300 + palette*256+index]  // S0 palette, 4 x 256 entries
```

Tile and sprite graphics do **not** directly contain RGB values. They contain 8-bit palette indices, which are translated through these palette areas.

Palette index `0` is normally initialized to `0x0000` and therefore acts as transparent, although software is free to rewrite it.

---

## `b0` — Bitmap Layer 0

```c
volatile uint16_t *b0;
```

| Property | Value |
|---|---|
| Element size | 16 bits per pixel |
| Reserved size | `0x00800000` bytes = 8 MiB |

B0 has two operating modes, selected by `CTRL_0` bit 4.

### Normal Mode (`CTRL_0` bit 4 = 0)

`b0[]` contains ARGB4444 pixels directly. The logical bitmap is addressed as a 1024-pixel-wide surface.

For screen coordinate `(x, y)`, the VDP starts each displayed line at:

```c
((y + B0_Y_OFF) * 1024) + B0_X_OFF
```

and then reads 640 consecutive 16-bit pixels.

Example:

```c
b0[y * 1024 + x] = 0xFF00;  // opaque red
b0[y * 1024 + x] = 0x0000;  // transparent
```

`B0_X_OFF` and `B0_Y_OFF` provide viewport scrolling.

As in the Verilog, a 640-pixel line is a linear read; there is no special horizontal wrap operation when `B0_X_OFF + x` passes the end of a 1024-pixel row.

### Linux Compatibility Mode (`CTRL_0` bit 4 = 1)

`b0[]` is treated as a tightly packed 640×480 RGB565 framebuffer:

```c
index = y * 640 + x;
```

RGB565 format:

| Bits | Component |
|---|---|
| 15:11 | Red |
| 10:5 | Green |
| 4:0 | Blue |

The FPGA converts RGB565 to opaque ARGB4444 by selecting:

```text
A = 0xF
R = RGB565[15:12]
G = RGB565[10:7]
B = RGB565[4:1]
```

`B0_X_OFF` and `B0_Y_OFF` are ignored in Linux-compatibility mode.

---

## `t0_map` / `t1_map` — Tile Maps

```c
volatile uint16_t *t0_map;
volatile uint16_t *t1_map;
```

| Property | Value |
|---|---|
| Element size | 16 bits per map entry |
| Size each | `0x8000` bytes = 32768 bytes |
| Logical map | 128 × 128 entries = 16384 entries |
| Tile size | 8 × 8 pixels |
| Logical world | 1024 × 1024 pixels |

Map addressing is row-major:

```c
map_index = tile_y * 128 + tile_x;
```

Example:

```c
t0_map[20 * 128 + 10] = 37;
```

This places tile number 37 at tile coordinate `(10, 20)`.

### Current Map Entry Format

Only bits 9:0 are currently used by the Verilog:

| Bits | Meaning |
|---|---|
| 9:0 | Tile number, 0..1023 |
| 15:10 | Currently ignored/reserved |

T0 uses `T0_X_OFF` / `T0_Y_OFF` and T1 uses `T1_X_OFF` / `T1_Y_OFF`.

Offsets are in **pixels**, not tiles, so sub-tile scrolling is supported.

Because the map is 128 tiles × 8 pixels = 1024 pixels in each direction, tile-map scrolling naturally wraps through the 10-bit/128×128 addressing used by the hardware.

---

## `t0_data` / `t1_data` — Tile Pixel Data

```c
volatile uint8_t *t0_data;
volatile uint8_t *t1_data;
```

| Property | Value |
|---|---|
| Element size | 8-bit palette index |
| Size each | `0x10000` bytes = 65536 bytes |
| Number of tiles | 1024 |
| Bytes per tile | 64 |
| Tile dimensions | 8 × 8 pixels |

Each byte is an index into the corresponding 256-entry tile palette in `r0`.

Tile data is laid out consecutively, row-major inside each tile:

```c
address = tile_number * 64 + pixel_y * 8 + pixel_x;
```

where `pixel_x` and `pixel_y` are both `0..7`.

Example:

```c
t0_data[37 * 64 + 2 * 8 + 5] = 7;
```

This sets pixel `(5, 2)` of T0 tile 37 to palette index 7, so the displayed color is:

```c
r0[0x100 + 7] & 0xffff
```

T1 is identical except it uses `t1_data` and the palette beginning at:

```c
r0[0x200]
```

---

## `s0_att` — Sprite Attribute Table

```c
volatile uint64_t *s0_att;
```

| Property | Value |
|---|---|
| Element size | 64 bits per sprite |
| Size | `0x0200` bytes = 512 bytes |
| Number of entries | 64 sprites |

Each `s0_att[n]` is one complete sprite attribute record:

| Bits | Field | Meaning |
|---|---|---|
| 63 | `ACTIVE` | Sprite enabled |
| 54:53 | `SCALE` | Present in format, currently **unused by HDL** |
| 52 | `SIZE` | `0` = 16×16, `1` = 32×32 |
| 51 | `V_FLIP` | Vertical flip |
| 50 | `H_FLIP` | Horizontal flip |
| 49:48 | `PALETTE` | Palette number 0..3 |
| 46:36 | `DATA OFFSET` | 11-bit address into S0 data, in 16-byte words |
| 25:16 | `Y` | Y position 0..1023 |
| 9:0 | `X` | X position 0..1023 |

Unlisted bits are currently unused/reserved.

### Example Attribute Construction

```c
uint64_t a = 0;

a |= (uint64_t)(x & 0x3ff) << 0;
a |= (uint64_t)(y & 0x3ff) << 16;
a |= (uint64_t)(offset & 0x7ff) << 36;
a |= (uint64_t)(palette & 3) << 48;
a |= (uint64_t)(hflip & 1) << 50;
a |= (uint64_t)(vflip & 1) << 51;
a |= (uint64_t)(size & 1) << 52;
a |= UINT64_C(1) << 63;  // active

s0_att[n] = a;
```

### Data Offset

`DATA OFFSET` is **not a byte offset**. It is the address of a 128-bit / 16-byte sprite-data word in `s0_data`.

For a 16×16 sprite:

```text
one 16-byte word stores one complete 16-pixel row
row address     = offset + row
complete sprite = 16 words = 256 bytes
```

For a 32×32 sprite:

```text
two 16-byte words store one 32-pixel row
row address     = offset + row*2
complete sprite = 64 words = 1024 bytes
```

Vertical flip reverses the selected source row. Horizontal flip reverses the source pixel order across that row.

The hardware examines up to 64 attribute records, but draws at most 32 active sprites intersecting one scanline (`MAX_SPR_LINE = 32`).

Transparent sprite pixels do not overwrite the sprite line buffer. Because attributes are processed sequentially, a later sprite attribute can overwrite visible pixels produced by an earlier sprite when they overlap.

---

## `s0_data` — Sprite Pixel Data

```c
volatile uint8_t *s0_data;
```

| Property | Value |
|---|---|
| Element size | 8-bit palette index |
| Size | `0x8000` bytes = 32768 bytes |
| Hardware view | 2048 × 128-bit words = 2048 × 16 bytes |

Each byte is one sprite pixel palette index. A 128-bit hardware read obtains 16 consecutive pixels at once. Palette selection comes from `s0_att[]`.

The displayed ARGB4444 color for a byte value `index` is:

```c
r0[0x300 + palette_number * 256 + index] & 0xffff
```

### 16×16 Sprite

For a 16×16 sprite whose attribute `OFFSET` is `off`:

```c
s0_data[(off + row) * 16 + x]
```

gives source pixel `x = 0..15`, `row = 0..15`.

### 32×32 Sprite

```c
word = off + row*2 + x/16;
pixel = s0_data[word*16 + (x & 15)];
```

where `x = 0..31` and `row = 0..31`.

The pixel byte itself has no alpha bit. Transparency comes from the selected palette color: if that ARGB4444 palette entry has `A == 0`, the sprite pixel is transparent and does not overwrite what is behind it.

---

## Quick Memory Summary

| Pointer | Type | Size | Meaning |
|---|---|---:|---|
| `r0` | `uint32_t` | `0x2000` | Registers + T0/T1/S0 palettes |
| `b0` | `uint16_t` | `0x800000` | Bitmap storage |
| `t0_map` | `uint16_t` | `0x008000` | 128×128 T0 tile-number map |
| `t0_data` | `uint8_t` | `0x010000` | 1024 × 8×8 T0 palette-index tiles |
| `t1_map` | `uint16_t` | `0x008000` | 128×128 T1 tile-number map |
| `t1_data` | `uint8_t` | `0x010000` | 1024 × 8×8 T1 palette-index tiles |
| `s0_att` | `uint64_t` | `0x000200` | 64 sprite attribute records |
| `s0_data` | `uint8_t` | `0x008000` | Sprite palette-index pixel data |
