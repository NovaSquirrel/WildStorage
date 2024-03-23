#include <nds.h>
#include <stdarg.h>

extern char filename[200];
extern char full_file_path[256];
extern char title_buffer[32];
extern char text_conversion_buffer[40];
extern int current_player;
extern int player_offset;
extern u8 savefile[256 * 1024];
extern const char *acww_folder_prefix;

extern u16 *mainBGMap;
extern u16 *mainBGMap2;
extern u16 *subBGMap;
extern u16 *subBGMap2;

void set_savefile_u16(int index, u16 value);
u16 get_savefile_u16(int index);
const char *player_name(int which);
const char *town_name();
const char *text_from_save(int index, int length);

int confirm_choice(const char *prompt);
int popup_notice(const char *prompt);
int popup_noticef(const char *fmt, ...);
int popup_noticef_on_screen(u16 *map, const char *fmt, ...);
int choose_file();
void clear_screen(u16 *screen);
void map_print(u16 *map, int x, int y, const char *text);
void map_printf(u16 *map, int x, int y, const char *fmt, ...);
void map_box(u16 *map, int x, int y, int w, int h);
void map_put(u16 *map, int x, int y, char c);
int choose_from_list(const char *prompt, const char **choices, int choice_count, int current_choice);
int choose_from_list_on_screen(u16 *map, const char *prompt, const char **choices, int choice_count, int current_choice);
int choose_item_from_all_on_screen(u16 *map, const char *prompt, u16 initial_item);

const char *name_for_item(unsigned short item_id);
unsigned short icon_for_item(unsigned short item_id);
#define AREA_NOT_AVAILABLE 0xf030
#define EMPTY_ITEM 0xfff1
#define INVALID_ITEM_SLOT 0xffff

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
};

#define TOWN_SAVE_SIZE 0x15FE0
#define PER_PLAYER_OFFSET 8844

#define EXTRA_STORAGE_SIZE 1500
