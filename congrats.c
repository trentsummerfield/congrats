#include <ncurses.h>

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define GRAVITY 0.9
#define M_PI 3.1415826

static char *msg;
static int msg_len;
static bool print_msg;

static long unsigned frame = 0;

enum color {
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
};

struct particle {
    int old_row, old_col;
    float x, y;
    float dx, dy;
    float mass;
    char symbol;
    struct particle *next;
    enum color color;
} *particles = NULL;

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

        attron(COLOR_PAIR(WHITE) | A_BOLD);
        mvprintw(row, col, msg);
        attroff(COLOR_PAIR(WHITE) | A_BOLD);
    }
}

void draw_particles()
{
    int row, col, r, c;
    getmaxyx(stdscr, row, col);
    struct particle *p = particles;
    for (;p!= NULL; p= p->next) {
        if (p->x < 0 || p->x >= col) continue;
        if (p->y < 0 || p->y >= row) continue;
        r = row - p->y - 1;
        c = p->x;
        p->old_row = r;
        p->old_col = c;
        attron(COLOR_PAIR(p->color) | A_BOLD);
        mvaddch(r, c, p->symbol);
        attroff(COLOR_PAIR(p->color) | A_BOLD);
    }
}

void clear_particle(struct particle *p)
{
    mvprintw(p->old_row, p->old_col, " ");
}

void clear_screen()
{
    struct particle *p = particles;
    for (;p!= NULL; p= p->next) {
        clear_particle(p);
    }
}

void draw_frame()
{
    clear_screen();
    draw_particles();
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

void new_particle(float x, float y, float dx, float dy, char symbol, float mass, enum color color)
{
    struct particle *p = malloc(sizeof(*p));
    p->x = x;
    p->y = y;
    p->dx = dx;
    p->dy = dy;
    p->mass = mass;
    p->next = NULL;
    p->symbol = symbol;
    p->color = color;


    struct particle *n;
    if (particles == NULL) {
        particles = p;
    } else {
        n = particles;
        while (n->next != NULL)
            n = n->next;
        n->next = p;
    }
}

void new_particles_from(struct particle *p)
{
    int num = (rand() % 8) + 3;
    float step = 360.f / num;
    float angle = 0.f;
    float speed = ((float)rand() / RAND_MAX) * 5 + 2;
    for (int i = 0; i < num; i++) {
        float dx = speed * cos(angle * M_PI / 180.f);
        float dy = speed * sin(angle * M_PI / 180.f);
        new_particle(p->x, p->y, dx, dy, '*', 0.5f, p->color);
        angle += step;
    }
}

void spawn_rand_particle()
{
    int row, col;
    getmaxyx(stdscr, row, col);
    float dx = (((float)rand() / RAND_MAX) * 20) - 10;
    float dy = (((float)rand() / RAND_MAX) * 7) + 3;
    new_particle(col / 2.f, 0.f, dx, dy, 'O', 1.0f, rand() % 6);
}

void add_gravity(struct particle *p)
{
    p->dx *= p->mass;
    p->dy *= p->mass;
}

void move_particle(struct particle *p)
{
    p->x += p->dx;
    p->y += p->dy;
    p->mass *= 0.9;
}

void remove_particle(struct particle *p)
{
    struct particle *n = particles;
    if (n == p) {
        particles = particles->next;
    } else {
        while (n->next != p)
            n = n->next;
        n->next = p->next;
    }
    clear_particle(p);
    free(p);
}

void burst()
{
    struct particle *n, *p = particles;
    while (p != NULL) {
        n = p->next;
        if (p->symbol == 'O' && p->dy < 0.5) {
            new_particles_from(p);
            remove_particle(p);
        } else if (p->symbol == '*' && p->mass < 0.2) {
            p->symbol = '.';
            p->mass = 0.3;
        } else if (p->symbol == '.' && p->mass < 0.2) {
            remove_particle(p);
        }
        p = n;
    }
}

void step_simulation()
{
    struct particle *particle = particles;

    burst();

    for (;particle != NULL; particle = particle->next) {
        add_gravity(particle);
        move_particle(particle);
    }
}

void random_spawn()
{
    while ((float)rand() / RAND_MAX > 0.8)
        spawn_rand_particle();
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    initscr();
    start_color();
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);

    curs_set(0);
    timeout(75);

    while(true) {
        random_spawn();
        step_simulation();
        draw_frame();
        int ch = getch();
        if (ch != ERR) break;
        frame++;
    }
    endwin();

    return 0;
}
