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

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <fatfs.h>
#include <nds.h>
#include <nds/arm9/dldi.h>
#include <my_font.h>
#include <time.h>
#include "wild.h"
#include "acstr.h"

// ------------------------------------------------------------------------------------------------
// Prototypes
void menu_storage();

// ------------------------------------------------------------------------------------------------
// Strings
const char *main_menu_options[] = {"Save/Load", "Storage", "Map", "Patterns", "House", "Miscellaneous", "Edit player", "Quit"};
const char *file_options[] = {"Load file", "Save file", "Save backup"};
const char *player_options[] = {"1st player", "2nd player", "3rd player", "4th player"};
const char *misc_menu_options[] = {"Emotions", "Face/Hair"};

// ------------------------------------------------------------------------------------------------
// Variables

char full_file_path[256];
char title_buffer[32];
char text_conversion_buffer[40];
u8 savefile[256 * 1024];

u16 *mainBGMap;
u16 *mainBGMap2;
u16 *subBGMap;
u16 *subBGMap2;

// ------------------------------------------------------------------------------------------------

const char *text_from_save(int index, int length) {
	acstrDecode(text_conversion_buffer, &savefile[index], length);
	return text_conversion_buffer;
}

void show_player_information() {
	clear_screen(subBGMap);
	map_print(subBGMap, 1, 1, full_file_path);
	map_printf(subBGMap, 1, 2, "Town: %s", text_from_save(4, 8));
	map_printf(subBGMap, 1, 3, "Player 1: %s", text_from_save(0x228E + 8844*0, 8));
	map_printf(subBGMap, 1, 4, "Player 2: %s", text_from_save(0x228E + 8844*1, 8));
	map_printf(subBGMap, 1, 5, "Player 3: %s", text_from_save(0x228E + 8844*2, 8));
	map_printf(subBGMap, 1, 6, "Player 4: %s", text_from_save(0x228E + 8844*3, 8));
	//map_printf(subBGMap, 1, 4, "Money: %d", *((int*)&savefile[0x01B4C + player_offset]));
}

int reload_savefile() {
	FILE *file = fopen(full_file_path, "rb");
	size_t r = fread(savefile, 1, sizeof(savefile), file);
	fclose(file);
	int success = r == sizeof(savefile);
	if(memcmp(savefile+2, savefile+2+TOWN_SAVE_SIZE, 10) != 0) { // Name of the town should be the same between both copies of the town
		return 0;
	}
	if(success) {
		show_player_information();
	}
	return success;
}

void wait_for_start() {
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		uint32_t keys_down = keysDown();
		if(keys_down & KEY_START)
			break;
	}
}

void fix_checksum() {
	u16 total = 0;
	for(int i=0; i < TOWN_SAVE_SIZE; i += 2) {
		total += get_savefile_u16(i);
	}
	u16 checksum_in_save = get_savefile_u16(TOWN_SAVE_SIZE-4);
	if(total != 0) {
		set_savefile_u16(TOWN_SAVE_SIZE-4, checksum_in_save - total);
	}
	memcpy(savefile + TOWN_SAVE_SIZE, savefile, TOWN_SAVE_SIZE);
}

void menu_save_load() {
	switch(choose_from_list("Save/Load", file_options, 3, 0)) {
		case 0: // Load
			if(choose_file() == 1) {
				if(reload_savefile())
					break;
				else {
					popup_notice("That isn't a valid savefile");
				}
			}		
			break;
		case 1: // Save
		{
			fix_checksum();
			FILE *file = fopen(full_file_path, "wb");
			size_t w = fwrite(savefile, 1, sizeof(savefile), file);
			fclose(file);
			if(w == sizeof(savefile)) {
				popup_notice("Saved successfully!");
			} else {
				popup_notice("Unable to save");
			}
			break;
		}
		case 2: // Save backup
		{
			fix_checksum();
			time_t timer;
			struct tm* tm_info;
			char buffer[64];

			// Create a file with the current date and time on it
			timer = time(NULL);
			tm_info = localtime(&timer);
			strftime(buffer, sizeof(buffer), "ACWW %Y-%m-%d %H-%M.sav", tm_info);

			FILE *file = fopen(buffer, "wb");
			size_t w = fwrite(savefile, 1, sizeof(savefile), file);
			fclose(file);
			if(w == sizeof(savefile)) {
				popup_notice("Backup successful!");
			} else {
				popup_notice("Unable to backup");
			}
			break;
		}
	}
}

void menu_map() {

}

void menu_patterns() {

}

void menu_house() {

}

void menu_miscellaneous() {

}

void menu_switch_player() {
	/*
	int result = choose_from_list("Which player?", player_options, 4, 0);
	if(result >= 0)
		current_player = result;
	player_offset = current_player * 8844;
	show_player_information();
	*/
}

int main(int argc, char **argv) {
	lcdMainOnBottom();
	keysSetRepeat(16, 5);

	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	vramSetBankD(VRAM_D_SUB_SPRITE);
	vramSetBankE(VRAM_E_LCD);
	vramSetBankF(VRAM_F_LCD);
	vramSetBankG(VRAM_G_LCD);
	vramSetBankH(VRAM_H_LCD);
	vramSetBankI(VRAM_I_LCD);

	// https://mtheall.com/vram.html#T0=2&NT0=1024&MB0=0&TB0=1&S0=1&T1=2&NT1=1024&MB1=2&TB1=3&S1=1&T2=8&MB2=4&TB2=5
	Keyboard kb2;
	videoSetMode(MODE_0_2D);
	int mainBG  = bgInit(0, BgType_Text4bpp, BgSize_T_512x256, 0, 1);
	int mainBG2 = bgInit(1, BgType_Text4bpp, BgSize_T_512x256, 2, 3);
	keyboardInit(&kb2, 2, BgType_Text4bpp, BgSize_T_256x512, 4, 5, true, true);

	videoSetModeSub(MODE_0_2D);
	int subBG  = bgInitSub(0, BgType_Text4bpp, BgSize_T_512x256, 0, 1);
	int subBG2 = bgInitSub(1, BgType_Text4bpp, BgSize_T_512x256, 2, 3);

    mainBGMap  = (u16*)bgGetMapPtr(mainBG);
    subBGMap   = (u16*)bgGetMapPtr(subBG);
    mainBGMap2 = (u16*)bgGetMapPtr(mainBG2);
    subBGMap2  = (u16*)bgGetMapPtr(subBG2);

	dmaCopy(my_fontTiles, bgGetGfxPtr(mainBG),  sizeof(my_fontTiles));
	dmaCopy(my_fontTiles, bgGetGfxPtr(subBG),   sizeof(my_fontTiles));
	dmaCopy(my_fontTiles, bgGetGfxPtr(mainBG2), sizeof(my_fontTiles));
	dmaCopy(my_fontTiles, bgGetGfxPtr(subBG2),  sizeof(my_fontTiles));
	dmaCopy(my_fontPal,   BG_PALETTE,           sizeof(my_fontPal));
	dmaCopy(my_fontPal,   BG_PALETTE_SUB,       sizeof(my_fontPal));

    setBrightness(3, 0); // Both screens full brightness

	bool init_ok = fatInitDefault();
	if(!init_ok) {
		map_print(mainBGMap,  0, 0, "fatInitDefault() error");
		map_print(mainBGMap,  0, 1, "Push start to exit...");
		wait_for_start();
		return 0;
	}

	// --------------------------------------------------------------
	while(1) {
		int result = choose_file();
		if(result == -2) {
			map_print(mainBGMap,  0, 0, "opendir() error");
			map_print(mainBGMap,  0, 1, "Push start to exit...");
			wait_for_start();
			return 0;
		}
		if(result == 1) { // Success
			if(reload_savefile())
				break;
			else {
				popup_notice("That isn't a valid savefile");
			}
		}
	}

	int main_menu_y = 0;
	while(1) {
		sprintf(title_buffer, "Main menu");
		main_menu_y = choose_from_list(title_buffer, main_menu_options, 8, main_menu_y);
		switch(main_menu_y) {
			case 0:
				menu_save_load();
				break;
			case 1:
				menu_storage();
				show_player_information();
				break;
			case 2:
				menu_map();
				break;
			case 3:
				menu_patterns();
				break;
			case 4:
				menu_house();
				break;
			case 5:
				menu_miscellaneous();
				break;
			case 6:
				menu_switch_player();
				break;
			case 7:
				return 0;
			default:
				main_menu_y = 0;
				break;
		}
	}
}
