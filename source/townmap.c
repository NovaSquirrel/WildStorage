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
*/
#include <nds.h>
#include "wild.h"

#include <acre_terrain.h>

#define FIRST_PATTERN_ITEM 0x00a7
#define LAST_PATTERN_ITEM 0x00c6

// ------------------------------------------------------------------------------------------------
// Strings

// ------------------------------------------------------------------------------------------------
// Variables
extern const char acre_maps[];
extern int pattern_select_page[2];
int townmap_pattern_mode = 0;
int town_map_edit_x = 32;
int town_map_edit_y = 32;

// ------------------------------------------------------------------------------------------------
// Functions

struct acww_pattern *get_pattern_for_slot(int screen, int slot_x, int slot_y);
void copy_pattern_to_vram(struct acww_pattern *pattern, u16 *vram);
void copy_pattern_to_vram_8x8(struct acww_pattern *pattern, u16 *vram);
void pattern_tiles_at(u16 *map, int base_x, int base_y, int initial);

int townmap_base_terrain(int x, int y) {
	if(x < 0 || x > 63 || y < 0 || y > 63)
		return -1;
	x += 16;
	y += 16;
	u8 acre_type = savefile[0x0C330 + y/16*6 + x/16]; // 6x6 town layout, including the surrounding cliffs
	return acre_maps[acre_type*256 + (y&15)*16 + (x&15)];
}

int offset_for_item_on_map(int x, int y) {
	return TOWN_ITEM_GRID_START + (y/16*512*4) + (x/16*512) + ((y&15)*32) + ((x&15)*2);
}

bool get_hole_on_town_map(int x, int y) {
	return savefile[HOLE_GRID_START + (((x/8)&1) + (y&15)*2) + (x/16*32) + (y/16*128)] & (1 << (x&7)) ? true : false;
}

void set_hole_on_town_map(int x, int y, bool hole) {
	if(hole)
		savefile[HOLE_GRID_START + (((x/8)&1) + (y&15)*2) + (x/16*32) + (y/16*128)] |= 1 << (x&7);
	else
		savefile[HOLE_GRID_START + (((x/8)&1) + (y&15)*2) + (x/16*32) + (y/16*128)] &= ~(1 << (x&7));
}

void townmap_draw_terrain_32(int x, int y) {
	int terrain = townmap_base_terrain(x, y);
	int item = (x >= 0 && x <= 63 && y >= 0 && y <= 63) ? get_savefile_u16(offset_for_item_on_map(x, y)) : EMPTY_ITEM;
	int icon = (x >= 0 && x <= 63 && y >= 0 && y <= 63) ? icon_for_item(item) : ICON_EMPTY;
	x *= 4;
	y *= 4;
	int map_x = x & 31;
	int map_y = (y & 31) + (x&32);

	if(item >= FIRST_PATTERN_ITEM && item <= LAST_PATTERN_ITEM) {
		// If it's a pattern, just draw a pattern instead of trying to show acre terrain
		for(int ty=0; ty<4; ty++) {
			for(int tx=0; tx<4; tx++) {
				mainBGMap256[map_x+tx + (map_y+ty)*32] = (item - FIRST_PATTERN_ITEM) * 16 + tx*4 + ty + 64;
				mainBGMapText[map_x+tx + (map_y+ty)*32] = ' ';
			}
		}
	} else {
		// Otherwise draw terrain and an optional item above it
		for(int ty=0; ty<4; ty++) {
			for(int tx=0; tx<4; tx++) {
				int tile = (terrain+1)*4;
				if(tx != 0 && ty == 0)
					tile += 1;
				else if(tx == 0 && ty != 0)
					tile += 2;
				else if(tx != 0 && ty != 0)
					tile += 3;
				mainBGMap256[map_x+tx + (map_y+ty)*32] = tile;
				mainBGMapText[map_x+tx + (map_y+ty)*32] = ' ';
			}
		}
		if(icon != ICON_EMPTY) {
			// Translate to the weird ordering you get by making the 16x16 tileset make sense visually
			int tile = (icon - ICON_X);
			tile = 26*16 + ((tile&7)*2) + (tile/8*32);
			if(icon == ICON_PICTURE)
				tile = 0 + 34*16;
			else if(icon == ICON_MAIL)
				tile = 2 + 34*16;
			else if(icon == ICON_SPACE_USED)
				tile = 6 + 34*16;
			mainBGMapText[map_x+1 + (map_y+1)*32] = tile+0;
			mainBGMapText[map_x+2 + (map_y+1)*32] = tile+1;
			mainBGMapText[map_x+1 + (map_y+2)*32] = tile+16;
			mainBGMapText[map_x+2 + (map_y+2)*32] = tile+17;
		}
	}
}

void townmap_draw_terrain_8(int x, int y) {
	int terrain = (townmap_base_terrain(x, y)+1)*4+3;
	int map_x = x & 31;
	int map_y = (y & 31) + (x&32);
	mainBGMap256[map_x + (map_y)*32] = terrain;
	if(x >= 0 && x <= 63 && y >= 0 && y <= 63) {
		int item = get_savefile_u16(offset_for_item_on_map(x, y));
		if(item >= FIRST_PATTERN_ITEM && item <= LAST_PATTERN_ITEM) {
			mainBGMap256[map_x + (map_y)*32] = 0x20 + (item - FIRST_PATTERN_ITEM);
			mainBGMapText[map_x + (map_y)*32] = ' ';
		} else 
			mainBGMapText[map_x + (map_y)*32] = icon_for_item(item);
	} else
		mainBGMapText[map_x + (map_y)*32] = ' ';
}

void townmap_draw_column_32(int base_x, int base_y) {
	for(int offset_y=-1; offset_y<=7; offset_y++) {
		townmap_draw_terrain_32(base_x, base_y + offset_y);
	}
}

void townmap_draw_row_32(int base_x, int base_y) {
	for(int offset_x=-1; offset_x<=9; offset_x++) {
		townmap_draw_terrain_32(base_x + offset_x, base_y);
	}
}

void townmap_draw_column_8(int base_x, int base_y) {
	for(int offset_y=-1; offset_y<=25; offset_y++) {
		townmap_draw_terrain_8(base_x, base_y + offset_y);
	}
}

void townmap_draw_row_8(int base_x, int base_y) {
	for(int offset_x=-1; offset_x<=33; offset_x++) {
		townmap_draw_terrain_8(base_x + offset_x, base_y);
	}
}

void menu_townmap() {
	clear_screen(subBGMapText);
	clear_screen_256(subBGMap256);
	clear_screen(mainBGMapText);
	clear_screen_256(mainBGMap256);

	u16 *bottomChr = bgGetGfxPtr(mainBG256);
	u16 *topChr    = bgGetGfxPtr(subBG256);
	dmaCopy(acre_terrainTiles, bottomChr, sizeof(acre_terrainTiles));

	// Copy all player patterns to VRAM
	pattern_select_page[0] = 0; // Set get_pattern_for_slot() to get player patterns
	for(int player=0; player<4; player++) {
		for(int pattern=0; pattern<8; pattern++) {
			int vram_index = player*8+pattern+4;
			struct acww_pattern *pattern_pointer = get_pattern_for_slot(0, pattern, player);
			copy_pattern_to_vram(pattern_pointer, bottomChr+512*vram_index);
			copy_pattern_to_vram(pattern_pointer, topChr+512*vram_index);
			copy_pattern_to_vram_8x8(pattern_pointer, bottomChr+(player*8+pattern+32)*32);
			pattern_tiles_at(subBGMap256, 0+pattern*4, 2+player*4, vram_index*16);
		}
	}
	/*  Tile numbers for the 256 layers are then as follows:
		Tiles 000-01F: Terrain
		Tiles 020-03F: 8x8 versions of player patterns
		Tiles 040-240: Player patterns
	*/

	map_box(subBGMapText,  0,  20, 32, 4); // Box that will hold item info

	// Variables
	int edit_x = town_map_edit_x;
	int edit_y = town_map_edit_y;
	int pattern_x = 0;
	int pattern_y = 0;
	bool edit_swapping = false;
	int edit_swap_x = 0, edit_swap_y = 0;
	bool redraw_map = true;
	bool top_screen = false;
	bool draw_unset_mode = false;
	bool wait_for_release = true;

	// UI loop
	while(1) {
		swiWaitForVBlank();
		oamUpdate(&oamMain);
		oamUpdate(&oamSub);
		if(townmap_pattern_mode) {
			int topleft_x = edit_x - 4;
			int topleft_y = edit_y - 3;
			bgSetScroll(mainBG256, topleft_x*32+16, topleft_y*32+16);
			bgSetScroll(mainBGText, topleft_x*32+16, topleft_y*32+16);
			if(redraw_map) {
				for(int i=topleft_x; i<=topleft_x+8; i++)
					townmap_draw_column_32(i, topleft_y);
			} else {
				// Update all surrounding edges/corners instead of the entire screen
				townmap_draw_column_32(topleft_x-1, topleft_y);
				townmap_draw_column_32(topleft_x+8, topleft_y);
				townmap_draw_row_32(topleft_x, topleft_y-1);
				townmap_draw_row_32(topleft_x, topleft_y+6);
			}
		} else {
			int topleft_x = edit_x - 16;
			int topleft_y = edit_y - 12;
			bgSetScroll(mainBG256, topleft_x*8+4, topleft_y*8+4);
			bgSetScroll(mainBGText, topleft_x*8+4, topleft_y*8+4);
			if(redraw_map) {
				for(int i=topleft_x; i<=topleft_x+32; i++)
					townmap_draw_column_8(i, topleft_y);
			} else {
				// Update all surrounding edges/corners instead of the entire screen
				townmap_draw_column_8(topleft_x-1, topleft_y);
				townmap_draw_column_8(topleft_x+32, topleft_y);
				townmap_draw_row_8(topleft_x, topleft_y-1);
				townmap_draw_row_8(topleft_x, topleft_y+24);
			}
		}
		redraw_map = false;

		just_animate_scrolling_bg();
		bgUpdate();
		scanKeys();

		uint32_t keys_down = keysDown();
		uint32_t keys_repeat = keysDownRepeat();
		uint32_t keys_held = keysHeld();
		if(keys_held == 0)
			wait_for_release = false;

		// Handle moving around and swapping between item mode and pattern mode
		if(top_screen) {
			if(keys_repeat & KEY_LEFT)
				pattern_x = (pattern_x - 1) & 7;
			if(keys_repeat & KEY_RIGHT)
				pattern_x = (pattern_x + 1) & 7;
			if(keys_repeat & KEY_UP)
				pattern_y = (pattern_y - 1) & 3;
			if(keys_repeat & KEY_DOWN)
				pattern_y = (pattern_y + 1) & 3;
		} else {
			if(keys_repeat & KEY_LEFT && edit_x > 0)
				edit_x = edit_x - 1;
			if(keys_repeat & KEY_RIGHT && edit_x < 63)
				edit_x = edit_x + 1;
			if(keys_repeat & KEY_UP && edit_y > 0)
				edit_y = edit_y - 1;
			if(keys_repeat & KEY_DOWN && edit_y < 63)
				edit_y = edit_y + 1;
		}
		if(keys_down & KEY_Y) { // Zoom
			townmap_pattern_mode ^= 1;
			top_screen = false;
			redraw_map = true;
			edit_swapping = false;
		}

		int item_offset = offset_for_item_on_map(edit_x, edit_y);
		u16 item = get_savefile_u16(item_offset);

		// Display cursor sprites and handle key presses for actions
		oamSetHidden(&oamMain, 1, true); // Default to having the swap cursor hidden
		if(townmap_pattern_mode) {
			int topleft_x = edit_x - 4;
			int topleft_y = edit_y - 3;
			oamSet(&oamMain,
				0,      //OAM index
				edit_x*32-topleft_x*32-16, edit_y*32-topleft_y*32-16,
				0,      // Priority
				0,      // Palette
				SpriteSize_32x32, SpriteColorFormat_16Color, top_screen ? SPRITE_MAIN_TILE_POINTER(0x30) : SPRITE_MAIN_TILE_POINTER(0x20),
				-1, false, false, false, false, false
				);
			u16 draw_with = FIRST_PATTERN_ITEM + pattern_x + pattern_y * 8;
			if(keys_down & KEY_A && top_screen) {
				top_screen = false;
				wait_for_release = true;
			} else {
				if(keys_down & KEY_A) {
					draw_unset_mode = item == draw_with;
				}
				if(keys_held & KEY_A && !wait_for_release) { // Draw
					u16 draw_with = FIRST_PATTERN_ITEM + pattern_x + pattern_y * 8;
					if(draw_unset_mode) {
						if(item >= FIRST_PATTERN_ITEM && item <= LAST_PATTERN_ITEM) {
							set_savefile_u16(item_offset, EMPTY_ITEM);
						}
					} else {
						if(item == EMPTY_ITEM || (item >= FIRST_PATTERN_ITEM && item <= LAST_PATTERN_ITEM)) {
							set_savefile_u16(item_offset, draw_with);
						}
					}
					if(get_savefile_u16(item_offset) != item)
						redraw_map = true;
				}
			}
			if(keys_held & KEY_B && !wait_for_release) { // Get
				if(item >= FIRST_PATTERN_ITEM && item <= LAST_PATTERN_ITEM) {
					pattern_x = (item - FIRST_PATTERN_ITEM) & 7;
					pattern_y = (item - FIRST_PATTERN_ITEM) / 8;
				}
			}
			if(keys_down & KEY_X) { // Pick from palettes
				top_screen = !top_screen;
			}
		} else { // Item mode
			int topleft_x = edit_x - 16;
			int topleft_y = edit_y - 12;
			oamSet(&oamMain,
				0,      //OAM index
				edit_x*8-topleft_x*8-4, edit_y*8-topleft_y*8-4,
				0,      // Priority
				0,      // Palette
				SpriteSize_8x8, SpriteColorFormat_16Color, top_screen ? SPRITE_MAIN_TILE_POINTER(0x51) : SPRITE_MAIN_TILE_POINTER(0x50),
				-1, false, false, false, false, false
				);
			if(keys_down & KEY_A) { // Move
				if(edit_swapping) {				
					int item_offset_2 = offset_for_item_on_map(edit_swap_x, edit_swap_y);
					u16 item2 = get_savefile_u16(item_offset_2);
					set_savefile_u16(item_offset, item2);
					set_savefile_u16(item_offset_2, item);

					bool hole1 = get_hole_on_town_map(edit_x, edit_y);
					bool hole2 = get_hole_on_town_map(edit_swap_x, edit_swap_y);
					set_hole_on_town_map(edit_x, edit_y, hole2);
					set_hole_on_town_map(edit_swap_x, edit_swap_y, hole1);

					edit_swapping = false;
					redraw_map = true;
				} else {
					edit_swap_x = edit_x;
					edit_swap_y = edit_y;
					edit_swapping = true;
				}
			}
			if(edit_swapping) {
				int draw_x = edit_swap_x*8-topleft_x*8-4;
				int draw_y = edit_swap_y*8-topleft_y*8-4;
				if(draw_x >= 0 && draw_x < SCREEN_WIDTH && draw_y >= 0 && draw_y < SCREEN_HEIGHT)
					oamSet(&oamMain,
						1,      //OAM index
						draw_x, draw_y,
						0,      // Priority
						0,      // Palette
						SpriteSize_8x8, SpriteColorFormat_16Color, SPRITE_MAIN_TILE_POINTER(0x51),
						-1, false, false, false, false, false
						);
			}
			if(keys_down & KEY_B) { // Bury
				set_hole_on_town_map(edit_x, edit_y, !get_hole_on_town_map(edit_x, edit_y));
			}
			if(keys_down & KEY_X) { // Edit
				clear_screen(subBGMapText);
				wait_vblank_and_animate();
				bgSetScroll(subBGText, 0, 0);
				bgHide(subBG256);

				// Hide sprites
				oamSetHidden(&oamSub, 0, true);
				oamSetHidden(&oamSub, 1, true);
				oamSetHidden(&oamMain, 0, true);
				oamSetHidden(&oamMain, 1, true);

				edit_swapping = false;
				int new_item = edit_item_menu(subBGMapText, item, 0);
				if(new_item != item) {
					set_savefile_u16(item_offset, new_item);
				}

				bgShow(subBG256);
				redraw_map = true;
				clear_screen(subBGMapText);
				map_box(subBGMapText,  0,  20, 32, 4); // Box that will hold item info
			}
		}
		// Pattern selector
		oamSet(&oamSub,
			0,      //OAM index
			pattern_x*32, 16+pattern_y*32,
			0,      // Priority
			0,      // Palette
			SpriteSize_32x32, SpriteColorFormat_16Color, top_screen ? SPRITE_MAIN_TILE_POINTER(0x20) : SPRITE_MAIN_TILE_POINTER(0x30),
			-1, false, false, false, false, false
			);

		map_print(subBGMapText, 1, 0, townmap_pattern_mode ? "\xe0:Draw \xe1:Get \xe2:Select \xe3:Zoom" : "\xe0:Move \xe1:Bury  \xe2:Edit \xe3:Zoom");
		map_printf(subBGMapText, 1,  19, "X,Y: %2d,%2d       Acre: %d,%d", edit_x, edit_y, edit_x/16+1, edit_y/16+1);

		// Display the item name and icon
		map_rectfill(subBGMapText, 1, 21, 30, 2, ' ');
		if(item != EMPTY_ITEM) {
			map_put(subBGMapText,   1, 22, icon_for_item(item));
			map_print(subBGMapText, 2, 22, name_for_item(item));
		}

		if(get_hole_on_town_map(edit_x, edit_y)) { // Holes
			map_put(subBGMapText,   30, 22, ICON_BURIED);
		}

		// Exit
		if(keys_down & KEY_START)
			break;
	}

	bgSetScroll(mainBG256, 0, 0);
	bgSetScroll(mainBGText, 0, 0);
	clear_screen_256(subBGMap256);
	clear_screen_256(mainBGMap256);

	// Fix neighbor positions if their houses were moved
	int neighbor_x[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
	int neighbor_y[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
	for(int y=0; y<64; y++) {
		for(int x=0; x<64; x++) {
			u16 item = get_savefile_u16(offset_for_item_on_map(x, y));
			if(item >= 0x5001 && item <= 0x5008) { // Neighbor house
				int neighbor = item - 0x5001;
				if(neighbor_x[neighbor] == -1) {
					neighbor_x[neighbor] = x;
					neighbor_y[neighbor] = y;
				}
			}
		}
	}
	for(int neighbor=0; neighbor<8; neighbor++) {
		if(neighbor_x[neighbor] == -1)
			continue;
		int stored_x = savefile[NEIGHBOR_START + 0x700*neighbor + 0x90] - 0x10;
		int stored_y = savefile[NEIGHBOR_START + 0x700*neighbor + 0x91] - 0x10;
		if(stored_x != neighbor_x[neighbor] || stored_y != neighbor_y[neighbor]) {
			popup_noticef("Moving neighbor %d", neighbor+1);
			savefile[NEIGHBOR_START + 0x700*neighbor + 0x90] = neighbor_x[neighbor];
			savefile[NEIGHBOR_START + 0x700*neighbor + 0x91] = neighbor_y[neighbor];
		}
	}
	town_map_edit_x = edit_x;
	town_map_edit_y = edit_y;
}
