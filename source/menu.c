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
#include <dirent.h>
#include <nds.h>
#include "wild.h"

const char *are_you_sure_options[] = {"No", "Yes!"};
const char *ok_options[] = {"OK"};

void clear_screen(u16 *screen) {
	dmaFillHalfWords(' ', screen, 32*32*2);
}

void map_rectfill(u16 *map, int x, int y, int w, int h, u16 c) {
	for(int i=0; i<h; i++)
		for(int j=0; j<w; j++)
			map[(y+i)*32+x+j] = c;
}

void map_put(u16 *map, int x, int y, u16 c) {
	map[y * 32 + x] = c;
}

void map_print(u16 *map, int x, int y, const char *text) {
	int index = y * 32 + x;
	while(*text) {
		map[index++] = *text;
		text++;
	}
}

void map_printf(u16 *map, int x, int y, const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	char buffer[128];
	vsprintf(buffer, fmt, argp);
	map_print(map, x, y, buffer);
	va_end(argp);
}

void map_box(u16 *map, int x, int y, int w, int h) {
	map_put(map, x,     y, TILE_BORDER_DR);
	map_put(map, x+w-1, y, TILE_BORDER_DL);
	map_put(map, x,     y+h-1, TILE_BORDER_UR);
	map_put(map, x+w-1, y+h-1, TILE_BORDER_UL);

	for(int i=1; i<(w-1); i++) {
		map_put(map, x+i, y,     TILE_BORDER_HORIZ);
		map_put(map, x+i, y+h-1, TILE_BORDER_HORIZ);
	}
	for(int i=1; i<(h-1); i++) {
		map_put(map, x,     y+i, TILE_BORDER_VERT);
		map_put(map, x+w-1, y+i, TILE_BORDER_VERT);
	}
}

int choose_from_list_on_screen(u16 *map, const char *prompt, const char **choices, int choice_count, int current_choice) {
	#define OPTIONS_PER_PAGE 20
	#define OPTIONS_START_Y 2
	clear_screen(map);
	bool redraw_page = 1;
	int page_number;

	if(current_choice >= choice_count)
		current_choice = choice_count - 1;

	int max_page = choice_count / OPTIONS_PER_PAGE + ((choice_count % OPTIONS_PER_PAGE) != 0);

	while(1) {
		if(redraw_page) {
			page_number = current_choice / OPTIONS_PER_PAGE;

			clear_screen(map);
			map_print(map,  1, 1, prompt);
			if(max_page > 1) {
				map_printf(map, 25, 23, "%2d/%2d", page_number+1, max_page);
			}

			for(int i=0; i<OPTIONS_PER_PAGE; i++) {
				if((page_number * OPTIONS_PER_PAGE + i) >= choice_count)
					break;
				map_print(map, 2, i+OPTIONS_START_Y, choices[page_number * OPTIONS_PER_PAGE + i]);
			}

			map_print(map,  1, (current_choice % OPTIONS_PER_PAGE) + OPTIONS_START_Y, "\xf0");

			redraw_page = 0;
		}

		wait_vblank_and_animate();
		scanKeys();

		// Respond to button presses

		uint32_t keys_down = keysDown();
		uint32_t keys_repeat = 	keysDownRepeat();
		int old_choice = current_choice;

		if(keys_repeat & KEY_UP) {
			current_choice--;
			if(current_choice < 0)
				current_choice = choice_count - 1;
		}
		if(keys_repeat & KEY_DOWN) {
			current_choice++;
			if(current_choice >= choice_count)
				current_choice = 0;
		}
		if(keys_repeat & KEY_LEFT) {
			current_choice -= 5;
			if(current_choice < 0)
				current_choice = 0;
		}
		if(keys_repeat & KEY_RIGHT) {
			current_choice += 5;
			if(current_choice >= choice_count)
				current_choice = choice_count - 1;
		}

		if(current_choice != old_choice) {
			if(page_number != (current_choice / OPTIONS_PER_PAGE)) {
				redraw_page = 1;
			} else {
				map_print(map, 1, (old_choice % OPTIONS_PER_PAGE) + OPTIONS_START_Y, " ");
				map_print(map, 1, (current_choice % OPTIONS_PER_PAGE) + OPTIONS_START_Y, "\xf0");
			}
		}

		if(keys_down & KEY_A)
			return current_choice;
		if(keys_down & KEY_B)
			return -1;
		if(keys_down & KEY_START)
			return -2;
	}
}

int choose_from_list(const char *prompt, const char **choices, int choice_count, int current_choice) {
	return choose_from_list_on_screen(mainBGMapText, prompt, choices, choice_count, current_choice);
}

int confirm_choice(const char *prompt) {
	return choose_from_list(prompt, are_you_sure_options, 2, 0);
}

int popup_notice(const char *prompt) {
	return choose_from_list(prompt, ok_options, 1, 0);
}

int popup_noticef(const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	char buffer[128];
	vsprintf(buffer, fmt, argp);
	int code = choose_from_list(buffer, ok_options, 1, 0);
	va_end(argp);
	return code;
}

int popup_noticef_on_screen(u16 *map, const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	char buffer[128];
	vsprintf(buffer, fmt, argp);
	int code = choose_from_list_on_screen(map, buffer, ok_options, 1, 0);
	va_end(argp);
	return code;
}

int choose_file() {
	#define MAX_FILES_IN_FOLDER 100
	char *directory_names[MAX_FILES_IN_FOLDER];
	int directory_index[MAX_FILES_IN_FOLDER];
	char name_buffer[28];
	int return_code = 0;

	while(!return_code) {
		DIR *dir = opendir(".");
		if(dir == NULL) {
			return -2;
		}

		// Reset the list
		int item_count = 0;
		memset(directory_names, 0, sizeof(directory_names));

		// Build up the list of items
		while(1) {
			// Get the index before moving onto the next entry
			directory_index[item_count] = telldir(dir);

			struct dirent *cur = readdir(dir);
			if(cur == NULL)
				break;
			if(strlen(cur->d_name) == 0)
				break;
			if(!strcmp(cur->d_name, "."))
				continue;

			strlcpy(name_buffer, cur->d_name, sizeof(name_buffer));
			if(cur->d_type == DT_DIR) {
				strlcat(name_buffer, "/", sizeof(name_buffer));
			} else {
				int length = strlen(cur->d_name);
				if(( // Only show filenames ending in .SAV with any capitalization
					(cur->d_name[length-1] != 'v' && cur->d_name[length-1] != 'V') ||
					(cur->d_name[length-2] != 'a' && cur->d_name[length-2] != 'A') ||
					(cur->d_name[length-3] != 's' && cur->d_name[length-3] != 'S') ||
					(cur->d_name[length-4] != '.')))
					continue;
			}

			// Record the entry in the list
			directory_names[item_count] = strdup(name_buffer);

			item_count++;
			if(item_count >= MAX_FILES_IN_FOLDER)
				break;
		}

		// Present the choices to the user
        char *cwd = getcwd(NULL, 0);
		strlcpy(name_buffer, cwd, sizeof(name_buffer));
        free(cwd);
		int result = choose_from_list(name_buffer, (const char**)directory_names, item_count, 0);

		// Clean up
		for(int i=0; i<item_count; i++) {
			free(directory_names[i]);
		}

		if(result >= 0) {
			rewinddir(dir); // libc bug workaround
			seekdir(dir, directory_index[result]);

			struct dirent *cur = readdir(dir);

			if(cur != NULL) {
				if(cur->d_type == DT_DIR) {
					chdir(cur->d_name);
				} else {
					strlcpy(filename, cur->d_name, sizeof(filename));
					return_code = 1;
				}
			}
		} else if(result == -1) { // B
			chdir("..");
		} else { // Start
			return_code = -1;
		}
		closedir(dir);
	}

	return return_code;
}
