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
#include "acstr.h"

// ------------------------------------------------------------------------------------------------
// Strings
const char *utility_options[] = {"No Resetti", "Remove weeds", "Revive all flowers", "Water all flowers", "Replenish fruit", "Un-bury items", "Remove stray items", "Sort closets", "Sort extra storage", "Reset pattern order", "Perfect town acre check"};

// ------------------------------------------------------------------------------------------------
// Variables
extern u16 extra_storage[3][EXTRA_STORAGE_SIZE];
extern int edited_extra_storage[3];

// ------------------------------------------------------------------------------------------------
// Functions

int item_sort_compare(const void *a, const void *b) {
	u16 *item_a = (u16*)a;
	u16 *item_b = (u16*)b;
	return *item_a - *item_b;
}

void menu_utility() {
	static int which_option = 0;

	while(1) {
		int option = choose_from_list("Utility features", utility_options, 11, which_option);
		if(option < 0)
			return;
		which_option = option;

		switch(which_option) {
			case 0: // No resetti
				savefile[0x2208 + PER_PLAYER_OFFSET * 0] &= ~4;
				savefile[0x2208 + PER_PLAYER_OFFSET * 1] &= ~4;
				savefile[0x2208 + PER_PLAYER_OFFSET * 2] &= ~4;
				savefile[0x2208 + PER_PLAYER_OFFSET * 3] &= ~4;
				popup_notice("Resetti flag cleared");
				break;
			case 1: // Remove weeds
			{
				int weeds = 0;
				for(int i=TOWN_ITEM_GRID_START; i<=TOWN_ITEM_GRID_END; i+=2) {
					u16 item = get_savefile_u16(i);
					if((item >= 0x1d && item <= 0x0024) || item == 0x001b || item == 0x0089) { // Weeds, but also rafflesia
						set_savefile_u16(i, EMPTY_ITEM);
						weeds++;
					}
				}
				popup_noticef("Removed %d weeds", weeds);
				break;
			}
			case 2: // Revive flowers
			{
				int flowers = 0, flowers2 = 0;
				for(int i=TOWN_ITEM_GRID_START; i<=TOWN_ITEM_GRID_END; i+=2) {
					u16 item = get_savefile_u16(i);
					if(item >= 0x6E && item <= 0x89) {
						item -= 0x6E;
						flowers++;
					} else if(item >= 0x8A && item <= 0xA4) {
						item -= 0x8A;
						flowers2++;
					}
					set_savefile_u16(i, item);
				}
				popup_noticef("%d flowers (%d were watered)", flowers, flowers2);
				break;
			}
			case 3: // Water flowers
			{
				int flowers = 0;
				for(int i=TOWN_ITEM_GRID_START; i<=TOWN_ITEM_GRID_END; i+=2) {
					u16 item = get_savefile_u16(i);
					if(item >= 0x6E && item <= 0x88) {
						item = item - 0x6E + 0x8A;
						flowers++;
					}
					set_savefile_u16(i, item);
				}
				popup_noticef("Watered %d flowers", flowers);
				break;
			}
			case 4: // Replenish fruit
			{
				int fruit = 0;
				for(int i=TOWN_ITEM_GRID_START; i<=TOWN_ITEM_GRID_END; i+=2) {
					u16 item = get_savefile_u16(i);

					if(item >= 0x34 && item <= 0x36) {
						item = 0x33;
						fruit++;
					} else if(item >= 0x3C && item <= 0x3E) {
						item = 0x3B;
						fruit++;
					} else if(item >= 0x44 && item <= 0x46) {
						item = 0x44;
						fruit++;
					} else if(item >= 0x4C && item <= 0x4E) {
						item = 0x4B;
						fruit++;
					} else if(item >= 0x54 && item <= 0x56) {
						item = 0x53;
						fruit++;
					} else if(item >= 0xCD && item <= 0xCF) {
						item = 0xCC;
						fruit++;
					}
					set_savefile_u16(i, item);
				}
				popup_noticef("Restocked %d trees", fruit);
				break;
			}
			case 5: // Un-bury items
			{
				int buried = 0;
				for(int i=0x0E354; i<=0x0E553; i++) {
					if(savefile[i]) {
						for(int j=0x80; j; j>>=1) {
							if(savefile[i] & j)
								buried++;
						}
						savefile[i] = 0;
					}
				}
				popup_noticef("Un-buried %d items", buried);
				break;
			}
			case 6: // Remove stray items
			{
				int stray = 0;
				for(int i=TOWN_ITEM_GRID_START; i<=TOWN_ITEM_GRID_END; i+=2) {
					u16 item = get_savefile_u16(i) & 0xf000;
					if(item == 0x1000 || item == 0x3000 || item == 0x4000) {
						set_savefile_u16(i, EMPTY_ITEM);
						stray++;
					}
				}
				popup_noticef("Removed %d stray items", stray);
				break;
			}
			case 7: // Sort closets
				qsort(&savefile[0x15430 + 180*0], 90, 2, item_sort_compare);
				qsort(&savefile[0x15430 + 180*1], 90, 2, item_sort_compare);
				qsort(&savefile[0x15430 + 180*2], 90, 2, item_sort_compare);
				qsort(&savefile[0x15430 + 180*3], 90, 2, item_sort_compare);
				popup_notice("Sorted all players' closets");
				break;
			case 8: // Sort extra storage				
				qsort(extra_storage[0], EXTRA_STORAGE_SIZE, 2, item_sort_compare);
				qsort(extra_storage[1], EXTRA_STORAGE_SIZE, 2, item_sort_compare);
				qsort(extra_storage[2], EXTRA_STORAGE_SIZE, 2, item_sort_compare);
				edited_extra_storage[0] = 1;
				edited_extra_storage[1] = 1;
				edited_extra_storage[2] = 1;
				popup_notice("Sorted extra storage");
				break;
			case 9: // Reset pattern inventory order
			{
				for(int i=0; i<4; i++) {
					for(int j=0; j<8; j++) {
						savefile[0x114c + PER_PLAYER_OFFSET * i + j] = j;
					}
				}
				popup_notice("Reset the pattern ordering");
				break;
			}
			case 10: // Perfect town acre check
			{
				clear_screen(mainBGMapText);

				map_print(mainBGMapText,  0,  0,  "Trees and flowers per-acre:");
				map_print(mainBGMapText,  0,  6,  "Weeds and items per-acre:");
				map_print(mainBGMapText,  0,  12, "Acre status:");

				int num_perfect = 0;
				int num_ok = 0;
				int num_unacceptable = 0;
				// Based on https://www.thonky.com/acww/perfect-town-guide and some guesses!
				for(int y=0; y < 4; y++) {
					for(int x=0; x < 4; x++) {
						int base = TOWN_ITEM_GRID_START + (512 * 4 * y) + (512 * x);
						int trees = 0;
						int flowers = 0;
						int weeds = 0;
						int items = 0;
						for(int i=0; i<512; i+=2) {
							u16 item = get_savefile_u16(base + i);
							if(item == 0x001b || item == 0x0089 || (item >= 0x001f && item <= 0x0024)) // Weeds or rafflesia
								weeds++;
							else if(item >= 0x0025 && item <= 0x006d)
								trees++;
							else if((item >= 0x0000 && item <= 0x001a) || (item >= 0x006e && item <= 0x00a5))
								flowers++;
							else {
								u16 icon = icon_for_item(item);
								switch(icon) {
                                    // Not sure exactly what items are counted here! I think this should probably be close enough though.
									case ICON_ITEM: case ICON_FURNITURE: case ICON_SHIRT: case ICON_HAT:
									case ICON_GLASSES: case ICON_BELLS: case ICON_TOOL: case ICON_MUSIC:
									case ICON_UMBRELLA: case ICON_GYROID: case ICON_FOSSIL_ID: case ICON_TRASH:
									case ICON_FISH: case ICON_BUG: case ICON_FLOWERBAG: case ICON_SAPLING:
									case ICON_PAPER: case ICON_PICTURE:
										items++;
										break;
								}
								if(item >= 0x137d && item <= 0x137f) // Explosives
									items++;
							}
						}
						map_printf(mainBGMapText,  x*4,    y+1, "%.2d\xd3", trees);	
						map_printf(mainBGMapText,  x*4+16, y+1, "%.2d\xd4", flowers);
						map_printf(mainBGMapText,  x*4,    y+7, "%.2d\xd6", weeds);	
						map_printf(mainBGMapText,  x*4+16, y+7, "%.2d\xc2", items);

						bool unacceptable = weeds > 2 || items > 2;

						bool perfect = (trees >= 12 && trees <= 15 && flowers >= 3)
						            || (trees < 12 && flowers >= (3 + (12-trees)*2))
						            || (trees > 15 && flowers >= (3 + (trees-15)*2));

						bool acceptable = (trees >= 10 && trees <= 17)
						            || (trees <= 9 && flowers >= 3 + (9-trees)*2)
						            || (trees >= 18 && flowers >= 3 + (18-trees)*2);
						if(unacceptable) {
							map_print(mainBGMapText,  x*8, y+13, "Bad");
							num_unacceptable++;
						} else if(perfect) {
							map_print(mainBGMapText,  x*8, y+13, "Perfect");
							num_perfect++;
						} else if(acceptable) {
							map_print(mainBGMapText,  x*8, y+13, "Good");
							num_ok++;
						} else {
							map_print(mainBGMapText,  x*8, y+13, "Meh");
						}
					}
				}

				map_printf(mainBGMapText,  0, 18, "Perfect: %2d, Good: %2d", num_perfect, num_ok);
				map_printf(mainBGMapText,  0, 19, "    Bad: %2d, Meh:  %2d", num_unacceptable, 16 - num_perfect - num_ok - num_unacceptable);

				if(num_unacceptable)
					map_print(mainBGMapText, 0, 20, "Need to fix weeds or stray items");
				else if((num_ok + num_perfect) < 16)
					map_print(mainBGMapText, 0, 20, "Need all acres good or perfect");
				else if(num_perfect < 8)
					map_print(mainBGMapText, 0, 20, "Need 8 perfect acres");
				else
					map_print(mainBGMapText, 0, 20, "Seems to be perfect!");

				map_print(mainBGMapText,   0, 22, "Push start to exit...");
				wait_for_start();
				break;
			}
		}
	}
}
