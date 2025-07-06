#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define COLS 20
#define ROWS 20
#define CELLS (COLS*ROWS)

typedef enum {
    DEAD = 0,
    ALIVE = 1
}CellState;

#define CHAR_ALIVE '*'
#define CHAR_DEAD '.'

typedef struct {
    int rows;
    int cols;
    CellState *cells;
}Grid;

/**
 * @brief create and allocate a new grid
 * @return pointer to new grid, or NULL
 */
Grid* grid_create(int rows, int cols){
    Grid *grid = malloc(sizeof(Grid));
    if(!grid) return NULL;

    grid->rows = rows;
    grid->cols = cols;
    grid->cells = malloc(rows*cols*sizeof(CellState));
    if(!grid->cells){
        free(grid);
        return NULL;
    }
    return grid;
}

/**
 * @brief free memory occupied by grid
 */
void grid_destroy(Grid *grid){
    if(grid){
        free(grid->cells);
        free(grid);
    }
}

/**
 * @brief translate the specified 2D coordinates in 1D index with wrapping
 */
static inline int cell_to_index(const Grid *grid,int x, int y){
    if(x < 0){
        x = (-x) % COLS;
        x = COLS - x;
    }
    if(y < 0){
        y = (-y) % ROWS;
        y = ROWS - y;
    }
    if(x >= COLS) x = x % COLS;
    if(y >= ROWS) y = y % ROWS;

    return y*COLS+x;
}

/**
 * @brief sets specified cell at x,y to the specified state.
*/
void set_cell(Grid *grid, int x, int y, CellState state){
    grid->cells[cell_to_index(grid,x,y)] = state;
}

/**
 * @brief function returns the state of the grid at x,y
 */
CellState get_cell(const Grid *grid, int x, int y){
    return grid->cells[cell_to_index(grid,x,y)];
}

/**
 * @brief return number of living cells neighbors of x, y
 */
int count_living_neighbors(Grid *grid, int x, int y){
    int alive = 0;

    for(int yo = -1; yo<=1; yo++){
        for(int xo = -1; xo<=1; xo++){
            if(xo == 0 && yo == 0) continue;

            alive += get_cell(grid, x+xo, y+yo);
        }
    }
    return alive;
}

/**
 * @brief compute new state of Game of Life according to
 * its rules
 */
void compute_new_state(Grid *old, Grid *new){
    for(int y = 0; y < old->rows; y++){
        for(int x = 0; x < old->cols; x++){
            int n_alive = count_living_neighbors(old,x,y);
            CellState current_state = get_cell(old, x, y);
            CellState new_state = DEAD;

            if(current_state == ALIVE){
                if(n_alive == 2 || n_alive == 3) new_state = ALIVE;
            }else{
                if(n_alive == 3) new_state = ALIVE;
            }
            set_cell(new,x,y,new_state);
        }
    }
}

/**
 * @brief shows the grid on the screen, clearing the terminal
 * using the required VT100 escape sequence.
 */
void print_grid(Grid *grid){
    printf("\x1b[H\x1b[2J\x1b[3J");     //clear screen
    for(int y = 0; y < grid->rows; y++){
        for(int x = 0; x < grid->cols; x++){
            putchar(get_cell(grid, x, y) == ALIVE ? CHAR_ALIVE : CHAR_DEAD);
        }
        putchar('\n');
    }
}

/**
 * @brief sets all cells to a specified state
 */
void grid_set_all(Grid *grid, CellState state){
    for(int i = 0; i < grid->rows; i++){
        grid->cells[i] = state;
    }
}

/**
 * @brief adds glider pattern to grid
 */
void grid_add_glider(Grid *grid, int x, int y){
    set_cell(grid, x + 1, y,     ALIVE);
    set_cell(grid, x + 2, y + 1, ALIVE);
    set_cell(grid, x,     y + 2, ALIVE);
    set_cell(grid, x + 1, y + 2, ALIVE);
    set_cell(grid, x + 2, y + 2, ALIVE);
}

int main(void){
    Grid *current_grid = grid_create(ROWS, COLS);
    Grid *next_grid = grid_create(ROWS,COLS);
    if(!current_grid || !next_grid){
        fprintf(stderr, "Error: couldnt allocate memory for grids.\n");
        return 1;
    }

    grid_set_all(current_grid, DEAD);

    grid_add_glider(current_grid, 10, 10);
    
    print_grid(current_grid);

    while(1){
        print_grid(current_grid);
        compute_new_state(current_grid, next_grid);

        Grid *temp = current_grid;
        current_grid = next_grid;
        next_grid = temp;

        usleep(150000);
    }

    grid_destroy(current_grid);
    grid_destroy(next_grid);

    return 0;
}