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
#include <time.h>
#include "wild.h"
#include "acstr.h"

#include <my_font.h>
#include <background.h>

// ------------------------------------------------------------------------------------------------
// Prototypes
void menu_storage();
void menu_player_edit();
void menu_utility();
void menu_patterns();
void load_extra_storage();
void save_extra_storage();
void load_extra_patterns();
void save_extra_patterns();
const char *getNeighborName(size_t id);

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
char text_conversion_buffer[64];
u8 savefile[256 * 1024];

bool has_acww_folder;
const char *acww_folder_prefix = "";

int mainBGText, mainBG256, mainBGBehind, subBGText, subBG256, subBGBehind;
u16 *mainBGMapText;
u16 *mainBGMap256;
u16 *mainBGMapBehind;
u16 *subBGMapText;
u16 *subBGMap256;
u16 *subBGMapBehind;

// ------------------------------------------------------------------------------------------------

const char *text_from_save(int index, int length) {
	acstrDecode(text_conversion_buffer, &savefile[index], length);
	return text_conversion_buffer;
}

const char *town_name() {
	return text_from_save(4, 8);
}

void show_town_information_on_top_screen() {
	clear_screen(subBGMapText);
	map_print(subBGMapText, 1, 1, filename);
	map_printf(subBGMapText, 1, 2, "Town: %s",     town_name());

	map_printf(subBGMapText, 1, 4, "Players:");
	for(int i=0; i<4; i++) {
		const char *name = player_name_or_null(i);
		if(name)
			map_printf(subBGMapText, 1+(i&1)*16, 5+i/2, "* %s", name);
		else
			map_printf(subBGMapText, 1+(i&1)*16, 5+i/2, "* Empty");
	}

	map_print(subBGMapText, 1, 8, "Villagers:");
	for(int i=0; i<8; i++) {
		int identity = savefile[0x09094 + 0x700*i + 0x73]; // romsave.txt seems to have the wrong offset here
		const char *name = getNeighborName(identity);
		if(identity < 150) {
			map_printf(subBGMapText, 1+(i&1)*16, 9+i/2, "* %s", name);
		}
	}

	if(savefile[0x11438] || savefile[0x11439]) {
		int ninth_identity = savefile[0x11443];
		const char *ninth_name = getNeighborName(ninth_identity);

		char town1[8*3+1];
		char town2[8*3+1];
		acstrDecode(town1, &savefile[0x11438+2], 8);
		acstrDecode(town2, &savefile[0x1144C+2], 8);

		if(ninth_identity < 150) {
			map_print(subBGMapText,  1, 14, "Moving:");
			map_printf(subBGMapText, 1, 15, "* %s from %s", ninth_name, town1);
			if(strcmp(town1, town2)) {
				map_printf(subBGMapText, 1, 16, "  and %s", town2);
			}
		}
	}

	//map_printf(subBGMapText, 1, 4, "Money: %d", *((int*)&savefile[0x01B4C + player_offset]));
}

int verify_loaded_savefile() {
	if(memcmp(savefile+2, savefile+2+TOWN_SAVE_SIZE, 10) != 0) { // Name of the town should be the same between both copies of the town
		popup_notice("That isn't a valid savefile");
		return 0;
	}
	show_town_information_on_top_screen();
	load_extra_storage();
	load_extra_patterns();
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

int background_scroll = 0;
void wait_vblank_and_animate() {
	swiWaitForVBlank();
	background_scroll++;
	int background_scroll_slower = background_scroll / 8;
	bgSetScroll(mainBGBehind, background_scroll_slower, background_scroll_slower);
	bgSetScroll(subBGBehind,  background_scroll_slower, background_scroll_slower);
	bgUpdate();
}

void setup_scrolling_background(u16 *map) {
	for(int i=0; i<1024; i++) {
		map[i] = (i & 3) | (((i / 32) & 3) * 4) | TILE_PALETTE(1);
	}
}

void wait_for_start() {
	while(1) {
		wait_vblank_and_animate();
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
			if(choose_file(SAVE_FILES) == 1) {
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
				save_extra_patterns();
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

	// https://mtheall.com/vram.html#T0=2&NT0=1024&MB0=0&TB0=1&S0=1&T1=1&NT1=1024&MB1=2&TB1=3&S1=1&T2=2&NT2=512&MB2=4&TB2=7&S2=2
	videoSetMode(MODE_0_2D);
	mainBGText   = bgInit(0, BgType_Text4bpp, BgSize_T_512x256, 0, 1);
	mainBG256    = bgInit(1, BgType_Text8bpp, BgSize_T_512x256, 2, 3);
	mainBGBehind = bgInit(2, BgType_Text4bpp, BgSize_T_256x256, 4, 7);

	videoSetModeSub(MODE_0_2D);
	subBGText    = bgInitSub(0, BgType_Text4bpp, BgSize_T_512x256, 0, 1);
	subBG256     = bgInitSub(1, BgType_Text8bpp, BgSize_T_512x256, 2, 3);
	subBGBehind  = bgInitSub(2, BgType_Text4bpp, BgSize_T_256x256, 4, 7);

    mainBGMapText   = (u16*)bgGetMapPtr(mainBGText);
    mainBGMap256    = (u16*)bgGetMapPtr(mainBG256);
    mainBGMapBehind = (u16*)bgGetMapPtr(mainBGBehind);
    subBGMapText    = (u16*)bgGetMapPtr(subBGText);
    subBGMap256     = (u16*)bgGetMapPtr(subBG256);
    subBGMapBehind  = (u16*)bgGetMapPtr(subBGBehind);

	dmaCopy(my_fontTiles,    bgGetGfxPtr(mainBGText)  ,sizeof(my_fontTiles));
	dmaFillHalfWords(0,      bgGetGfxPtr(mainBG256),   64);
	dmaCopy(backgroundTiles, bgGetGfxPtr(mainBGBehind),sizeof(backgroundTiles));
	dmaCopy(my_fontTiles,    bgGetGfxPtr(subBGText),   sizeof(my_fontTiles));
	dmaFillHalfWords(0,      bgGetGfxPtr(subBG256),    64);
	dmaCopy(backgroundTiles, bgGetGfxPtr(subBGBehind), sizeof(backgroundTiles));
	dmaCopy(my_fontPal,      BG_PALETTE,               sizeof(my_fontPal));
	dmaCopy(my_fontPal,      BG_PALETTE_SUB,           sizeof(my_fontPal));
	dmaCopy(backgroundPal,   BG_PALETTE+16,            sizeof(backgroundPal));
	dmaCopy(backgroundPal,   BG_PALETTE_SUB+16,        sizeof(backgroundPal));
	clear_screen_256(mainBGMap256);
	clear_screen_256(subBGMap256);
	setup_scrolling_background(mainBGMapBehind);
	setup_scrolling_background(subBGMapBehind);

    setBrightness(3, 0); // Both screens full brightness

	//keyboardShow();

	bool init_ok = fatInitDefault();
	if(!init_ok) {
		map_print(mainBGMapText,  0, 0, "fatInitDefault() error");
		map_print(mainBGMapText,  0, 1, "Push start to exit...");
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
	clear_screen(subBGMapText);
	map_box(subBGMapText, 0, 9, 32, 6);
	map_print(subBGMapText,  1, 10, "WildStorage");
	map_print(subBGMapText,  1, 11, "An ACWW item storage tool");
	map_print(subBGMapText,  1, 13, "made by NovaSquirrel");
	map_print(subBGMapText,  1, 20, "Version 0.0.1");

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
				int result = choose_file(SAVE_FILES);
				if(result == -2) {
					map_print(mainBGMapText,  0, 0, "opendir() error");
					map_print(mainBGMapText,  0, 1, "Push start to exit...");
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
		int new_menu_y = choose_from_list(title_buffer, main_menu_options, 8, main_menu_y);
		if(new_menu_y >= 0)
			main_menu_y = new_menu_y;
		switch(main_menu_y) {
			case 0:
				menu_save_load();
				break;
			case 1:
				menu_storage();
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
		show_town_information_on_top_screen();
	}
}
