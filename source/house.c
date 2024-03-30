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

// ------------------------------------------------------------------------------------------------
// Strings
const char *room_names[] = {"Main", "North", "East", "West", "Upstairs"};

// ------------------------------------------------------------------------------------------------
// Variables

// ------------------------------------------------------------------------------------------------
// Functions

void draw_house_grid_on_map(u16 *map, int map_x, int map_y, int offset, int size_guide) {
	map_rectfill(map, map_x, map_y, 16, 16, ' ');

	switch(size_guide) {
		case 0:
			map_put(map,      map_x+4,  map_y+7,           TILE_BORDER_GRAY_CORNER);
			map_put(map,      map_x+4,  map_y+7+6+1,       TILE_BORDER_GRAY_CORNER | TILE_FLIP_V);
			map_put(map,      map_x+11, map_y+7,           TILE_BORDER_GRAY_CORNER | TILE_FLIP_H);
			map_put(map,      map_x+11, map_y+7+6+1,       TILE_BORDER_GRAY_CORNER | TILE_FLIP_V | TILE_FLIP_H);
			map_rectfill(map, map_x+4,  map_y+8,     1, 6, TILE_BORDER_GRAY_VERT);
			map_rectfill(map, map_x+11, map_y+8,     1, 6, TILE_BORDER_GRAY_VERT | TILE_FLIP_H);
			map_rectfill(map, map_x+5,  map_y+7,     6, 1, TILE_BORDER_GRAY_HORIZ);
			map_rectfill(map, map_x+5,  map_y+7+6+1, 6, 1, TILE_BORDER_GRAY_HORIZ | TILE_FLIP_V);
			break;
		case 1:
			map_put(map,      map_x+3,  map_y+5,           TILE_BORDER_GRAY_CORNER);
			map_put(map,      map_x+3,  map_y+5+8+1,       TILE_BORDER_GRAY_CORNER | TILE_FLIP_V);
			map_put(map,      map_x+12, map_y+5,           TILE_BORDER_GRAY_CORNER | TILE_FLIP_H);
			map_put(map,      map_x+12, map_y+5+8+1,       TILE_BORDER_GRAY_CORNER | TILE_FLIP_V | TILE_FLIP_H);
			map_rectfill(map, map_x+3,  map_y+6,     1, 8, TILE_BORDER_GRAY_VERT);
			map_rectfill(map, map_x+12, map_y+6,     1, 8, TILE_BORDER_GRAY_VERT | TILE_FLIP_H);
			map_rectfill(map, map_x+4,  map_y+5,     8, 1, TILE_BORDER_GRAY_HORIZ);
			map_rectfill(map, map_x+4,  map_y+5+8+1, 8, 1, TILE_BORDER_GRAY_HORIZ | TILE_FLIP_V);
			break;
	}

	for(int y=0; y<16; y++) {
		for(int x=0; x<16; x++) {
			u16 item = get_savefile_u16(offset + x*2 + y*16*2);
			int icon = icon_for_item(item);
			if(icon == ' ')
				continue;
			map_put(map, x+map_x, y+map_y, icon);
		}
	}
}

void menu_house() {
	bool redraw_top = true;
	bool redraw_bottom = true;
	int edit_x = 8;
	int edit_y = 8;
	int edit_screen = 0;
	int edit_page = 0;

	bool edit_swapping = false;
	int edit_swap_x = 0, edit_swap_y = 0, edit_swap_screen = 0, edit_swap_page = 0;

	// ----------------------------------------------------
	// UI loop
	while(1) {
		if(redraw_top) {
			clear_screen(subBGMapText);

			// Room box
			map_box(subBGMapText,   7,  4, 18, 18);

			// Instructions
			map_print(subBGMapText,  2, 1, "\xe0:Move \xe1:Jump \xe2:Edit \xe3:Spin");
			map_print(subBGMapText,  2, 2, "Start:Finish  \xe4\xe5:Switch room");
			map_print(subBGMapText,  8, 4+18, "^Top layer");

			redraw_top = false;
		}
		if(redraw_bottom) {
			clear_screen(mainBGMapText);

			// Room box
			map_box(mainBGMapText,  7,  1, 18, 18);

			// Item name and room name
			map_box(mainBGMapText,  0,  20, 32, 4);

			map_print(mainBGMapText, 8, 1+18, "^Bottom layer");
			redraw_bottom = false;
		}

		wait_vblank_and_animate();
		scanKeys();

		uint32_t keys_down = keysDown();
		uint32_t keys_repeat = 	keysDownRepeat();

		// Movement
		if(keys_repeat & KEY_LEFT)
			edit_x = (edit_x - 1) & 15;
		if(keys_repeat & KEY_RIGHT)
			edit_x = (edit_x + 1) & 15;
		if(keys_repeat & KEY_UP)
			edit_y = (edit_y - 1) & 15;
		if(keys_repeat & KEY_DOWN)
			edit_y = (edit_y + 1) & 15;
		if(keys_down & KEY_B)
			edit_screen ^= 1;
		if(keys_down & KEY_L) {
			edit_page = edit_page-1;
			if(edit_page < 0)
				edit_page = 4;
		}
		if(keys_down & KEY_R) {
			edit_page = edit_page+1;
			if(edit_page > 4)
				edit_page = 0;
		}

		int item_offset = YOUR_HOUSE_START + edit_x*2 + edit_y*16*2 + edit_screen*16*16*2 + edit_page*0x450;
		u16 item = get_savefile_u16(item_offset);
		if(keys_down & KEY_A) { // Swap
			if(edit_swapping) {				
				int item_offset_2 = YOUR_HOUSE_START + edit_swap_x*2 + edit_swap_y*16*2 + edit_swap_screen*16*16*2 + edit_swap_page*0x450;
				u16 item2 = get_savefile_u16(item_offset_2);
				set_savefile_u16(item_offset, item2);
				set_savefile_u16(item_offset_2, item);
				edit_swapping = false;
			} else {
				edit_swap_screen = edit_screen;
				edit_swap_x = edit_x;
				edit_swap_y = edit_y;
				edit_swap_page = edit_page;
				edit_swapping = true;
			}
		}
		if(keys_down & KEY_X) { // Edit
			// Hide sprites
			oamSetHidden(&oamSub, 0, true);
			oamSetHidden(&oamSub, 1, true);
			oamSetHidden(&oamMain, 0, true);
			oamSetHidden(&oamMain, 1, true);

			edit_swapping = false;
			int new_item = edit_item_menu(edit_screen ? subBGMapText : mainBGMapText, item, 0);
			if(new_item != item) {
				set_savefile_u16(item_offset, new_item);
			}

			if(edit_screen)
				redraw_top = true;
			else
				redraw_bottom = true;
		}
		if(keys_down & KEY_Y) { // Rotate
			if(item >= 0x3000 && item <= 0x4fff) {
				set_savefile_u16(item_offset, (item & 0xfffc) + ((item+1)&3));
			}
		}

		// Display the cursor, using sprites
		if(edit_screen) {
			oamSet(&oamSub,
				0,      //OAM index
				edit_x*8+8*8, edit_y*8+5*8, // Position
				0,      // Priority
				0,      // Palette
				SpriteSize_8x8, SpriteColorFormat_16Color, SPRITE_SUB_TILE_POINTER(0),
				-1, false, false, false, false, false // No rotation, don't double, don't hide, no vflip, no hflip, no mosaic
				);
			oamSetHidden(&oamMain, 0, true);
		} else {
			oamSet(&oamMain,
				0,      //OAM index
				edit_x*8+8*8, edit_y*8+2*8, // Position
				0,      // Priority
				0,      // Palette
				SpriteSize_8x8, SpriteColorFormat_16Color, SPRITE_MAIN_TILE_POINTER(0),
				-1, false, false, false, false, false // No rotation, don't double, don't hide, no vflip, no hflip, no mosaic
				);
			oamSetHidden(&oamSub, 0, true);
		}

		// Secondary cursor for swaps
		if(edit_swapping && edit_swap_page == edit_page) {
			if(edit_swap_screen) {
				oamSet(&oamSub,
					1,      //OAM index
					edit_swap_x*8+8*8, edit_swap_y*8+5*8, // Position
					0,      // Priority
					0,      // Palette
					SpriteSize_8x8, SpriteColorFormat_16Color, SPRITE_SUB_TILE_POINTER(1),
					-1, false, false, false, false, false // No rotation, don't double, don't hide, no vflip, no hflip, no mosaic
					);
				oamSetHidden(&oamMain, 1, true);
			} else {
				oamSet(&oamMain,
					1,      //OAM index
					edit_swap_x*8+8*8, edit_swap_y*8+2*8, // Position
					0,      // Priority
					0,      // Palette
					SpriteSize_8x8, SpriteColorFormat_16Color, SPRITE_MAIN_TILE_POINTER(1),
					-1, false, false, false, false, false // No rotation, don't double, don't hide, no vflip, no hflip, no mosaic
					);
				oamSetHidden(&oamSub, 1, true);
			}
		} else {
			oamSetHidden(&oamMain, 1, true);
			oamSetHidden(&oamSub,  1, true);
		}

		// Update both maps (probably don't need to do this every frame? But it doesn't hurt anything)
		draw_house_grid_on_map(mainBGMapText, 8, 2, YOUR_HOUSE_START + 16*16*2*0 + edit_page*0x450, edit_page==0);
		draw_house_grid_on_map(subBGMapText,  8, 5, YOUR_HOUSE_START + 16*16*2*1 + edit_page*0x450, edit_page==0);

		// Display the item and room names
		map_rectfill(mainBGMapText, 1, 21, 30, 2, ' ');
		map_printf(mainBGMapText, 1, 21, "Room: %s", room_names[edit_page]);
		if(item != EMPTY_ITEM) {
			map_put(mainBGMapText,   1, 22, icon_for_item(item));
			map_print(mainBGMapText, 2, 22, name_for_item(item));

			if(item >= 0x3000 && item <= 0x4fff) { // Show the direction
				map_put(mainBGMapText,   30, 22, 0xFC + (item & 3));
			}
		}

		// Exit
		if(keys_down & KEY_START)
			break;
	}
}

