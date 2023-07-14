/* Maze */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <png.h>
#include "libattopng.h"

#define DIR_NORTH 0
#define DIR_EAST 1
#define DIR_SOUTH 2
#define DIR_WEST 3

#define OUTOFBOUNDS 255
#define PATH_UNTROD 0
#define PATH_NORTH 1
#define PATH_EAST 2
#define PATH_SOUTH 4
#define PATH_WEST 8

#define MAP_CELL_TYPE uint8_t
#define DIRECTION uint8_t

#define RGBA(r, g, b, a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))

/* Structures etc... */
typedef struct coords {
  int x;
  int y;
} coords;

typedef struct crawler {
  coords position;
  int attempt;
  DIRECTION directions[5];
} crawler;

const char *gfx[] = {
    " ", "│", "─", "└", "│", "│", "┌", "├",
    "─", "┘", "─", "┴", "┐", "┤", "┬", "┼",
};

/* Function Prototypes  */
void make_maze();
void crawler_init(crawler *crawler);
void crawler_randomize_directions(crawler *crawler);
void crawler_info(crawler *crawler);
void crawler_look_to(coords *from, coords *to, uint8_t dir);
int crawler_in_bounds(coords *coords);
MAP_CELL_TYPE cell_is(coords *coords);
MAP_CELL_TYPE *cell(coords *coords);
MAP_CELL_TYPE *cell_xy(int x, int y);
void display_map();
char dir_to_str(uint8_t dir);
void dir_swap(DIRECTION *a, DIRECTION *b);
void usage();
void png_map();

/* Global Variables */
int width = 50;
int height = 50;
MAP_CELL_TYPE *map;
uint8_t PATHS[] = {PATH_NORTH, PATH_EAST, PATH_SOUTH, PATH_WEST};

const DIRECTION opposite[] = {DIR_SOUTH, DIR_WEST, DIR_NORTH, DIR_EAST};
const coords offsets[4] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
const char *DIR_STR = "NESW";
const char *prog_name;

int main(int argc, char *argv[]) {
  int c;
  int png = 0;
  int display = 0;

  prog_name = argv[0];

  while ((c = getopt(argc, argv, "pdw:h:")) != -1) {
    switch (c) {
    case 'w':
      width = atoi(optarg);
      break;
    case 'h':
      height = atoi(optarg);
      break;
    case 'd':
      display = 1;
      break;
    case 'p':
      png = 1;
      break;
    case '?':
      usage();
      break;
    }
  }

  if (width <= 10 || height <= 10 ) usage();

  map = calloc(width * height, sizeof(uint8_t));
  if (map == NULL) {
    fprintf(stderr, "Out of memory\n");
    exit(1);
  }

  srand(time(NULL));

  make_maze();
  if (display) display_map();
  if (png) png_map();
  return 0;
}

void make_maze() {
  uint8_t movement_stack[width * height];
  int stack_pointer = 0;

  DIRECTION try_direction;
  coords look_to;

  crawler crawler;
  crawler_init(&crawler);

  while (1) {
    crawler_randomize_directions(&crawler);
    while (crawler.attempt < 4) {
      try_direction = crawler.directions[crawler.attempt];
      crawler_look_to(&crawler.position, &look_to, try_direction);
      if (crawler_in_bounds(&look_to) && cell_is(&look_to) == PATH_UNTROD) {
        *cell(&crawler.position) |= PATHS[try_direction];
        *cell(&look_to) |= PATHS[opposite[try_direction]];
        crawler.position = look_to;
        movement_stack[stack_pointer++] = try_direction;
        break;
      }
      crawler.attempt++;
    }
    if (crawler.attempt == 4) {
      uint8_t step_back = opposite[movement_stack[--stack_pointer]];
      if (stack_pointer < 0) break;
      crawler_look_to(&crawler.position, &crawler.position, step_back);
    }
  }
}

MAP_CELL_TYPE cell_is(coords *coords) { return *cell(coords); }
MAP_CELL_TYPE *cell(coords *coords) { return cell_xy(coords->x, coords->y); }
MAP_CELL_TYPE *cell_xy(int x, int y) { return &map[x + y * width]; }

void crawler_look_to(coords *from, coords *to, uint8_t dir) {
  to->x = from->x + offsets[dir].x;
  to->y = from->y + offsets[dir].y;
}

char dir_to_str(DIRECTION dir) {
  if (dir < 0 || dir > 4) return '?';
  return DIR_STR[dir];
}

int crawler_in_bounds(coords *coords) {
  if (coords->x < 0 || coords->x >= width || coords->y < 0 ||
      coords->y >= height) {
    return 0;
  }
  return 1;
}

void crawler_init(crawler *crawler) {
  crawler->position.x = (rand() % width);
  crawler->position.y = (rand() % height);
  crawler->attempt = 0;

  crawler->directions[0] = 0;
  crawler->directions[1] = 1;
  crawler->directions[2] = 2;
  crawler->directions[3] = 3;
  crawler->directions[4] = 9;

  crawler_randomize_directions(crawler);
}

void crawler_randomize_directions(crawler *crawler) {
  crawler->attempt = 0;
  uint8_t tmp;
  int swap;

  dir_swap(&crawler->directions[0], &crawler->directions[rand() % 4]);
  dir_swap(&crawler->directions[1], &crawler->directions[rand() % 4]);
  dir_swap(&crawler->directions[2], &crawler->directions[rand() % 4]);
  dir_swap(&crawler->directions[3], &crawler->directions[rand() % 4]);
}

void dir_swap(DIRECTION *a, DIRECTION *b) {
  DIRECTION tmp = *a;
  *a = *b;
  *b = tmp;
}

void crawler_info(crawler *crawler) {
  fprintf(stderr, "Crawler at (%d, %d) Attempt: %d Dirs:", crawler->position.x,
          crawler->position.y, crawler->attempt);
  for (int n = 0; n < 5; n++) {
    fprintf(stderr, " %d", crawler->directions[n]);
  }
  fprintf(stderr, "\n");
}

void display_map() {
  for (int y = height - 1; y >= 0; y--) {
    for (int x = 0; x < width; x++) {
      printf("%s", gfx[*cell_xy(x, y)]);
    }
    printf("\n");
  }
}

void png_map() {
  int px, py;
  MAP_CELL_TYPE cell;

  libattopng_t* png = libattopng_new(width * 4, height *4, PNG_PALETTE);
  uint32_t palette[] = {
    RGBA(0, 0, 0, 255),
    RGBA(255, 255, 255, 255),
    RGBA(255, 0, 0, 255),
  };
  libattopng_set_palette(png, palette, 3);

  for (int y = height - 1; y >= 0; y--) {
    for (int x = 0; x < width; x++) {
      px = x*4;
      py = (height-1-y)*4;
      cell = *cell_xy(x, y);
      libattopng_set_pixel(png, px, py, 1);
      libattopng_set_pixel(png, px+1, py, 1);
      libattopng_set_pixel(png, px+2, py, 1);
      libattopng_set_pixel(png, px+3, py, 1);
      libattopng_set_pixel(png, px, py+1, 1);
      libattopng_set_pixel(png, px+1, py+1, 0);
      libattopng_set_pixel(png, px+2, py+1, 0);
      libattopng_set_pixel(png, px+3, py+1, 1);
      libattopng_set_pixel(png, px, py+2, 1);
      libattopng_set_pixel(png, px+1, py+2, 0);
      libattopng_set_pixel(png, px+2, py+2, 0);
      libattopng_set_pixel(png, px+3, py+2, 1);
      libattopng_set_pixel(png, px, py+3, 1);
      libattopng_set_pixel(png, px+1, py+3, 1);
      libattopng_set_pixel(png, px+2, py+3, 1);
      libattopng_set_pixel(png, px+3, py+3, 1);
      if (cell & PATH_NORTH) {
        libattopng_set_pixel(png, px+1, py, 0);
        libattopng_set_pixel(png, px+2, py, 0);
      }
      if (cell & PATH_EAST) {
        libattopng_set_pixel(png, px+3, py+1, 0);
        libattopng_set_pixel(png, px+3, py+2, 0);
      }
      if (cell & PATH_SOUTH) {
        libattopng_set_pixel(png, px+1, py+3, 0);
        libattopng_set_pixel(png, px+2, py+3, 0);
      }
      if (cell & PATH_WEST) {
        libattopng_set_pixel(png, px, py+1, 0);
        libattopng_set_pixel(png, px, py+2, 0);
      }
    }
  }

  libattopng_set_pixel(png,0,0,1);
  libattopng_save(png, "maze.png");
  libattopng_destroy(png);
  fprintf(stderr, "%i\n", *cell_xy(0,height-1));
}

void usage() {
  fprintf(stderr, "Usage: %s [-w <width>] [-h <height>]\n\
      -p create png\n\
      -d display map\n\
      width and height must be at least 10 (default 50)\n",
          prog_name);
  exit(1);
}
