/*
  WildStorage
  A tool for ACWW to add extra item storage

  Copyright (c) 2024 NovaSquirrel

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  -----

  This program references research and code from Animal Map

  (c) 2005-2007 DsPet
  Released as FreeWare and Open-source
  Additional modifications thanks to Virus of Haxxor World
*/
#include <nds.h>
#include "wild.h"
#include "acstr.h"

// ------------------------------------------------------------------------------------------------
// Data

// Generated with the following Python code:
/*
from PIL import Image
im = Image.open('acww_palettes.png')

first_color_index = 16

pixels = list(im.getdata())
width, height = im.size
assert width == 15
assert height == 16

shared_color_list = []
map_to_shared_colors = []

# Read and process
for y in range(height):
	row = pixels[y * width:(y + 1) * width]

	map_for_row = []
	for p in row:
		if p in shared_color_list:
			map_for_row.append(shared_color_list.index(p))
		else:
			map_for_row.append(len(shared_color_list))
			shared_color_list.append(p)
	map_to_shared_colors.append(map_for_row)

# Output everything
print("#define PATTERN_SHARED_COLOR_COUNT %d" % len(shared_color_list))

print("const unsigned char  pattern_map_to_shared[] = {")
for palette in map_to_shared_colors:
	print("  0, " + ", ".join(["%3d" % (x+first_color_index) for x in palette]) + ", ")
print("};")

def convert_to_bgr555(t):
	r = round(t[0] / 8)
	g = round(t[1] / 8)
	b = round(t[2] / 8)
	return r | (g << 5) | (b << 10)

print("const unsigned short pattern_shared_colors[] __attribute__((aligned(4))) = {")
print("  " + ", ".join(["0x%.4x" % convert_to_bgr555(x) for x in shared_color_list]))
print("};")
*/

#define PATTERN_SHARED_COLOR_COUNT 168
#define PATTERN_SHARED_COLOR_STARTS_AT 32

const unsigned char  pattern_map_to_shared[] = {
  0,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,
  0,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  45,  46,
  0,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  45,  46,
  0,  73,  74,  75,  76,  77,  78,  40,  79,  80,  41,  81,  82,  83,  84,  46,
  0,  85,  86,  87,  88,  89,  90,  43,  91,  92,  93,  94,  95,  96,  97,  46,
  0,  32,  98,  99, 100,  97, 101, 102,  93,  80, 103, 104, 105, 106,  45,  46,
  0,  38, 107, 108, 109,  41, 110, 111, 112,  32, 113, 114, 115, 106,  45,  46,
  0, 116, 117, 118, 119,  97, 120, 121, 122, 123, 124, 125, 126, 106,  45,  46,
  0,  41,  32,  35, 110, 113, 127, 111, 114, 128, 112, 115, 129, 106,  45,  46,
  0,  38,  41,  44, 107, 110, 130, 108, 111, 131, 109, 112, 132, 106,  45,  46,
  0,  32, 133,  35, 134,  38, 135,  41, 136, 137, 123, 138, 139, 106,  45,  46,
  0, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,  46,
  0, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163,  35, 164, 165,  45,  46,
  0,  46, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
  0,  46, 180, 181, 182, 183, 184, 185, 186, 106, 187, 188, 189, 190, 191,  45,
  0, 192,  32, 133,  35, 193,  38,  41, 194, 195, 196, 197, 198, 199,  45,  46,
};

const unsigned short pattern_shared_colors[] __attribute__((aligned(4))) = {
  0x001f, 0x19df, 0x02bf, 0x03ff, 0x03f5, 0x03ea, 0x03e0, 0x2aa0, 0x5540, 0x7c00, 0x7c0a, 0x7c15, 0x7c1f, 0x0000, 0x7fff, 0x3dff, 0x3edf, 0x3f9f, 0x3fff, 0x3ffb, 0x3ff5, 0x3fef, 0x42aa, 0x560a, 0x7def, 0x7df6, 0x7dfc, 0x7dff, 0x0014, 0x00d4, 0x01d4, 0x0294, 0x028e, 0x0286, 0x0280, 0x1140, 0x2880, 0x5000, 0x5006, 0x500e, 0x5014, 0x0260, 0x372b, 0x6ff6, 0x3660, 0x532a, 0x6bf5, 0x6a05, 0x7eab, 0x7da9, 0x6d26, 0x5883, 0x4400, 0x01d5, 0x22ba, 0x477f, 0x1c3f, 0x353f, 0x4e5f, 0x7d9a, 0x7f3f, 0x4eff, 0x3a5b, 0x2597, 0x10f3, 0x004f, 0x015f, 0x2edf, 0x57bf, 0x1934, 0x361a, 0x7f10, 0x7f95, 0x7ffa, 0x35ad, 0x23e8, 0x47f1, 0x6bfa, 0x7d08, 0x7e31, 0x7f5a, 0x211f, 0x463f, 0x6b5f, 0x00c0, 0x2188, 0x4270, 0x6358, 0x1534, 0x2e3a, 0x471f, 0x02da, 0x1f3c, 0x3f7e, 0x5fdf, 0x23ff, 0x47ff, 0x6bff, 0x7d1f, 0x7e3f, 0x7f5f, 0x01ff, 0x03f0, 0x3e00, 0x7c0f, 0x7e5f, 0x0057, 0x004b, 0x3242, 0x29e1, 0x1e22, 0x1a66, 0x2699, 0x1e59, 0x2637, 0x1a3a, 0x25d5, 0x1971, 0x150d, 0x7fb0, 0x7726, 0x6280, 0x737a, 0x6f36, 0x77bc, 0x7bde, 0x3dd0, 0x3632, 0x31f0, 0x2e13, 0x5a6e, 0x14bf, 0x7c92, 0x5e60, 0x77be, 0x6f7c, 0x673a, 0x5ad8, 0x5296, 0x4a54, 0x4213, 0x35b1, 0x2d6f, 0x252d, 0x18cb, 0x1089, 0x0848, 0x0006, 0x77bd, 0x6f7b, 0x6739, 0x5ad6, 0x5294, 0x4a52, 0x4210, 0x2d6b, 0x2529, 0x18c6, 0x1084, 0x0842, 0x3e3f, 0x0200, 0x7e60, 0x7c1a, 0x7dbf, 0x0013, 0x025f, 0x4aff
};

typedef struct acww_name {
  u16 id;
  u8 name[8];
} acww_name;

// Based on the notes in RAC as a starting point
struct acww_pattern {
  u8 data[32][16]; // data[row][column], where each byte has the left pixel in the lower nybble and right pixel in the upper nybble

  struct acww_name author_town;
  struct acww_name author;

  u16 unknown; // Unknown,seems to be 0 for custom patterns, 1 for the premade you start out with, and a Wendell pattern had $021d?

  u8 pattern_name[16];

  u16 unknown2;
  // 0000 0000 pppp ????
  // |||| |||| |||| ++++- Values observed here: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  // |||| |||| ||||       Might be what gets saved from the three options you get after designing? But there's more than 3 values.
  // |||| |||| ||||       2 = "Formal, refined"
  // |||| |||| ||||       9 = "Basic, yet new!"
  // |||| |||| ||||       0 7 = "Didn't have one"
  // |||| |||| ++++------ Palette used (0-15)
  // ++++-++++----------- I've only seen zero here so far
};

// ------------------------------------------------------------------------------------------------
// Strings
//const char *edit_player_options[] = {"Emotions", "Hair style", "Hair color", "Face", "Gender", "Tan", "Birth month", "Birth day"};

// ------------------------------------------------------------------------------------------------
// Variables
int pattern_select_x;
int pattern_select_y;

// ------------------------------------------------------------------------------------------------
// Functions

void clear_screen_256(u16 *map) {
	dmaFillHalfWords(0, map, 32*32*2);
}

void copy_pattern_to_vram(struct acww_pattern *pattern, u16 *vram) {
	u8 palette_offset = pattern->unknown2 & 0xf0;

	for(int y=0; y<32; y++) {
		int row_offset = y << 2;
		for(int x=0; x<16; x++) {
			u8 byte = pattern->data[y][x];
			u8 left_pixel  = pattern_map_to_shared[palette_offset + (byte & 15)];
			u8 right_pixel = pattern_map_to_shared[palette_offset + (byte >> 4)];
			vram[row_offset | (x & 3) | ((x & 0xC) << 5)] = left_pixel | (right_pixel << 8);
			// Index is: xxyyyyyxx
		}
	}
}

void pattern_tiles_at(u16 *map, int base_x, int base_y, int initial) {
	int tile = initial;
	for(int x=0; x<4; x++) {
		for(int y=0; y<4; y++) {
			map[(base_x+x)+32*(base_y+y)] = tile++;
		}
	}
}

void draw_selection_box_at(u16 *map, int x, int y) {
	map_put(mainBGMapText,   x,   y, TILE_SELECTION_BOX_TOP_LEFT);
	map_put(mainBGMapText,   x+1, y, TILE_SELECTION_BOX_TOP);
	map_put(mainBGMapText,   x+2, y, TILE_SELECTION_BOX_TOP);
	map_put(mainBGMapText,   x+3, y, TILE_SELECTION_BOX_TOP_LEFT | TILE_FLIP_H);
	map_put(mainBGMapText,   x,   y+1, TILE_SELECTION_BOX_LEFT);
	map_put(mainBGMapText,   x+3, y+1, TILE_SELECTION_BOX_LEFT | TILE_FLIP_H);
	map_put(mainBGMapText,   x,   y+2, TILE_SELECTION_BOX_LEFT);
	map_put(mainBGMapText,   x+3, y+2, TILE_SELECTION_BOX_LEFT | TILE_FLIP_H);
	map_put(mainBGMapText,   x,   y+3, TILE_SELECTION_BOX_TOP_LEFT | TILE_FLIP_V);
	map_put(mainBGMapText,   x+1, y+3, TILE_SELECTION_BOX_TOP | TILE_FLIP_V);
	map_put(mainBGMapText,   x+2, y+3, TILE_SELECTION_BOX_TOP | TILE_FLIP_V);
	map_put(mainBGMapText,   x+3, y+3, TILE_SELECTION_BOX_TOP_LEFT | TILE_FLIP_H | TILE_FLIP_V);
}

void redraw_pattern_manager_bottom_screen() {

}

void menu_patterns() {
	pattern_select_x = 0;
	pattern_select_y = 0;
	bool first_draw = true;

	clear_screen(mainBGMapText);
	clear_screen(subBGMapText);
	clear_screen_256(mainBGMap256);
	clear_screen_256(subBGMap256);

	u16 *topChr    = bgGetGfxPtr(subBG256);
	u16 *bottomChr = bgGetGfxPtr(mainBG256);

	// Bottom screen: patterns in the savefile
	for(int player=0; player<4; player++) {
		map_print(mainBGMapText, 1, 6*player, player_name(player));
		for(int pattern=0; pattern<8; pattern++) {
			int vram_index = player*8+pattern+1;
			pattern_tiles_at(mainBGMap256, 4*pattern, 6*player+1, 16*vram_index);
			copy_pattern_to_vram((struct acww_pattern*)&savefile[0x0000C + sizeof(struct acww_pattern)*pattern + PER_PLAYER_OFFSET*player], bottomChr+512*vram_index);
		}
	}

	// Top screen: external patterns
	for(int player=0; player<4; player++) {
		for(int pattern=0; pattern<8; pattern++) {
			int vram_index = player*8+pattern+1;
			pattern_tiles_at(subBGMap256, 4*pattern, 5*player+1, 16*vram_index);
			copy_pattern_to_vram((struct acww_pattern*)&savefile[0x0000C + sizeof(struct acww_pattern)*pattern + PER_PLAYER_OFFSET*0], topChr+512*vram_index);
		}
	}

	dmaCopy(pattern_shared_colors,   BG_PALETTE+PATTERN_SHARED_COLOR_STARTS_AT,     sizeof(pattern_shared_colors));
	dmaCopy(pattern_shared_colors,   BG_PALETTE_SUB+PATTERN_SHARED_COLOR_STARTS_AT, sizeof(pattern_shared_colors));

	map_box(subBGMapText,  0,  20, 32, 4);
//	map_print(subBGMapText, 1, 1,  "Pattern manager");
//	map_print(subBGMapText, 1, 2, "\xe0:Move \xe1:Jump \xe2:Menu \xe3:????");
	map_print(subBGMapText, 1, 0, "\xe0:Move \xe1:Jump \xe2:Menu \xe3:????");

	// ------------------------------------------

	// UI loop
	while(1) {
		wait_vblank_and_animate();
		scanKeys();

		uint32_t keys_down = keysDown();
		uint32_t keys_repeat = 	keysDownRepeat();

		int old_pattern_select_x = pattern_select_x;
		int old_pattern_select_y = pattern_select_y;

		// Move the selection around
		if(keys_repeat & KEY_UP) {
			pattern_select_y = (pattern_select_y-1)&3;
		}
		if(keys_repeat & KEY_DOWN) {
			pattern_select_y = (pattern_select_y+1)&3;
		}
		if(keys_repeat & KEY_LEFT) {
			pattern_select_x = (pattern_select_x-1)&7;
		}
		if(keys_repeat & KEY_RIGHT) {
			pattern_select_x = (pattern_select_x+1)&7;
		}
		if(first_draw || pattern_select_x != old_pattern_select_x || pattern_select_y != old_pattern_select_y) {
			map_put(mainBGMapText,   old_pattern_select_x*4+1, 6*old_pattern_select_y+5, ' ');
			map_put(mainBGMapText,   old_pattern_select_x*4+2, 6*old_pattern_select_y+5, ' ');
			map_rectfill(mainBGMapText, old_pattern_select_x*4, 6*old_pattern_select_y+1, 4, 4, ' ');

			map_rectfill(subBGMapText, 1, 21, 30, 2, ' ');
			struct acww_pattern *pattern = (struct acww_pattern*)&savefile[0x0000C + sizeof(struct acww_pattern)*pattern_select_x + PER_PLAYER_OFFSET*pattern_select_y];

			acstrDecode(text_conversion_buffer, pattern->pattern_name, 16);
			map_print(subBGMapText, 1, 21, text_conversion_buffer);

			acstrDecode(text_conversion_buffer, pattern->author.name, 8);
			map_print(subBGMapText, 1, 22, text_conversion_buffer);

			acstrDecode(text_conversion_buffer, pattern->author_town.name, 8);
			map_print(subBGMapText, 16, 22, text_conversion_buffer);

			first_draw = true;
		}
		map_put(mainBGMapText,   pattern_select_x*4+1, 6*pattern_select_y+5, TILE_VERTICAL_PICKER_L);
		map_put(mainBGMapText,   pattern_select_x*4+2, 6*pattern_select_y+5, TILE_VERTICAL_PICKER_R);
		draw_selection_box_at(mainBGMap256, pattern_select_x*4, 6*pattern_select_y+1);

		// Exit
		if(keys_down & KEY_START)
			break;
	}

	clear_screen_256(mainBGMap256);
	clear_screen_256(subBGMap256);
}