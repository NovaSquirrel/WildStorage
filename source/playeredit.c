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
const char *edit_player_options[] = {"Emotions", "Hair style", "Hair color", "Face", "Gender", "Tan", "Birth month", "Birth day"};

const char *face_options[] = {
	"M Brown Eyes + Lashes",
	"M Black Eyes + Brows",
	"M Blue Eyes + Eyelids",
	"M Green+Small Pupils+Brows",
	"M Brown Eyes",
	"M Arched Black Eyes",
	"M Blue Eyes + Rosy Cheeks",
	"M Blue Eyes + Small Pupils",
	"F Black Anime Eyes w/ Lash",
	"F Black Squinted Eyes + Lash",
	"F Blue Eyes + Rosey Cheeks",
	"F Blue Eyes + Eyelids",
	"F Green Oval Eyes w/ Lashes",
	"F Brown Arch+Lash",
	"F Blue Eyes"
	"F Green Circle Eyes+Lashes"
};

const char *hair_style_options[] = {
	"Male buzz cut", "Male cowlick", "Male long bangs (cover eye)", "Male spikes", "Male middle part", "Male inward spikes", "Male bowl cut", "Male spiked bangs",
	"Female rightward bangs", "Female mop top", "Female pig tails", "Female triangle spikes", "Female pony tail", "Female pony stub", "Female ears short", "Female curls"
};
const char *hair_color_options[] = {"Dark brown", "Light brown", "Orange", "Blue", "Yellow", "Green", "Pink", "White"};
const char *tan_options[] = {"No tan", "Light tan", "Tan", "Dark tan"};
const char *gender_options[] = {"Male", "Female"};
const char *month_options[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const char *day_options[] = {"1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th", "10th", "11th", "12th", "13th", "14th", "15th", "16th", "17th", "18th", "19th", "20th", "21st", "22nd", "23rd", "24th", "25th", "26th", "27th", "28th", "29th", "30th", "31st"};
const char *emotions_options[] = {
	"(nothing)",
	" 1 anger",
	" 2 shock",
	" 3 laughter",
	" 4 surprise",
	" 5 rage",
	" 6 distress",
	" 7 fear",
	" 8 sorrow",
	" 9 blankness",
	"10 joy",
	"11 confusion",
	"12 inspiration",
	"13 happiness",
	"14 thought",
	"15 sadness",
	"16 disappointment",
	"17 scheming",
	"18 fatigue",
	"19 love",
	"20 contentment",
	"21 irritation",
	"22 discontent",
	"23 shyness",
	"24 disbelief",
	"25 agreement",
	"26 approval",
	"27 acceptance",
	"28 exasperation",
	"29 realization",
	"30 (machine gun left-to-right)",
	"31 (stabbing motion right-arm)",
	"32 (machine gun right-to-left)",
	"33 (throws hands in air)",
	"34 (strange posture)",
	"35 (slight shift + plunk)",
	"36 (like surprise)",
	"37 (like confusion)",
	"38 (flap + fairy dust)",
	"39 (fatigue-like)",
	"40 (lay down + yawn)",
	"41 (get out of hole)",
	"42 (fall in a hole)",
	"43 (blankness in-a-hole)",
	"44 (realization in-a-hole)",
	"45 (get out of hole #2)",
	"46 (fall in hole)",
	"47 (fall in hole and squirm)",
	"48 (sitting on something)",
	"49 (sit with feet fly in air)",
	"50 (smell underarm?)",
	"51 (levitation)",
	"52 (floating)",
	"53 (end floating, SFX)",
	"54 (more floating bits)",
	"55 (more floating bits)",
	"56 (more floating bits)",
	"57 (quick surprise, floating)",
	"58 (quick discontent-like)",
	"59 (quick distress-like)"
};

// ------------------------------------------------------------------------------------------------
// Variables

char player_name_1[20];
char player_name_2[20];
char player_name_3[20];
char player_name_4[20];
const char *player_name_selection[] = {player_name_1, player_name_2, player_name_3, player_name_4};
int player_offset;

// ------------------------------------------------------------------------------------------------
// Functions

const char *player_name(int which) {
	const char *name = text_from_save(0x228E + PER_PLAYER_OFFSET*which, 8);
	if(!name[0]) {
		switch(which) {
			case 0:	name = "Player 1"; break;
			case 1:	name = "Player 2"; break;
			case 2:	name = "Player 3"; break;
			case 3:	name = "Player 4"; break;
		}
	}
	return name;
}

void fill_in_player_name_options() {
	strcpy(player_name_1, player_name(0));
	strcpy(player_name_2, player_name(1));
	strcpy(player_name_3, player_name(2));
	strcpy(player_name_4, player_name(3));
}

void edit_emotions() {
	const char *emotions[4];
	int emotion_slot = 0;

	while(1) {
		emotions[0] = emotions_options[(savefile[player_offset + 0x021F8] < 60) ? savefile[player_offset + 0x021F8] : 0];
		emotions[1] = emotions_options[(savefile[player_offset + 0x021F9] < 60) ? savefile[player_offset + 0x021F9] : 0];
		emotions[2] = emotions_options[(savefile[player_offset + 0x021FA] < 60) ? savefile[player_offset + 0x021FA] : 0];
		emotions[3] = emotions_options[(savefile[player_offset + 0x021FB] < 60) ? savefile[player_offset + 0x021FB] : 0];

		int result = choose_from_list("Which emotion slot?", emotions, 4, emotion_slot);
		if(result < 0)
			return;
		emotion_slot = result;

		result = choose_from_list("Which emotion?", emotions_options, 60, savefile[player_offset + 0x021F8 + emotion_slot]);
		if(result >= 0)
			savefile[player_offset + 0x021F8 + emotion_slot] = result;
	}
}

void menu_player_edit() {
	static int which_player = 0;

	fill_in_player_name_options();

	while(1) {
		int new_player = choose_from_list("Which player?", player_name_selection, 4, which_player);
		if(new_player < 0)
			return;
		which_player = new_player;
		player_offset = PER_PLAYER_OFFSET * which_player;

		int edit_player_option = 0;
		while(1) {
			int result = choose_from_list("Edit what?", edit_player_options, 8, edit_player_option);
			if(result < 0)
				break;
			edit_player_option = result;
			switch(result) {
				case 0:
					edit_emotions();
					break;
				case 1:
					result = choose_from_list("Hair style?", hair_style_options, 16, savefile[player_offset + 0x02248] >> 4);
					if(result >= 0)
						savefile[player_offset + 0x02248] = (savefile[player_offset + 0x02248] & 0x0F) | (result << 4);
					break;
				case 2:
					result = choose_from_list("Hair color?", hair_color_options, 8, savefile[player_offset + 0x02249] & 7);
					if(result >= 0)
						savefile[player_offset + 0x02249] = (savefile[player_offset + 0x02249] & 0xF8) | result;
					break;
				case 3:
					result = choose_from_list("Face?", face_options, 16, savefile[player_offset + 0x02248] & 15);
					if(result >= 0)
						savefile[player_offset + 0x02248] = (savefile[player_offset + 0x02248] & 0xF0) | result;
					break;
				case 4:
					result = choose_from_list("Gender?", gender_options, 2, (savefile[player_offset + 0x02296] & 15));
					if(result >= 0)
						savefile[player_offset + 0x02296] = (savefile[player_offset + 0x02296] & 0xF0) | result;
					break;
				case 5:
					result = choose_from_list("Tan?", tan_options, 4, savefile[player_offset + 0x02249] >> 4);
					if(result >= 0)
						savefile[player_offset + 0x02249] = (savefile[player_offset + 0x02249] & 0xCF) | (result << 4);
					break;
				case 6:
					result = choose_from_list("Birth month?", month_options, 12, savefile[player_offset + 0x02225]-1);
					if(result >= 0)
						savefile[player_offset + 0x02225] = result+1;
					break;
				case 7:
					result = choose_from_list("Birth day?", day_options, 31, savefile[player_offset + 0x02224]-1);
					if(result >= 0)
						savefile[player_offset + 0x02224] = result+1;
					break;
			}
		}
	}
}
