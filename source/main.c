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
#include <sys/stat.h> // For mkdir
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
void menu_player_edit();
void menu_utility();
void load_extra_storage();
void save_extra_storage();

// ------------------------------------------------------------------------------------------------
// Strings
const char *main_menu_options[] = {"Save/Load", "Storage", "Map", "Patterns", "House", "Utilities", "Edit player", "Quit"};
const char *file_options[] = {"Load file", "Save file", "Save backup", "Load from cartridge", "Save to cartridge"};
const char *title_screen_options[] = {"Load from SD card", "Load from cartridge", "Quit"};

// ------------------------------------------------------------------------------------------------
// Variables

char filename[200];
char full_file_path[256];
char title_buffer[32];
char text_conversion_buffer[40];
u8 savefile[256 * 1024];

bool has_acww_folder;
const char *acww_folder_prefix = "";

u16 *mainBGMap;
u16 *mainBGMap2;
u16 *subBGMap;
u16 *subBGMap2;

// ------------------------------------------------------------------------------------------------

const char *text_from_save(int index, int length) {
	acstrDecode(text_conversion_buffer, &savefile[index], length);
	return text_conversion_buffer;
}

const char *town_name() {
	return text_from_save(4, 8);
}

void show_player_information() {
	clear_screen(subBGMap);
	map_print(subBGMap, 1, 1, full_file_path);
	map_printf(subBGMap, 1, 2, "Town: %s",     town_name());
	map_printf(subBGMap, 1, 3, "Player 1: %s", text_from_save(0x228E + PER_PLAYER_OFFSET*0, 8));
	map_printf(subBGMap, 1, 4, "Player 2: %s", text_from_save(0x228E + PER_PLAYER_OFFSET*1, 8));
	map_printf(subBGMap, 1, 5, "Player 3: %s", text_from_save(0x228E + PER_PLAYER_OFFSET*2, 8));
	map_printf(subBGMap, 1, 6, "Player 4: %s", text_from_save(0x228E + PER_PLAYER_OFFSET*3, 8));
	//map_printf(subBGMap, 1, 4, "Money: %d", *((int*)&savefile[0x01B4C + player_offset]));
}

int verify_loaded_savefile() {
	if(memcmp(savefile+2, savefile+2+TOWN_SAVE_SIZE, 10) != 0) { // Name of the town should be the same between both copies of the town
		popup_notice("That isn't a valid savefile");
		return 0;
	}
	show_player_information();
	load_extra_storage();
	return 1;
}

int reload_savefile() {
	FILE *file = fopen(filename, "rb");
	if(file == NULL) {
		popup_notice("Can't open savefile");
		return 0;
	}
	size_t r = fread(savefile, 1, sizeof(savefile), file);
	fclose(file);
	if(r != sizeof(savefile)) {
		popup_notice("That isn't a valid savefile");
		return 0;
	}
	return verify_loaded_savefile();
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

int verify_cartridge_flash_type() {
	int type = cardEepromGetType();
	if(type == 3) {
		if(cardEepromGetSize() == sizeof(savefile)) {
			return 1;
		}
		popup_notice("Cartridge's save isn't 256K");
	} else {
		popup_notice("Cartridge doesn't have flash");
	}
	return 0;
}

int load_cartridge() {
	if(!verify_cartridge_flash_type())
		return 0;
	cardReadEeprom(0, savefile, sizeof(savefile), 3); // 3 = flash
	return verify_loaded_savefile();
}

int save_cartridge() {
	if(!verify_cartridge_flash_type())
		return 0;
	fix_checksum();
	cardEepromChipErase();
	cardWriteEeprom(0, savefile, sizeof(savefile), 3); // 3 = flash
	return 1;
}

void get_backup_filename() {
	time_t timer;
	struct tm* tm_info;
	char buffer[60];

	// Create a file with the current date and time on it
	timer = time(NULL);
	tm_info = localtime(&timer);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H-%M.sav", tm_info);
	sprintf(full_file_path, "%s%s %s", acww_folder_prefix, town_name(), buffer);
}

void menu_save_load() {
	switch(choose_from_list("Save/Load", file_options, 5, 0)) {
		case 0: // Load
			if(choose_file() == 1) {
				reload_savefile();
			}		
			break;
		case 1: // Save
		{
			fix_checksum();
			FILE *file = fopen(filename, "wb");
			size_t w = fwrite(savefile, 1, sizeof(savefile), file);
			fclose(file);
			if(w == sizeof(savefile)) {
				popup_notice("Saved successfully!");
				save_extra_storage();
			} else {
				popup_notice("Unable to save");
			}
			break;
		}
		case 2: // Save backup
		{
			fix_checksum();

			get_backup_filename();
			FILE *file = fopen(full_file_path, "wb");
			size_t w = fwrite(savefile, 1, sizeof(savefile), file);
			fclose(file);
			if(w == sizeof(savefile)) {
				popup_notice("Backup successful!");
			} else {
				popup_notice("Unable to backup");
			}
			break;
		}
		case 3: // Load from cartridge
			if(confirm_choice("Load from cartridge?") == 1 && load_cartridge()) {
				popup_notice("Loaded from cartridge!");
			}
			break;
		case 4: // Save to cartridge
			if(confirm_choice("Save to cartridge?") == 1 && save_cartridge()) {
				popup_notice("Saved to cart! Hopefully");
			}
			break;
	}
}

void menu_map() {

}

void menu_patterns() {

}

void menu_house() {

}

int makeFolder(const char *path) {
	// from RAC by PinoBatch
	if(mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0) {
		if(errno != EEXIST) {
			return -1;
		} 
	}
	return 0;
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

	//keyboardShow();

	bool init_ok = fatInitDefault();
	if(!init_ok) {
		map_print(mainBGMap,  0, 0, "fatInitDefault() error");
		map_print(mainBGMap,  0, 1, "Push start to exit...");
		wait_for_start();
		return 0;
	}

	// Create ACWW folder
    has_acww_folder = makeFolder("/data") >= 0 && makeFolder("/data/acww") >= 0;
	if(has_acww_folder) {
		acww_folder_prefix = "/data/acww/";
	}

	// --------------------------------------------------------------
	// "Title screen"
	clear_screen(subBGMap);
	map_box(subBGMap, 0, 9, 32, 6);
	map_print(subBGMap,  1, 10, "WildStorage");
	map_print(subBGMap,  1, 11, "An ACWW item storage tool");
	map_print(subBGMap,  1, 13, "made by NovaSquirrel");
	map_print(subBGMap,  1, 20, "Version 0.0.1");

	int title_screen_y = 0;
	bool proceed_from_title_screen = 0;
	while(!proceed_from_title_screen) {
		int new_title_screen_y = choose_from_list("WildStorage", title_screen_options, 3, title_screen_y);
		if(new_title_screen_y < 0)
			continue;
		title_screen_y = new_title_screen_y;
		switch(title_screen_y) {
			case 0: // Load SD card
			{
				int result = choose_file();
				if(result == -2) {
					map_print(mainBGMap,  0, 0, "opendir() error");
					map_print(mainBGMap,  0, 1, "Push start to exit...");
					wait_for_start();
					return 0;
				}
				if(result == 1) { // Success
					if(reload_savefile())
						proceed_from_title_screen = 1;
				}
				break;
			}
			case 1: // Load cartridge
			{
				if(load_cartridge()) {
					proceed_from_title_screen = 1;
				}
				break;
			}
			case 2: // quit
				return 0;
		}
	}

	// Main menu
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
				menu_utility();
				break;
			case 6:
				menu_player_edit();
				break;
			case 7:
				return 0;
			default:
				main_menu_y = 0;
				break;
		}
	}
}
