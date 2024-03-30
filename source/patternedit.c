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

// ------------------------------------------------------------------------------------------------
// Variables

u8 pattern_edit_data[32][16]; // data[row][column], where each byte has the left pixel in the lower nybble and right pixel in the upper nybble
int pattern_edit_palette = 0;

enum {
	TOOL_1PX,
	TOOL_2PX,
	TOOL_3PX,
	TOOL_LINE,
	TOOL_RECT_LINE,
	TOOL_RECT_FILL,
	TOOL_FLOOD_FILL,
	TOOL_MORE_TOOLS,
	TOOL_UNDO,
	TOOL_REDO,

	TOOL_ROTATE_L,
	TOOL_ROTATE_R,
	TOOL_VFLIP,
	TOOL_HFLIP,
	TOOL_SWAP_COLORS,
	TOOL_UNUSED,
	TOOL_SHIFT,
	TOOL_LESS_TOOLS,
	TOOL_PALETTE_PREV,
	TOOL_PALETTE_NEXT,
};

// ------------------------------------------------------------------------------------------------
// Functions

#define EDIT_PATTERN_CANVAS_X 64
#define EDIT_PATTERN_PREVIEW_X 16
#define EDIT_PATTERN_PREVIEW_Y 6
#define EDIT_PATTERN_COLORS_X 4
#define EDIT_PATTERN_COLORS_Y 46
#define EDIT_TOOLS_X 4
#define EDIT_TOOLS_Y 110

void rectfill_bitmap_16(u16 *bitmap, int base_x, int base_y, int w, int h, u16 value) {
	for(int y=0; y<h; y++) {
		for(int x=0; x<w; x++) {
			bitmap[(base_y+y)*128 + base_x + x] = value;
		}
	}
}

void rect_around(u16 *bitmap, int base_x, int base_y, int w, int h) {
	// Only seems to work properly for coordinates that are a multiple of 4, but that's fine?
	for(int y=-1; y<=h; y++) {
		bitmap[(base_y+y)*128 + base_x - 1] = 12 << 8;
		bitmap[(base_y+y)*128 + base_x + w] = 12;
	}

	for(int x=-1; x<w; x++) {
		bitmap[(base_y-1)*128 + base_x + x] = 12 | (12 << 8);
		bitmap[(base_y+h)*128 + base_x + x] = 12 | (12 << 8);
	}
}

void redraw_edited_pattern() {
	u16 *bitmap = bgGetGfxPtr(mainBG256);

	rect_around(bitmap, EDIT_PATTERN_PREVIEW_X/2, EDIT_PATTERN_PREVIEW_Y, 32/2,   32);
	rect_around(bitmap, EDIT_PATTERN_COLORS_X/2,  EDIT_PATTERN_COLORS_Y,  14*4/2, 14*4);
	rect_around(bitmap, EDIT_TOOLS_X/2,           EDIT_TOOLS_Y,           14*4/2, 10*8);

	// Colors
	for(int i=0; i<16; i++) {
		u8 color = pattern_map_to_shared[pattern_edit_palette*16 + i];
		rectfill_bitmap_16(bitmap, ((i&3)*14+EDIT_PATTERN_COLORS_X)/2, EDIT_PATTERN_COLORS_Y+(i>>2)*14, 14/2, 14, color | (color << 8));
	}

	// Draw pattern
	for(int y=0; y<32; y++) {
		for(int x=0; x<16; x++) {
			u8 byte = pattern_edit_data[y][x];
			u8 left_pixel  = pattern_map_to_shared[pattern_edit_palette*16 + (byte & 15)];
			u8 right_pixel = pattern_map_to_shared[pattern_edit_palette*16 + (byte >> 4)];

			// 1x Preview
			bitmap[y*128 + x + (EDIT_PATTERN_PREVIEW_X/2 + EDIT_PATTERN_PREVIEW_Y*128)] = left_pixel | (right_pixel << 8);

			// 6x Main canvas
			u16 *base = &bitmap[y*128*6 + x*6 + EDIT_PATTERN_CANVAS_X/2];
			for(int fill_y=0; fill_y<6; fill_y++) {
				for(int fill_x=0; fill_x<3; fill_x++) {
					base[fill_y*128+fill_x+0] = left_pixel | (left_pixel<<8);
					base[fill_y*128+fill_x+3] = right_pixel | (right_pixel<<8);
				}
			}	
		}
	}
}

void draw_pattern_editor_tool_page(int page) {
	// Draw tool icons
	for(int y=0; y<10; y++) {
		for(int x=0; x<6; x++) {
			map_put(mainBGMapText, 1+x, 13+y, TILE_BASE_PATTERN_TOOLS+y*16+x+page*6);
		}
	}
}

void put_pattern_pixel(int x, int y, u8 color) {
	x &= 31;
	y &= 31;
	if(x & 1)
		pattern_edit_data[y][x/2] = (pattern_edit_data[y][x/2] & 0x0f) | (color << 4);
	else
		pattern_edit_data[y][x/2] = (pattern_edit_data[y][x/2] & 0xf0) | color;
}

u8 get_pattern_pixel(int x, int y) {
	x &= 31;
	y &= 31;
	if(x & 1)
		return pattern_edit_data[y][x/2] >> 4;
	else
		return pattern_edit_data[y][x/2] & 15;
}

void swap_pattern_pixels(int x, int y, int x2, int y2) {
	u8 a = get_pattern_pixel(x, y);
	u8 b = get_pattern_pixel(x2, y2);
	put_pattern_pixel(x, y, b);
	put_pattern_pixel(x2, y2, a);
}

u8 flood_fill_color;
void pattern_flood_fill(int x, int y, int new_color) {
	put_pattern_pixel(x, y, new_color);
	if(get_pattern_pixel(x-1, y) == flood_fill_color)
		pattern_flood_fill(x-1, y, new_color);
	if(get_pattern_pixel(x+1, y) == flood_fill_color)
		pattern_flood_fill(x+1, y, new_color);
	if(get_pattern_pixel(x, y-1) == flood_fill_color)
		pattern_flood_fill(x, y-1, new_color);
	if(get_pattern_pixel(x, y+1) == flood_fill_color)
		pattern_flood_fill(x, y+1, new_color);
}

void pattern_editor() {
	clear_screen(mainBGMapText);

	videoSetMode(MODE_3_2D);
	mainBGText   = bgInit(0, BgType_Text4bpp, BgSize_T_512x256, 0, 1);
	mainBGBehind = bgInit(2, BgType_Text4bpp, BgSize_T_256x256, 4, 7);
	mainBG256    = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 3, 0);
	bgSetPriority(mainBGText,   0);
	bgSetPriority(mainBG256,    1);
	bgSetPriority(mainBGBehind, 2);
	oamInit(&oamMain, SpriteMapping_1D_32, false);
	dmaFillHalfWords(0, bgGetGfxPtr(mainBG256), 256*256); // Clear whole screen
	bgSetScroll(mainBGText, 0, -6);

	// Initialize the pattern
	for(int i=0; i<32; i++) {
		for(int j=0; j<16; j++) {
			pattern_edit_data[i][j] = 0xFF;
		}
	}

	redraw_edited_pattern();
	draw_pattern_editor_tool_page(0);

	int edit_x = 0;
	int edit_y = 0;
	int edit_state = 0;
	int edit_tool_within_page = 0;
	int edit_tool_page = 0;
	int edit_color = 0;
	bool wait_for_release = false;
	int tool_when_switching_to_tools = 0;

	int other_coord_x = 0, other_coord_y = 0;
	bool have_other_coordinate = false;

	while(1) {
		wait_vblank_and_animate();
		scanKeys();

		uint32_t keys_down = keysDown();
		uint32_t keys_repeat = keysDownRepeat();
		uint32_t keys_held   = keysHeld();
		if(keys_held == 0)
			wait_for_release = false;
		int edit_tool = edit_tool_within_page + edit_tool_page * 10;

		if(keys_repeat & KEY_L)
			edit_color = (edit_color-1) & 15;
		if(keys_repeat & KEY_R)
			edit_color = (edit_color+1) & 15;

		bool pattern_edited = false;
		switch(edit_state) {
			case 0: // Draw
			{
				bool prevent_move = false;
				if(keys_down & KEY_A) {
					switch(edit_tool) {
						case TOOL_SWAP_COLORS:
						{
							u8 swap_color = get_pattern_pixel(edit_x, edit_y);
							for(int y=0; y<32; y++)
								for(int x=0; x<32; x++) {
									u8 test_color = get_pattern_pixel(x, y);
									if(test_color == swap_color)
										put_pattern_pixel(x, y, edit_color);
									else if(test_color == edit_color)
										put_pattern_pixel(x, y, swap_color);
								}
							pattern_edited = true;
							edit_color = swap_color;
							break;
						}
						case TOOL_FLOOD_FILL:
							flood_fill_color = get_pattern_pixel(edit_x, edit_y);
							if(flood_fill_color != edit_color)
								pattern_flood_fill(edit_x, edit_y, edit_color);
							pattern_edited = true;
							break;
						case TOOL_LINE:
						case TOOL_RECT_LINE:
						case TOOL_RECT_FILL:
							if(have_other_coordinate) {
								int x0 = edit_x < other_coord_x ? edit_x : other_coord_x;
								int x1 = edit_x > other_coord_x ? edit_x : other_coord_x;
								int y0 = edit_y < other_coord_y ? edit_y : other_coord_y;
								int y1 = edit_y > other_coord_y ? edit_y : other_coord_y;
								switch(edit_tool) {
									case TOOL_LINE:
									{
										// Brensenham's line algorithm, from Wikipedia										
										int dx = abs(other_coord_x - edit_x);
										int sx = edit_x < other_coord_x ? 1 : -1;
										int dy = -abs(other_coord_y - edit_y);
										int sy = edit_y < other_coord_y ? 1 : -1;
										int error = dx + dy;
										int x = edit_x;
										int y = edit_y;

										while(1) {
											put_pattern_pixel(x, y, edit_color);
											if(x == other_coord_x && y == other_coord_y)
												break;
											int e2 = 2 * error;
											if(e2 >= dy) {
												if(x == other_coord_x)
													break;
												error = error + dy;
												x += sx;
											}
											if(e2 <= dx) {
												if(y == other_coord_y)
													break;
												error = error + dx;
												y += sy;
											}
										}
										break;
									}
									case TOOL_RECT_LINE:
									{
										for(int x=x0; x<=x1; x++) {
											put_pattern_pixel(x, y0, edit_color);
											put_pattern_pixel(x, y1, edit_color);
										}
										for(int y=y0; y<=y1; y++) {
											put_pattern_pixel(x0, y, edit_color);
											put_pattern_pixel(x1, y, edit_color);
										}
										break;
									}
									case TOOL_RECT_FILL:
									{
										for(int x=x0; x<=x1; x++)
											for(int y=y0; y<=y1; y++)
												put_pattern_pixel(x, y, edit_color);
										break;
									}
								}
								pattern_edited = true;
								have_other_coordinate = false;
							} else {
								other_coord_x = edit_x;
								other_coord_y = edit_y;
								have_other_coordinate = true;
							}
							break;
					}
				}
				if((keys_held & KEY_A) && !wait_for_release) {
					switch(edit_tool) {
						case TOOL_1PX:
							put_pattern_pixel(edit_x, edit_y, edit_color);
							pattern_edited = true;
							break;
						case TOOL_2PX:
							put_pattern_pixel(edit_x,   edit_y,   edit_color);
							put_pattern_pixel(edit_x-1, edit_y,   edit_color);
							put_pattern_pixel(edit_x,   edit_y-1, edit_color);
							put_pattern_pixel(edit_x-1, edit_y-1, edit_color);
							pattern_edited = true;
							break;
						case TOOL_3PX:
						{
							for(int y=0; y<3; y++)
								for(int x=0; x<3; x++)
									put_pattern_pixel(edit_x-x, edit_y-y, edit_color);
							pattern_edited = true;
							break;
						}
						case TOOL_SHIFT:
						{
							int offset_x = 0, offset_y = 0;
							if(keys_repeat & KEY_LEFT) {
								offset_x = 1;
							} if(keys_repeat & KEY_RIGHT) {
								offset_x = -1;
							} if(keys_repeat & KEY_UP) {
								offset_y = 1;
							} if(keys_repeat & KEY_DOWN) {
								offset_y = -1;
							}
							if(offset_x || offset_y) {
								u8 temp[32][32];
								for(int y=0; y<32;y++)
									for(int x=0; x<32; x++)
										temp[x][y] = get_pattern_pixel(x + offset_x, y + offset_y);
								for(int y=0; y<32;y++)
									for(int x=0; x<32; x++)
										put_pattern_pixel(x, y, temp[x][y]);
							}

							prevent_move = true;
							pattern_edited = true;
							break;
						}
					}
				}
				if(!prevent_move) {
					if(keys_repeat & KEY_LEFT)
						edit_x = (edit_x - 1) & 31;
					if(keys_repeat & KEY_RIGHT)
						edit_x = (edit_x + 1) & 31;
					if(keys_repeat & KEY_UP)
						edit_y = (edit_y - 1) & 31;
					if(keys_repeat & KEY_DOWN)
						edit_y = (edit_y + 1) & 31;
				}
				if(keys_held & KEY_B)
					edit_color = get_pattern_pixel(edit_x, edit_y);
				if(keys_down & KEY_X)
					edit_state = 1;
				if(keys_down & KEY_Y) {
					edit_state = 2;
					have_other_coordinate = false;
					tool_when_switching_to_tools = edit_tool;
				}
				break;
			}
			case 1: // Colors
			{
				if(keys_repeat & KEY_LEFT)
					edit_color = (edit_color & 0xC) | ((edit_color-1) & 0x3);
				if(keys_repeat & KEY_RIGHT)
					edit_color = (edit_color & 0xC) | ((edit_color+1) & 0x3);
				if(keys_repeat & KEY_UP)
					edit_color = (edit_color - 4) & 15;
				if(keys_repeat & KEY_DOWN)
					edit_color = (edit_color + 4) & 15;
				if(keys_down & (KEY_X | KEY_A | KEY_B)) {
					edit_state = 0;
					wait_for_release = true;
				}
				if(keys_down & KEY_Y) {
					edit_state = 2;
					have_other_coordinate = false;
					tool_when_switching_to_tools = edit_tool;
				}
				break;
			}
			case 2: // Tools
			{
				if(keys_repeat & (KEY_LEFT | KEY_RIGHT))
					edit_tool_within_page ^= 1;
				if(keys_repeat & KEY_UP) {
					if(edit_tool_within_page <= 1)
						edit_tool_within_page += 10;
					edit_tool_within_page = (edit_tool_within_page - 2);
				}
				if(keys_repeat & KEY_DOWN)
					edit_tool_within_page = (edit_tool_within_page + 2) % 10;
				if(keys_down & KEY_X)
					edit_state = 1;
				if(keys_down & KEY_Y)
					edit_state = 0;
				if(edit_state != 2) {
					switch(edit_tool) { // Reset back to old tool
						case TOOL_LESS_TOOLS:
						case TOOL_MORE_TOOLS:
						case TOOL_UNDO:
						case TOOL_REDO:
						case TOOL_ROTATE_L:
						case TOOL_ROTATE_R:
						case TOOL_HFLIP:
						case TOOL_VFLIP:
						case TOOL_PALETTE_PREV:
						case TOOL_PALETTE_NEXT:
						case TOOL_UNUSED:
							edit_tool_page = tool_when_switching_to_tools / 10;
							edit_tool_within_page = tool_when_switching_to_tools % 10;
							draw_pattern_editor_tool_page(edit_tool_page);
							break;
					}
				}
				if(keys_down & KEY_A) {
					switch(edit_tool) {
						case TOOL_1PX: case TOOL_2PX:
						case TOOL_3PX: case TOOL_LINE:
						case TOOL_RECT_LINE:  case TOOL_RECT_FILL:
						case TOOL_FLOOD_FILL: case TOOL_SHIFT:
						case TOOL_SWAP_COLORS:
							edit_state = 0;
							wait_for_release = true;
							break;

						case TOOL_LESS_TOOLS: case TOOL_MORE_TOOLS:
							edit_tool_page ^= 1;
							draw_pattern_editor_tool_page(edit_tool_page);
							break;
						case TOOL_UNDO:
							break;
						case TOOL_REDO:
							break;

						case TOOL_ROTATE_L:
						case TOOL_ROTATE_R:
						{
							u8 temp[32][32];
							for(int y=0; y<32;y++)
								for(int x=0; x<32; x++)
									if(edit_tool == TOOL_ROTATE_L)
										temp[x][y] = get_pattern_pixel(31-y, x);
									else
										temp[x][y] = get_pattern_pixel(y, 31-x);
							for(int y=0; y<32;y++)
								for(int x=0; x<32; x++)
									put_pattern_pixel(x, y, temp[x][y]);
							pattern_edited = true;
							break;
						}
						case TOOL_VFLIP:
							for(int y=0; y<16; y++)
								for(int x=0; x<32; x++) {
									swap_pattern_pixels(x, y, x, 31-y);
								}
							pattern_edited = true;
							break;
						case TOOL_HFLIP:
							for(int y=0; y<31; y++)
								for(int x=0; x<16; x++) {
									swap_pattern_pixels(x, y, 31-x, y);
								}
							pattern_edited = true;
							break;
						case TOOL_UNUSED:
							break;
						case TOOL_PALETTE_PREV:
							pattern_edit_palette = (pattern_edit_palette-1) & 15;
							redraw_edited_pattern();
							break;
						case TOOL_PALETTE_NEXT:
							pattern_edit_palette = (pattern_edit_palette+1) & 15;
							redraw_edited_pattern();
							break;
					}
				}
				break;
			}
		}
		if(pattern_edited) {
			redraw_edited_pattern();
		}

		oamSet(&oamMain,
			0,      //OAM index
			edit_x*6+EDIT_PATTERN_CANVAS_X-1, edit_y*6-1,
			0,      // Priority
			0,      // Palette
			SpriteSize_16x16, SpriteColorFormat_16Color, (edit_state==0)?SPRITE_MAIN_TILE_POINTER(8):SPRITE_MAIN_TILE_POINTER(12),
			-1, false, false, false, false, false
			);

		oamSet(&oamMain,
			1,      //OAM index
			EDIT_PATTERN_COLORS_X+((edit_color&3)*14)-1, EDIT_PATTERN_COLORS_Y+(edit_color/4*14)-1,
			0,      // Priority
			0,      // Palette
			SpriteSize_16x16, SpriteColorFormat_16Color, (edit_state==1)?SPRITE_MAIN_TILE_POINTER(0x18):SPRITE_MAIN_TILE_POINTER(0x1C),
			-1, false, false, false, false, false
			);

		oamSet(&oamMain,
			2,      //OAM index
			EDIT_TOOLS_X+((edit_tool_within_page&1)*24)+4, EDIT_TOOLS_Y+(edit_tool_within_page/2*16),
			0,      // Priority
			0,      // Palette
			SpriteSize_32x16, SpriteColorFormat_16Color, (edit_state==2)?SPRITE_MAIN_TILE_POINTER(0x40):SPRITE_MAIN_TILE_POINTER(0x48),
			-1, false, false, false, false, false
			);

		if(have_other_coordinate) {
			u16 *sprite = SPRITE_MAIN_TILE_POINTER(0);
			if(edit_tool == TOOL_RECT_LINE || edit_tool == TOOL_RECT_FILL) { // For rectangles, show a rectangle preview
				int x0 = edit_x < other_coord_x ? edit_x : other_coord_x;
				int x1 = edit_x > other_coord_x ? edit_x : other_coord_x;
				int y0 = edit_y < other_coord_y ? edit_y : other_coord_y;
				int y1 = edit_y > other_coord_y ? edit_y : other_coord_y;
				oamSet(&oamMain,
					3,      //OAM index
					x0*6+EDIT_PATTERN_CANVAS_X-1, y0*6-1,
					0,      // Priority
					0,      // Palette
					SpriteSize_8x8, SpriteColorFormat_16Color, sprite,
					-1, false, false, false, false, false
					);
				oamSet(&oamMain,
					4,      //OAM index
					x1*6+EDIT_PATTERN_CANVAS_X-1, y0*6-1,
					0,      // Priority
					0,      // Palette
					SpriteSize_8x8, SpriteColorFormat_16Color, sprite,
					-1, false, false, false, false, false
					);
				oamSet(&oamMain,
					5,      //OAM index
					x0*6+EDIT_PATTERN_CANVAS_X-1, y1*6-1,
					0,      // Priority
					0,      // Palette
					SpriteSize_8x8, SpriteColorFormat_16Color, sprite,
					-1, false, false, false, false, false
					);
				oamSet(&oamMain,
					6,      //OAM index
					x1*6+EDIT_PATTERN_CANVAS_X-1, y1*6-1,
					0,      // Priority
					0,      // Palette
					SpriteSize_8x8, SpriteColorFormat_16Color, sprite,
					-1, false, false, false, false, false
					);
			} else { // Lines just show one coordinate
				oamSet(&oamMain,
					3,      //OAM index
					other_coord_x*6+EDIT_PATTERN_CANVAS_X-1, other_coord_y*6-1,
					0,      // Priority
					0,      // Palette
					SpriteSize_8x8, SpriteColorFormat_16Color, sprite,
					-1, false, false, false, false, false
					);
			}
		} else {
			oamSetHidden(&oamMain, 3, true);
			oamSetHidden(&oamMain, 4, true);
			oamSetHidden(&oamMain, 5, true);
			oamSetHidden(&oamMain, 6, true);
		}

		// Exit
		if(keys_down & KEY_START)
			break;
	}

	set_default_video_mode();
}
