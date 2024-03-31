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
#include <ctype.h>
#include "wild.h"

#include <my_font.h>

// ------------------------------------------------------------------------------------------------
// Variables
char text_input_buffer[200];

// ------------------------------------------------------------------------------------------------
// Functions

int ask_for_text_in_buffer(const char *prompt, char *buffer, int limit) {
	wait_vblank_and_animate();
	videoBgDisable(1); // Disable for one frame

	// The keyboard is hardcoded to use the first palette, which the UI normally uses, so put the UI palette elsewhere
	dmaCopy(my_fontPal, BG_PALETTE+32, sizeof(my_fontPal));
	clear_screen(mainBGMapText);
	ui_palette_offset = TILE_PALETTE(2);

	// Instructions
	map_print(mainBGMapText, 1, 1, prompt);

	wait_vblank_and_animate();
	keyboardInit(keyboardGetDefault(), 1, BgType_Text4bpp, BgSize_T_256x512, 6, 3, true, true);
	keyboardShow();

	// Variables
	int buffer_index = strlen(buffer);

	int return_value = 1;
	while(1) {
		wait_vblank_and_animate();
		scanKeys();

		map_printf(mainBGMapText, 1, 3, "%s\x1B ", buffer);

		int16_t c = keyboardUpdate();
		if (c != -1) {
			if (c == '\b') { // Backspace
				if(buffer_index) {
					buffer[--buffer_index] = 0;
				}
			} else if (c == '\n') {
				break;
			} else if (c >= 32) { // Visible text
				if(buffer_index < limit-1) {
					buffer[buffer_index++] = c;
					buffer[buffer_index] = 0;
				}
			}
		}

		uint32_t keys_down = keysDown();
		if (keys_down & KEY_B) {
			return_value = 0;
			break;
		}
		if (keys_down & KEY_START)
			break;
	}

	// Clean up
	wait_vblank_and_animate();
	keyboardHide();
	clear_screen(mainBGMapText);
	dmaFillHalfWords(0, bgGetGfxPtr(mainBG256), 64);

	// Put palettes back to how they were
	ui_palette_offset = TILE_PALETTE(0);
	dmaCopy(my_fontPal, BG_PALETTE, sizeof(my_fontPal));
	upload_pattern_palette();

	// Workaround for an issue where keyboardInit() seems to be displaying uninitialized data for a frame
	dmaFillHalfWords(0, (u16*)bgGetMapPtr(keyboardGetDefault()->background), 1024);

	wait_vblank_and_animate();
	bgShow(mainBGText);
	return return_value;
}

const char *ask_for_text(const char *prompt, const char *default_str, int limit) {
	if(default_str)
		strcpy(text_input_buffer, default_str);
	else
		text_input_buffer[0] = 0;
	if(limit == 0)
		limit = sizeof(text_input_buffer);
	if(ask_for_text_in_buffer(prompt, text_input_buffer, limit < sizeof(text_input_buffer) ? limit : sizeof(text_input_buffer)))
		return text_input_buffer;
	return NULL;
}


int ask_for_u16(const char *prompt, int initial) {
	if(initial >= 0) {
		sprintf(text_input_buffer, "%.4x", initial);
	} else {
		text_input_buffer[0] = 0;
	}

	while(1) {
		int result = ask_for_text_in_buffer(prompt, text_input_buffer, 5);
		if(!result)
			return -1;
		bool is_ok = true;
		for(int i=0; text_input_buffer[i]; i++) {
			if(!isxdigit(text_input_buffer[i])) {
				is_ok = false;
				break;
			}
		}
		if(is_ok)
			return strtol(text_input_buffer, NULL, 16);
	}
}
