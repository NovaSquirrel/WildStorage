#include <nds.h>
#include <stdarg.h>

extern char full_file_path[256];
extern char title_buffer[32];
extern char text_conversion_buffer[40];
extern int current_player;
extern int player_offset;
extern u8 savefile[256 * 1024];

extern u16 *mainBGMap;
extern u16 *mainBGMap2;
extern u16 *subBGMap;
extern u16 *subBGMap2;

int confirm_choice(const char *prompt);
int popup_notice(const char *prompt);
int choose_file();
void clear_screen(u16 *screen);
void map_print(u16 *map, int x, int y, const char *text);
void map_printf(u16 *map, int x, int y, const char *fmt, ...);
int choose_from_list(const char *prompt, const char **choices, int choice_count, int current_choice);

