#include <nds.h>
#include <stdarg.h>

extern char filename[200];
extern char full_file_path[256];
extern char title_buffer[32];
extern char text_conversion_buffer[64];
extern int current_player;
extern int player_offset;
extern u8 savefile[256 * 1024];
extern const char *acww_folder_prefix;
extern u16 ui_palette_offset;
extern bool no_savefile_mode;

extern int mainBGText, mainBG256, mainBGBehind, subBGText, subBG256, subBGBehind;
extern u16 *mainBGMapText;
extern u16 *mainBGMap256;
extern u16 *mainBGMapBehind;
extern u16 *subBGMapText;
extern u16 *subBGMap256;
extern u16 *subBGMapBehind;

void set_savefile_u16(int index, u16 value);
u16 get_savefile_u16(int index);
const char *player_name(int which);
const char *player_name_or_null(int which);
const char *town_name();
const char *text_from_save(int index, int length);
void fix_invalid_filename_chars(char *buffer);
const char *town_name_for_filename();
void ascii_str_to_acww(u8 *acww, const char *ascii, int size);

// Menus
int confirm_choice(const char *prompt);
int confirm_choice_on_screen(u16 *screen, const char *prompt);
int popup_notice(const char *prompt);
int popup_noticef(const char *fmt, ...);
int popup_noticef_on_screen(u16 *map, const char *fmt, ...);
#define ALL_FILES 0
#define SAVE_FILES 1
#define PATTERN_FILES 2
int choose_file(int file_type);
int choose_file_on_screen(u16 *map, int file_type);
int edit_item_menu(u16 *screen, u16 item, int include_sort);
int ask_for_text_in_buffer(const char *prompt, char *buffer, int limit);
const char *ask_for_text(const char *prompt, const char *default_str, int limit);
int ask_for_u16(const char *prompt, int initial);

void clear_screen(u16 *screen);
void clear_screen_256(u16 *map);
void map_print(u16 *map, int x, int y, const char *text);
void map_printf(u16 *map, int x, int y, const char *fmt, ...);
void map_box(u16 *map, int x, int y, int w, int h);
void map_put(u16 *map, int x, int y, u16 c);
void map_rectfill(u16 *map, int x, int y, int w, int h, u16 c);
int choose_from_list(const char *prompt, const char **choices, int choice_count, int current_choice);
int choose_from_list_on_screen(u16 *map, const char *prompt, const char **choices, int choice_count, int current_choice);
int choose_item_from_all_on_screen(u16 *map, const char *prompt, u16 initial_item);
void wait_for_start();
void wait_vblank_and_animate();
void upload_pattern_palette();
void set_default_video_mode();

extern const char *ok_options[];

const char *name_for_item(unsigned short item_id);
unsigned short icon_for_item(unsigned short item_id);

// Special tileset characters
enum {
	TILE_ARROW_ACTIVE   = 0xf0,
	TILE_ARROW_INACTIVE = 0xf1,
	TILE_BORDER_HORIZ   = 0xf2,
	TILE_BORDER_VERT    = 0xf3,
	TILE_BORDER_DR = 0xf4,
	TILE_BORDER_DL = 0xf5,
	TILE_BORDER_UR = 0xf6,
	TILE_BORDER_UL = 0xf7,
	TILE_BORDER_BOX = 0xf8,
	TILE_BUTTON_A = 0xe0,
	TILE_BUTTON_B = 0xe1,
	TILE_BUTTON_X = 0xe2,
	TILE_BUTTON_Y = 0xe3,
	TILE_BUTTON_L = 0xe4,
	TILE_BUTTON_R = 0xe5,
	TILE_VERTICAL_PICKER_L = 0xb0,
	TILE_VERTICAL_PICKER_R = 0xb1,
	TILE_VERTICAL_PICKER_L_INACTIVE = 0xb2,
	TILE_VERTICAL_PICKER_R_INACTIVE = 0xb3,
	TILE_VERTICAL_PICKER_L_BAR = 0xb4,
	TILE_VERTICAL_PICKER_R_BAR = 0xb5,
	TILE_VERTICAL_PICKER_L_BAR_INACTIVE = 0xb6,
	TILE_VERTICAL_PICKER_R_BAR_INACTIVE = 0xb7,
	TILE_SELECTION_BOX_TOP_LEFT = 0xe7,
	TILE_SELECTION_BOX_TOP      = 0xe8,
	TILE_SELECTION_BOX_LEFT     = 0xe9,
	TILE_BORDER_GRAY_CORNER = 0xf9,
	TILE_BORDER_GRAY_VERT   = 0xfa,
	TILE_BORDER_GRAY_HORIZ  = 0xfb,
};

enum {
	ICON_EMPTY     = ' ',
	ICON_X         = 0xc0,
	ICON_UNKNOWN   = 0xc1,
	ICON_ITEM      = 0xc2,
	ICON_FURNITURE = 0xc3,
	ICON_SHIRT     = 0xc4,
	ICON_HAT       = 0xc5,
	ICON_GLASSES   = 0xc6,
	ICON_BELLS     = 0xc7,
	ICON_TOOL      = 0xc8,
	ICON_MUSIC     = 0xc9,
	ICON_UMBRELLA  = 0xca,
	ICON_GYROID    = 0xcb,
	ICON_FOSSIL    = 0xcc,
	ICON_FOSSIL_ID = 0xcd,
	ICON_SHELL     = 0xce,
	ICON_TRASH     = 0xcf,
	ICON_FISH      = 0xd0,
	ICON_BUG       = 0xd1,
	ICON_BUILDING  = 0xd2,
	ICON_TREE      = 0xd3,
	ICON_FLOWER    = 0xd4,
	ICON_ROCK      = 0xd5,
	ICON_WEED      = 0xd6,
	ICON_FLOWERBAG = 0xd7,
	ICON_SAPLING   = 0xd8,
	ICON_EXPLOSIVE = 0xd9,
	ICON_FRUIT     = 0xda,
	ICON_PAPER     = 0xdb,
	ICON_ACORN     = 0xdc,
	ICON_TURNIP    = 0xdd,
	ICON_PITFALL   = 0xde,
	ICON_PRESENT   = 0xdf,
	ICON_PICTURE   = 0xec,
	ICON_MAIL      = 0xed,
	ICON_PATTERN   = 0xee,
	ICON_SPACE_USED = 0xef,
};
#define TILE_BASE_PATTERN_TOOLS 0x100

#define SPRITE_MAIN_TILE_POINTER(num) (&SPRITE_GFX[16*num])
#define SPRITE_SUB_TILE_POINTER(num) (&SPRITE_GFX_SUB[16*num])
#define PATTERN_SHARED_COLOR_COUNT 168
#define PATTERN_SHARED_COLOR_STARTS_AT 32
extern const unsigned char  pattern_map_to_shared[];
extern const unsigned short pattern_shared_colors[] __attribute__((aligned(4)));

#define EXTRA_STORAGE_SIZE 1500
#define EXTRA_PATTERN_STORAGE_SIZE 1024

// Item code related
#define FURNITURE_FACING_DOWN  0
#define FURNITURE_FACING_RIGHT 1
#define FURNITURE_FACING_UP    2
#define FURNITURE_FACING_LEFT  3

#define AREA_NOT_AVAILABLE 0xf030
#define EMPTY_ITEM 0xfff1
#define INVALID_ITEM_SLOT 0xffff

// Savefile offsets
#define TOWN_SAVE_SIZE 0x15FE0
#define PER_PLAYER_OFFSET 8844
#define TOWN_ITEM_GRID_START 0x0C354
#define TOWN_ITEM_GRID_END   0x0E352
#define YOUR_HOUSE_START 0xE558

// ------------------------------------------------------------------------------------------------
// Structures

typedef struct acww_name {
  u16 id;
  u8 name[8];
} acww_name;

// Based on the notes in RAC as a starting point
// 552 byte structure
struct acww_pattern {
  u8 data[32][16]; // data[row][column], where each byte has the left pixel in the lower nybble and right pixel in the upper nybble

  struct acww_name author_town;
  struct acww_name author;

  u16 unknown; // Unknown,seems to be 0 for custom patterns, 1 for the premade you start out with, and a Wendell pattern had $021d?

  u8 pattern_name[16];

  u16 unknown2;
  // 0000 0000 pppp ????
  // |||| |||| |||| ++++- Values observed here: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  // |||| |||| ||||       Might be what gets saved from the three options you get after designing? But there's more than 3 values.
  // |||| |||| ||||       2 = "Formal, refined"
  // |||| |||| ||||       9 = "Basic, yet new!"
  // |||| |||| ||||       0 7 = "Didn't have one"
  // |||| |||| ||||       Needs more research!
  // |||| |||| ++++------ Palette used (0-15)
  // ++++-++++----------- I've only seen zero here so far
};
