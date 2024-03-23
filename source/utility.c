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
// Strings
const char *utility_options[] = {"No Resetti", "Remove weeds", "Revive all flowers", "Water all flowers", "Replenish fruit", "Un-bury items", "Remove stray items", "Sort closets", "Sort extra storage"};

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
		int option = choose_from_list("Utility features", utility_options, 9, which_option);
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
				for(int i=0x0C354; i<=0x0E352; i+=2) {
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
				for(int i=0x0C354; i<=0x0E352; i+=2) {
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
				for(int i=0x0C354; i<=0x0E352; i+=2) {
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
				for(int i=0x0C354; i<=0x0E352; i+=2) {
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
				for(int i=0x0C354; i<=0x0E352; i+=2) {
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
		}
	}
}
