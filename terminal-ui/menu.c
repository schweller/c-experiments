/* menu.c - A simple terminal UI menu system inspired by Angband */
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MENU_ITEMS 10
#define TITLE_Y 2
#define MENU_START_Y 5

typedef struct {
    char *text;
    char key;       
    int enabled;
} MenuItem;

typedef struct {
    char *title;
    MenuItem items[MAX_MENU_ITEMS];
    int item_count;
    int selected;
} Menu;

void init_terminal(void);
void cleanup_terminal(void);
void draw_menu(Menu *menu);
void draw_border(int height, int width);
int handle_input(Menu *menu);
Menu create_main_menu(void);
Menu create_settings_menu(void);
void free_menu(Menu *menu);

void init_terminal(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);  /* Highlighted item */
        init_pair(2, COLOR_CYAN, COLOR_BLACK);    /* Title */
        init_pair(3, COLOR_WHITE, COLOR_BLACK);   /* Normal text */
    }
}

void cleanup_terminal(void) {
    endwin();
}

void draw_border(int height, int width) {
    int x, y;
    
    /* Corners */
    mvaddch(0, 0, '+');
    mvaddch(0, width - 1, '+');
    mvaddch(height - 1, 0, '+');
    mvaddch(height - 1, width - 1, '+');
    
    /* Horizontal lines */
    for (x = 1; x < width - 1; x++) {
        mvaddch(0, x, '-');
        mvaddch(height - 1, x, '-');
    }
    
    /* Vertical lines */
    for (y = 1; y < height - 1; y++) {
        mvaddch(y, 0, '|');
        mvaddch(y, width - 1, '|');
    }
}

/* Draw the menu - the heart of our UI
 * 
 * Best practices demonstrated:
 * - Clear screen before redrawing (prevents artifacts)
 * - Center text for professional look
 * - Use attributes (A_REVERSE) for highlighting
 * - Consistent spacing and alignment
 */
void draw_menu(Menu *menu) {
    int i;
    int max_y, max_x;
    int start_x;
    
    getmaxyx(stdscr, max_y, max_x);
    
    clear();
    
    /* Draw border for that classic roguelike feel */
    draw_border(max_y, max_x);
    
    /* Draw title - centered and colored */
    start_x = (max_x - strlen(menu->title)) / 2;
    if (has_colors()) attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(TITLE_Y, start_x, "%s", menu->title);
    if (has_colors()) attroff(COLOR_PAIR(2) | A_BOLD);
    
    /* Draw menu items */
    for (i = 0; i < menu->item_count; i++) {
        int y_pos = MENU_START_Y + i * 2;  /* 2-line spacing */
        
        if (menu->items[i].text == NULL) continue;
        
        start_x = (max_x - strlen(menu->items[i].text) - 6) / 2;
        
        if (i == menu->selected && menu->items[i].enabled) {
            attron(A_REVERSE);
            if (has_colors()) attron(COLOR_PAIR(1));
        }
        
        mvprintw(y_pos, start_x, " [%c] %s", 
                 menu->items[i].key, 
                 menu->items[i].text);
        
        if (!menu->items[i].enabled) {
            printw(" (disabled)");
        }
        
        if (i == menu->selected && menu->items[i].enabled) {
            attroff(A_REVERSE);
            if (has_colors()) attroff(COLOR_PAIR(1));
        }
    }
    
    mvprintw(max_y - 3, 2, "Arrow keys to navigate, Enter to select, Q to quit");
    
    refresh();
}

int handle_input(Menu *menu) {
    int ch = getch();
    int i;
    
    switch (ch) {
        case KEY_UP:
            /* Move up, wrap around to bottom */
            do {
                menu->selected--;
                if (menu->selected < 0) 
                    menu->selected = menu->item_count - 1;
            } while (!menu->items[menu->selected].enabled);
            break;
            
        case KEY_DOWN:
            /* Move down, wrap around to top */
            do {
                menu->selected++;
                if (menu->selected >= menu->item_count) 
                    menu->selected = 0;
            } while (!menu->items[menu->selected].enabled);
            break;
            
        case '\n':  /* Enter key */
        case '\r':
            if (menu->items[menu->selected].enabled)
                return 1;  /* Item selected */
            break;
            
        case 'q':
        case 'Q':
        case 27:  /* ESC key */
            return -1;  /* Quit/back */
            
        default:
            /* Check if user pressed a shortcut key */
            for (i = 0; i < menu->item_count; i++) {
                if ((ch == menu->items[i].key || 
                     ch == menu->items[i].key - 32) &&  /* Handle uppercase */
                    menu->items[i].enabled) {
                    menu->selected = i;
                    return 1;
                }
            }
            break;
    }
    
    return 0;  /* Continue in menu */
}

Menu create_main_menu(void) {
    Menu menu;
    menu.title = "MAIN MENU";
    menu.selected = 0;
    menu.item_count = 6;
    
    menu.items[0] = (MenuItem){"New Game", 'n', 1};
    menu.items[1] = (MenuItem){"Load Game", 'l', 0};
    menu.items[2] = (MenuItem){"Settings", 's', 1};
    menu.items[3] = (MenuItem){"High Scores", 'h', 1};
    menu.items[4] = (MenuItem){"Help", '?', 1};
    menu.items[5] = (MenuItem){"Quit", 'q', 1};
    
    return menu;
}

Menu create_settings_menu(void) {
    Menu menu;
    menu.title = "SETTINGS";
    menu.selected = 0;
    menu.item_count = 4;
    
    menu.items[0] = (MenuItem){"Sound: ON", 's', 1};
    menu.items[1] = (MenuItem){"Difficulty: Normal", 'd', 1};
    menu.items[2] = (MenuItem){"Key Bindings", 'k', 1};
    menu.items[3] = (MenuItem){"Back", 'b', 1};
    
    return menu;
}

void free_menu(Menu *menu) {}

int main(void) {
    int running = 1;
    int in_main_menu = 1;
    Menu main_menu, settings_menu;
    Menu *current_menu;
    
    /* Initialize everything */
    init_terminal();
    main_menu = create_main_menu();
    settings_menu = create_settings_menu();
    current_menu = &main_menu;
    
    /* Main loop - runs until user quits */
    while (running) {
        /* Draw current menu */
        draw_menu(current_menu);
        
        /* Handle input and get result */
        int result = handle_input(current_menu);
        
        if (result == -1) {
            if (in_main_menu) {
                running = 0;  /* Quit game */
            } else {
                current_menu = &main_menu;
                in_main_menu = 1;
            }
        } else if (result == 1) {
            int selected = current_menu->selected;
            
            if (current_menu == &main_menu) {
                if (selected == 2) {  /* Settings */
                    current_menu = &settings_menu;
                    in_main_menu = 0;
                } else if (selected == 5) {  /* Quit */
                    running = 0;
                } else {
                    clear();
                    mvprintw(10, 10, "Feature '%s' not implemented yet!", 
                             current_menu->items[selected].text);
                    mvprintw(12, 10, "Press any key to continue...");
                    refresh();
                    getch();
                }
            } else if (current_menu == &settings_menu) {
                if (selected == 3) {  /* Back */
                    current_menu = &main_menu;
                    in_main_menu = 1;
                }
            }
        }
    }
    
    free_menu(&main_menu);
    free_menu(&settings_menu);
    cleanup_terminal();
    
    return 0;
}