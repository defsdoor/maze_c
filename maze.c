/* Maze */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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

const char * gfx[] = {
  " ",
  "│",
  "─",
  "└",
  "│",
  "│",
  "┌",
  "├",
  "─",
  "┘",
  "─",
  "┴",
  "┐",
  "┤",
  "┬",
  "┼",
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

/* Global Variables */
int width = 50;
int height = 50;
MAP_CELL_TYPE *map;
uint8_t PATHS[] = {PATH_NORTH, PATH_EAST, PATH_SOUTH, PATH_WEST};

const DIRECTION opposite[] = {DIR_SOUTH, DIR_WEST, DIR_NORTH, DIR_EAST};

int main(int argc, char *argv[]) {
  int c;

  while ((c = getopt(argc, argv, "w:h:")) != -1) {
    switch (c) {
    case 'w':
      width = atoi(optarg);
      break;
    case 'h':
      height = atoi(optarg);
      break;
    }
  }

  if (width <= 10 || height <= 10) {
    fprintf(stderr, "Usage: %s [-w <width>] [-h <height>]\n\
        width and height must be at least 10 (default 50)\n",
            argv[0]);
    exit(1);
  }

  map = calloc(width * height, sizeof(uint8_t));
  if (map == NULL) {
    fprintf(stderr, "Out of memory\n");
    exit(1);
  }

  srand(time(NULL));

  make_maze();
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
      if (step_back == OUTOFBOUNDS || stack_pointer < 0) {
        break;
      }
      crawler_look_to(&crawler.position, &crawler.position, step_back);
    }
  }
  display_map();
}

void display_map() {
  for (int y = height - 1; y >= 0; y--) {
    for (int x = 0; x < width; x++) {
      printf("%s", gfx[*cell_xy(x, y)]);
    }
    printf("\n");
  }
}

MAP_CELL_TYPE cell_is(coords *coords) { return *cell(coords); }
MAP_CELL_TYPE *cell(coords *coords) { return cell_xy(coords->x, coords->y); }
MAP_CELL_TYPE *cell_xy(int x, int y) { return &map[x + y * width]; }

void crawler_look_to(coords *from, coords *to, uint8_t dir) {
  switch (dir) {
  case DIR_NORTH:
    to->x = from->x;
    to->y = from->y + 1;
    break;
  case DIR_EAST:
    to->x = from->x + 1;
    to->y = from->y;
    break;
  case DIR_SOUTH:
    to->x = from->x;
    to->y = from->y - 1;
    break;
  case DIR_WEST:
    to->x = from->x - 1;
    to->y = from->y;
    break;
  }
}

char dir_to_str(DIRECTION dir) {
  switch (dir) {
  case DIR_NORTH:
    return 'N';
  case DIR_EAST:
    return 'E';
  case DIR_SOUTH:
    return 'S';
  case DIR_WEST:
    return 'W';
  }
  return '?';
}

int crawler_in_bounds(coords *coords) {
  if (coords->x < 0 || coords->x >= width || coords->y < 0 ||
      coords->y >= height) {
    return 0;
  }
  return 1;
}

void crawler_info(crawler *crawler) {
  fprintf(stderr, "Crawler at (%d, %d) Attempt: %d Dirs:", crawler->position.x,
          crawler->position.y, crawler->attempt);
  for (int n = 0; n < 5; n++) {
    fprintf(stderr, " %d", crawler->directions[n]);
  }
  fprintf(stderr, "\n");
}

void crawler_init(crawler *crawler) {
  crawler->position.x = (rand() % width);
  crawler->position.y = (rand() % height);
  crawler->attempt = 0;

  for (int n = 0; n < 4; n++) {
    crawler->directions[n] = n;
  }
  crawler->directions[4] = 9;

  crawler_randomize_directions(crawler);
}

void crawler_randomize_directions(crawler *crawler) {
  crawler->attempt = 0;
  uint8_t tmp;
  int swap;
  for (int n = 0; n < 4; n++) {
    swap = rand() % 4;
    tmp = crawler->directions[n];
    crawler->directions[n] = crawler->directions[swap];
    crawler->directions[swap] = tmp;
  }
}
