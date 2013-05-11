#include <ncurses.h>

#include <stdbool.h>
#include <string.h>

static char *msg;
static int msg_len;
static bool print_msg;

static long unsigned frame = 0;

void draw_box(int row, int col, int height, int width)
{
    mvprintw(row, col, "+");
    for (int i = 0; i < width; i++) {
        printw("-");
    }
    printw("+");

    for (int j = 1; j < height + 1; j++) {
        mvprintw(row + j, col, "|");
        for (int i = 0; i < width; i++) {
            printw(" ");
        }
        printw("|");
    }

    mvprintw(row + height + 1, col, "+");
    for (int i = 0; i < width; i++) {
        printw("-");
    }
    printw("+");
}

void draw_message()
{
    if (print_msg) {
        int row, col;
        getmaxyx(stdscr, row, col);
        row = row / 2;
        col = (col - msg_len) / 2;
        draw_box(row - 2, col - 2, 3, msg_len + 2);

        attron(A_BOLD);
        mvprintw(row, col, msg);
        attroff(A_BOLD);
    }
}

void draw_frame()
{
    draw_message();
    refresh();
}

void parse_args(int argc, char *argv[])
{
    if (argc == 1) {
        print_msg = false;
    } else {
        print_msg = true;
        msg = argv[1];
        msg_len = strlen(msg);
    }
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    initscr();
    curs_set(0);
    timeout(500);

    while(true) {
        draw_frame();
        int ch = getch();
        if (ch != ERR) break;
        frame++;
    }
    endwin();

    return 0;
}
