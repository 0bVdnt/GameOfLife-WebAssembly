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
} // end of extern "C"
