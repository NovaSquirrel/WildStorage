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
// Data tables

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

// ------------------------------------------------------------------------------------------------
// Structures

typedef struct acww_name {
  u16 id;
  u8 name[8];
} acww_name;

// Based on the notes in RAC as a starting point
// 552 byte structure
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
  // |||| |||| ||||       Needs more research!
  // |||| |||| ++++------ Palette used (0-15)
  // ++++-++++----------- I've only seen zero here so far
};

// ------------------------------------------------------------------------------------------------
// Strings
const char *edit_pattern_options[] = {"Edit pattern", "Load from file", "Save to file", "Copy pattern", "Paste pattern", "Copy author", "Paste author", "Delete", "View pattern info"};
const char *bottom_screen_pattern_row_names[] = {"Able Sisters", "Villagers", "Town Flag", "Blanca"};

// ------------------------------------------------------------------------------------------------
// Variables
int pattern_select_x;
int pattern_select_y;
int pattern_select_screen;
int pattern_select_page[2];
int pattern_edit_option = 0;

// Swap status
bool pattern_swapping = false;
int pattern_swap_x;
int pattern_swap_y;
int pattern_swap_screen;
int pattern_swap_page;
struct acww_pattern *pattern_swap_pointer;

struct acww_pattern copied_pattern;
bool have_copied_pattern = false;

struct acww_name copied_author;
struct acww_name copied_author_town;
bool have_copied_name = false;

struct acww_pattern extra_pattern_storage[EXTRA_PATTERN_STORAGE_SIZE];
bool edited_extra_patterns = false;

// ------------------------------------------------------------------------------------------------
// Functions
extern bool has_acww_folder;
int makeFolder(const char *path);

void load_extra_patterns() {
	const char *town = town_name();

	// Empty out the array first
	memset(extra_pattern_storage, 0, sizeof(extra_pattern_storage));

	sprintf(full_file_path, "%sextra_%s_patterns.bin", acww_folder_prefix, town);
	FILE *f = fopen(full_file_path, "rb");
	if(f) {
		if(fread(extra_pattern_storage, 1, sizeof(extra_pattern_storage), f) != sizeof(extra_pattern_storage))
			popup_notice("Bad extra patterns file?");
		fclose(f);
	}
}

void save_extra_patterns() {
	const char *town = town_name();

	if(edited_extra_patterns) {
		sprintf(full_file_path, "%sextra_%s_patterns.bin", acww_folder_prefix, town);
		FILE *f = fopen(full_file_path, "wb");
		if(f) {
			if(fwrite(extra_pattern_storage, 1, sizeof(extra_pattern_storage), f) != sizeof(extra_pattern_storage))
				popup_notice("Couldn't save patterns file");
			fclose(f);
		} else {
			popup_notice("Couldn't create patterns file");
		}

		edited_extra_patterns = false;
	}
}

struct acww_pattern *get_pattern_for_slot(int screen, int slot_x, int slot_y) {
	if(screen == 0) {
		if(pattern_select_page[0] == 0) { // Players
			return (struct acww_pattern*)&savefile[0x0000C + sizeof(struct acww_pattern)*slot_x + PER_PLAYER_OFFSET*slot_y];
		} else {
			switch(slot_y) {
				case 0: // Able Sisters
					return (struct acww_pattern*)&savefile[0x0FAFC + sizeof(struct acww_pattern)*slot_x];
				case 1: // Villager
					return (struct acww_pattern*)&savefile[0x08D7C + 0x700 * slot_x];
				case 2: // Flag
					return (struct acww_pattern*)&savefile[0x15930];
				case 3: // Blanca
					return (struct acww_pattern*)&savefile[0x15700];
			}
		}
	}
	return &extra_pattern_storage[slot_x + slot_y*8 + pattern_select_page[1]*32];
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
	map_put(map, x,   y, TILE_SELECTION_BOX_TOP_LEFT);
	map_put(map, x+1, y, TILE_SELECTION_BOX_TOP);
	map_put(map, x+2, y, TILE_SELECTION_BOX_TOP);
	map_put(map, x+3, y, TILE_SELECTION_BOX_TOP_LEFT | TILE_FLIP_H);
	map_put(map, x,   y+1, TILE_SELECTION_BOX_LEFT);
	map_put(map, x+3, y+1, TILE_SELECTION_BOX_LEFT | TILE_FLIP_H);
	map_put(map, x,   y+2, TILE_SELECTION_BOX_LEFT);
	map_put(map, x+3, y+2, TILE_SELECTION_BOX_LEFT | TILE_FLIP_H);
	map_put(map, x,   y+3, TILE_SELECTION_BOX_TOP_LEFT | TILE_FLIP_V);
	map_put(map, x+1, y+3, TILE_SELECTION_BOX_TOP | TILE_FLIP_V);
	map_put(map, x+2, y+3, TILE_SELECTION_BOX_TOP | TILE_FLIP_V);
	map_put(map, x+3, y+3, TILE_SELECTION_BOX_TOP_LEFT | TILE_FLIP_H | TILE_FLIP_V);
}

void redraw_pattern_manager_top_screen(int update_map, int update_tiles) {
	if(update_map) {
		clear_screen(subBGMapText);
		clear_screen_256(subBGMap256);

		map_box(subBGMapText,  0,  20, 32, 4);
		map_print(subBGMapText, 1, 0, "\xe0:Move \xe1:Jump \xe2:Menu");// \xe3:Copy");
		map_printf(subBGMapText, 30, 0, "%.2d", pattern_select_page[1]+1);
	}

	u16 *topChr    = bgGetGfxPtr(subBG256);

	// Top screen: external patterns
	for(int player=0; player<4; player++) {
		for(int pattern=0; pattern<8; pattern++) {
			int vram_index = player*8+pattern+1;
			if(update_map)
				pattern_tiles_at(subBGMap256, 4*pattern, 5*player+1, 16*vram_index);
			if(update_tiles)
				copy_pattern_to_vram(get_pattern_for_slot(1, pattern, player), topChr+512*vram_index);
		}
	}
}

void redraw_pattern_manager_bottom_screen(int update_map, int update_tiles) {
	if(update_map) {
		clear_screen(mainBGMapText);
		clear_screen_256(mainBGMap256);
	}

	u16 *bottomChr = bgGetGfxPtr(mainBG256);

	// Bottom screen: patterns in the savefile
	for(int player=0; player<4; player++) {
		if(update_map) {
			map_print(mainBGMapText, 1, 6*player, pattern_select_page[0] ? bottom_screen_pattern_row_names[player] : player_name(player));
		}
		for(int pattern=0; pattern<8; pattern++) {
			int vram_index = player*8+pattern+1;
			if(update_map)
				pattern_tiles_at(mainBGMap256, 4*pattern, 6*player+1, 16*vram_index);
			if(update_tiles)
				copy_pattern_to_vram(get_pattern_for_slot(0, pattern, player), bottomChr+512*vram_index);
		}
	}
}

void erase_pattern_swap_cursor() {
	if(pattern_swap_screen == 0) {
		map_put(mainBGMapText,      pattern_swap_x*4+1, 6*pattern_swap_y+5, ' ');
		map_put(mainBGMapText,      pattern_swap_x*4+2, 6*pattern_swap_y+5, ' ');
	} else {
		map_put(subBGMapText,       pattern_swap_x*4+1, 5*pattern_swap_y+5, pattern_swap_y == 3 ? TILE_BORDER_HORIZ : ' ');
		map_put(subBGMapText,       pattern_swap_x*4+2, 5*pattern_swap_y+5, pattern_swap_y == 3 ? TILE_BORDER_HORIZ : ' ');
	}
}

void menu_patterns() {
	pattern_select_x = 0;
	pattern_select_y = 0;
	pattern_select_screen = 0;
	pattern_select_page[0] = 0;
	pattern_select_page[1] = 0;
	pattern_swapping = false;
	bool first_draw = true;

	redraw_pattern_manager_top_screen(1, 1);
	redraw_pattern_manager_bottom_screen(1, 1);

	dmaCopy(pattern_shared_colors,   BG_PALETTE+PATTERN_SHARED_COLOR_STARTS_AT,     sizeof(pattern_shared_colors));
	dmaCopy(pattern_shared_colors,   BG_PALETTE_SUB+PATTERN_SHARED_COLOR_STARTS_AT, sizeof(pattern_shared_colors));

	// ------------------------------------------

	char *original_directory = getcwd(NULL, 0);

	// UI loop
	while(1) {
		wait_vblank_and_animate();
		scanKeys();

		uint32_t keys_down = keysDown();
		uint32_t keys_repeat = 	keysDownRepeat();

		int old_pattern_select_x = pattern_select_x;
		int old_pattern_select_y = pattern_select_y;
		int old_pattern_select_screen = pattern_select_screen;
		int old_pattern_select_page = pattern_select_page[pattern_select_screen];

		// Move the selection around
		if(keys_repeat & KEY_LEFT) {
			pattern_select_x = (pattern_select_x - 1) & 7;
			if(pattern_select_x == 7) {
				if(pattern_select_screen == 0)
					pattern_select_page[0] ^= 1;
				else
					pattern_select_page[1] = (pattern_select_page[1] - 1) & 31;
			}
		}
		if(keys_repeat & KEY_RIGHT) {
			pattern_select_x = (pattern_select_x + 1) & 7;
			if(pattern_select_x == 0) {
				if(pattern_select_screen == 0)
					pattern_select_page[0] ^= 1;
				else
					pattern_select_page[1] = (pattern_select_page[1] + 1) & 31;
			}
		}
		if(pattern_select_screen == 0 && pattern_select_page[0] != old_pattern_select_page)
			redraw_pattern_manager_bottom_screen(1, 1);
		if(pattern_select_screen == 1 && pattern_select_page[1] != old_pattern_select_page) {
			map_printf(subBGMapText, 30, 0, "%.2d", pattern_select_page[1]+1);
			redraw_pattern_manager_top_screen(0, 1);
		}

		if(keys_repeat & KEY_UP) {
			pattern_select_y = (pattern_select_y-1)&3;
			if(pattern_select_y == 3)
				pattern_select_screen ^= 1;
		}
		if(keys_repeat & KEY_DOWN) {
			pattern_select_y = (pattern_select_y+1)&3;
			if(pattern_select_y == 0)
				pattern_select_screen ^= 1;
		}
		if(keys_down & KEY_B) {
			pattern_select_screen ^= 1;
		}

		if(keys_down & KEY_A) { // Move
			if(pattern_swapping) {
				erase_pattern_swap_cursor();

				struct acww_pattern *pattern = get_pattern_for_slot(pattern_select_screen, pattern_select_x, pattern_select_y);
				struct acww_pattern temp = *pattern;
				if(pattern != pattern_swap_pointer) {
					*pattern = *pattern_swap_pointer;
					*pattern_swap_pointer = temp;
					pattern_swapping = false;

					// Load new patterns
					if(pattern_select_screen == 0 || pattern_swap_screen == 0)
						redraw_pattern_manager_bottom_screen(0, 1);
					if(pattern_select_screen == 1 || pattern_swap_screen == 1) {
						redraw_pattern_manager_top_screen(0, 1);
						edited_extra_patterns = true;
					}

					// Swap patterns on the map
					if(pattern_swap_screen == 0 && pattern_select_screen == 0 && pattern_select_page[0] == 0 && pattern_swap_page == 0) {
						u16 swap_item_a = 0x00a7 + pattern_select_x + pattern_select_y*8;
						u16 swap_item_b = 0x00a7 + pattern_swap_x   + pattern_swap_y*8;

						for(int i=TOWN_ITEM_GRID_START; i<=TOWN_ITEM_GRID_END; i+=2) {
							u16 item = get_savefile_u16(i);
							if(item == swap_item_a)
								set_savefile_u16(i, swap_item_b);
							else if(item == swap_item_b)
								set_savefile_u16(i, swap_item_a);							
						}
					}
				}
			} else {
				pattern_swap_x = pattern_select_x;
				pattern_swap_y = pattern_select_y;
				pattern_swap_screen  = pattern_select_screen;
				pattern_swap_page    = pattern_select_page[pattern_select_screen];
				pattern_swap_pointer = get_pattern_for_slot(pattern_swap_screen,   pattern_swap_x, pattern_swap_y);
				pattern_swapping = true;
			}
			first_draw = true; // Redraw the text and cursors
		}
		if(keys_down & KEY_Y) { // Unused

		}
		if(keys_down & KEY_X) {
			if(pattern_swapping) {
				erase_pattern_swap_cursor();
				pattern_swapping = false;
			}
			bgHide(pattern_select_screen ? subBG256 : mainBG256);
			u16 *screen = pattern_select_screen ? subBGMapText : mainBGMapText;
			int edit_type = choose_from_list_on_screen(screen, "Edit pattern", edit_pattern_options, 9, pattern_edit_option);
			struct acww_pattern *pattern = get_pattern_for_slot(pattern_select_screen, pattern_select_x, pattern_select_y);

			if(edit_type >= 0) {
				pattern_edit_option = edit_type;
				switch(edit_type) {
					case 0: // Edit
						choose_from_list_on_screen(screen, "Not implemented yet!", ok_options, 1, 0);
						break;
					case 1: // Load from file
					{
						char *original_filename = strdup(filename);

						if(choose_file_on_screen(screen, PATTERN_FILES) == 1) {
							FILE *file = fopen(filename, "rb");
							if(file == NULL) {
								choose_from_list_on_screen(screen, "Can't open pattern", ok_options, 1, 0);
							} else {
								size_t r = fread(pattern, 1, sizeof(struct acww_pattern), file);
								fclose(file);
								if(r != sizeof(struct acww_pattern)) {
									choose_from_list_on_screen(screen, "Not a valid pattern?", ok_options, 1, 0);
								}
							}
							if(pattern_select_screen)
								edited_extra_patterns = true;
						}

						// Clean up
						strcpy(filename, original_filename);
						free(original_filename);
						break;
					}
					case 2: // Save to file
					{
						bool has_pattern_folder = has_acww_folder && makeFolder("/data/acww/patterns") >= 0;
						const char *pattern_folder_prefix = "";
						if(has_pattern_folder) {
							pattern_folder_prefix = "/data/acww/patterns/";
						}

						char name_buffer[20];
						char author_buffer[20];
						char town_buffer[20];
						acstrDecode(name_buffer,   pattern->pattern_name,     16);
						acstrDecode(town_buffer,   pattern->author_town.name, 8);
						acstrDecode(author_buffer, pattern->author.name,      8);
						sprintf(full_file_path, "%s%s by %s in %s.acww", pattern_folder_prefix, name_buffer, author_buffer, town_buffer);

						int is_ok = 1;
						FILE *f = fopen(full_file_path, "rb");
						if(f) {
							is_ok = confirm_choice_on_screen(screen, "OK to overwrite?");
							fclose(f);
						}

						if(is_ok == 1) {
							f = fopen(full_file_path, "wb");
							if(f) {
								if(fwrite(pattern, 1, sizeof(struct acww_pattern), f) != sizeof(struct acww_pattern))
									choose_from_list_on_screen(screen, "Couldn't save pattern file", ok_options, 1, 0);
								else
									choose_from_list_on_screen(screen, "Saved pattern to file", ok_options, 1, 0);
								fclose(f);
							} else {
								choose_from_list_on_screen(screen, "Couldn't create pattern file", ok_options, 1, 0);
							}
						}
						break;
					}
					case 3: // Copy pattern
						have_copied_pattern = true;
						copied_pattern = *pattern;
						break;
					case 4: // Paste pattern
						if(have_copied_pattern) {
							*pattern = copied_pattern;
						}
						if(pattern_select_screen)
							edited_extra_patterns = true;
						break;
					case 5: // Copy creator info
						copied_author      = pattern->author;
						copied_author_town = pattern->author_town;
						have_copied_name = true;
						break;
					case 6: // Paste creator info
						if(have_copied_name) {
							pattern->author      = copied_author;
							pattern->author_town = copied_author_town;
						}
						if(pattern_select_screen)
							edited_extra_patterns = true;
						break;
					case 7: // Delete
					{
						char delete_buffer[64];
						acstrDecode(text_conversion_buffer, pattern->pattern_name, 16);
						sprintf(delete_buffer, "Really delete %s?", text_conversion_buffer);
						if(confirm_choice_on_screen(screen, delete_buffer) == 1) {
							memset(pattern, 0, sizeof(struct acww_pattern));
						}
						if(pattern_select_screen)
							edited_extra_patterns = true;
						break;
					}
					case 8: // View pattern info
						sprintf(title_buffer, "Mystery value: %x", pattern->unknown2 & 15);
						choose_from_list_on_screen(screen, title_buffer, ok_options, 1, 0);
						sprintf(title_buffer, "Author: %.4x; Town: %.4x", pattern->author.id, pattern->author_town.id);
						choose_from_list_on_screen(screen, title_buffer, ok_options, 1, 0);
						break;
				}
			}
			bgShow(pattern_select_screen ? subBG256 : mainBG256);
			if(pattern_select_screen == 1)
				redraw_pattern_manager_top_screen(1, 1);
			else
				redraw_pattern_manager_bottom_screen(1, 1);
			first_draw = true;
		}

		if(first_draw || pattern_select_screen != old_pattern_select_screen || pattern_select_x != old_pattern_select_x || pattern_select_y != old_pattern_select_y) {
			// Display pattern name, author name, author town name
			map_rectfill(subBGMapText, 1, 21, 30, 2, ' ');
			struct acww_pattern *pattern = get_pattern_for_slot(pattern_select_screen, pattern_select_x, pattern_select_y);

			acstrDecode(text_conversion_buffer, pattern->pattern_name, 16);
			map_print(subBGMapText, 1, 21, text_conversion_buffer);

			acstrDecode(text_conversion_buffer, pattern->author.name, 8);
			map_print(subBGMapText, 1, 22, text_conversion_buffer);

			acstrDecode(text_conversion_buffer, pattern->author_town.name, 8);
			map_print(subBGMapText, 16, 22, text_conversion_buffer);

			// Erase old cursor position
			if(old_pattern_select_screen == 0) {
				map_put(mainBGMapText,      old_pattern_select_x*4+1, 6*old_pattern_select_y+5, ' ');
				map_put(mainBGMapText,      old_pattern_select_x*4+2, 6*old_pattern_select_y+5, ' ');
				map_rectfill(mainBGMapText, old_pattern_select_x*4,   6*old_pattern_select_y+1, 4, 4, ' ');
			} else {
				map_put(subBGMapText,       old_pattern_select_x*4+1, 5*old_pattern_select_y+5, old_pattern_select_y == 3 ? TILE_BORDER_HORIZ : ' ');
				map_put(subBGMapText,       old_pattern_select_x*4+2, 5*old_pattern_select_y+5, old_pattern_select_y == 3 ? TILE_BORDER_HORIZ : ' ');
				map_rectfill(subBGMapText,  old_pattern_select_x*4,   5*old_pattern_select_y+1, 4, 4, ' ');
			}

			// Swap cursor
			if(pattern_swapping) {
				if(pattern_swap_page == pattern_select_page[pattern_swap_screen]) {
					// Draw the swap cursor
					if(pattern_swap_screen == 0) {
						map_put(mainBGMapText,               pattern_swap_x*4+1, 6*pattern_swap_y+5, TILE_VERTICAL_PICKER_L_INACTIVE);
						map_put(mainBGMapText,               pattern_swap_x*4+2, 6*pattern_swap_y+5, TILE_VERTICAL_PICKER_R_INACTIVE);
					} else {
						map_put(subBGMapText,                pattern_swap_x*4+1, 5*pattern_swap_y+5, pattern_swap_y == 3 ? TILE_VERTICAL_PICKER_L_BAR_INACTIVE : TILE_VERTICAL_PICKER_L_INACTIVE);
						map_put(subBGMapText,                pattern_swap_x*4+2, 5*pattern_swap_y+5, pattern_swap_y == 3 ? TILE_VERTICAL_PICKER_R_BAR_INACTIVE : TILE_VERTICAL_PICKER_R_INACTIVE);
					}
				} else {
					// If it's the wrong page, erase the swap cursor just in case you have just moved onto a different page
					erase_pattern_swap_cursor();
				}
			}

			// Draw new cursor position
			if(pattern_select_screen == 0) {
				map_put(mainBGMapText,               pattern_select_x*4+1, 6*pattern_select_y+5, TILE_VERTICAL_PICKER_L);
				map_put(mainBGMapText,               pattern_select_x*4+2, 6*pattern_select_y+5, TILE_VERTICAL_PICKER_R);
				draw_selection_box_at(mainBGMapText, pattern_select_x*4,   6*pattern_select_y+1);
			} else {
				map_put(subBGMapText,                pattern_select_x*4+1, 5*pattern_select_y+5, pattern_select_y == 3 ? TILE_VERTICAL_PICKER_L_BAR : TILE_VERTICAL_PICKER_L);
				map_put(subBGMapText,                pattern_select_x*4+2, 5*pattern_select_y+5, pattern_select_y == 3 ? TILE_VERTICAL_PICKER_R_BAR : TILE_VERTICAL_PICKER_R);
				draw_selection_box_at(subBGMapText,  pattern_select_x*4,   5*pattern_select_y+1);
			}

			first_draw = false;
		}

		// Exit
		if(keys_down & KEY_START)
			break;
	}

	clear_screen_256(mainBGMap256);
	clear_screen_256(subBGMap256);
	chdir(original_directory);
	free(original_directory);
}
