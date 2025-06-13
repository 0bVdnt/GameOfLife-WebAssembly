#include <cstdint> // for uint8_t
#include <emscripten.h>
#include <vector>

int gridWidth;
int gridHeight;

std::vector<uint8_t> grid;

int getIdx(int r, int c) { return r * gridWidth + c; }

extern "C" {
EMSCRIPTEN_KEEPALIVE
void initGrid(int width, int height) {
  gridWidth = width;
  gridHeight = height;
  grid.assign(width * height, 0);
}

// This function returns a raw pointer to the beginning of grid data.
// JavaScript will use this pointer to read the grid state directly from Wasm
// memory.
EMSCRIPTEN_KEEPALIVE
uint8_t *getGridPtr() {
  // .data() returns a direct pointer to the underlying array, which is safe and
  // efficient.
  return grid.data();
}

EMSCRIPTEN_KEEPALIVE
void setCell(int r, int c) {
  if (r >= 0 && r < gridHeight && c >= 0 && c < gridWidth) {
    int index = getIdx(r, c);
    // This is the toggle logic. If the cell is 1, it becomes 0.
    // If it's 0, it becomes 1.
    grid[index] = !grid[index];
  }
}

EMSCRIPTEN_KEEPALIVE
void nextGeneration() {
  // A temporary copy of the grid to store the next state.
  // A "double buffer".
  std::vector<uint8_t> nextGrid = grid;

  for (int r = 0; r < gridHeight; ++r) {
    for (int c = 0; c < gridWidth; ++c) {
      int liveNeighbors = 0;
      for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
          if (i == 0 && j == 0)
            continue; // Do not count the cell itself.

          int neighbor_r = r + i;
          int neighbor_c = c + j;

          // Check boundaries to prevent wrap-around.
          if (neighbor_r >= 0 && neighbor_r < gridHeight && neighbor_c >= 0 &&
              neighbor_c < gridWidth) {
            if (grid[getIdx(neighbor_r, neighbor_c)]) {
              liveNeighbors++;
            }
          }
        }
      }

      // The Rules of Life.
      int currentIndex = getIdx(r, c);
      bool isAlive = grid[currentIndex];

      if (isAlive) {
        // A living cell dies if it has < 2 or > 3 neighbors.
        if (liveNeighbors < 2 || liveNeighbors > 3) {
          nextGrid[currentIndex] = 0;
        }
      } else {
        // A dead cell becomes alive if it has exactly 3 neighbors.
        if (liveNeighbors == 3) {
          nextGrid[currentIndex] = 1;
        }
      }
    }
  }

  grid = nextGrid;
}
} // end of extern "C"
