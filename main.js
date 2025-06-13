const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');

const GRID_WIDTH = 60;
const GRID_HEIGHT = 40;
const CELL_SIZE = 12; // Size of each cell in pixels

canvas.width = GRID_WIDTH * CELL_SIZE;
canvas.height = GRID_HEIGHT * CELL_SIZE;

Module.onRuntimeInitialized = () => {
    console.log("Wasm module loaded.");

    // Wrap the C++ functions
    const initGrid = Module.cwrap('initGrid', null, ['number', 'number']);
    const getGridPtr = Module.cwrap('getGridPtr', 'number', []);
    const setCell = Module.cwrap('setCell', null, ['number', 'number']);

    // This function reads the grid from Wasm memory and draws it to the canvas
    function drawGrid() {
        // Get the memory address of the grid's first element
        const gridPtr = getGridPtr();

        // Create a JavaScript view (an 8-bit unsigned integer array) into the
        // Wasm memory. This doesn't copy the data, it's a direct view.
        const gridData = new Uint8Array(Module.HEAPU8.buffer, gridPtr, GRID_WIDTH * GRID_HEIGHT);

        // Clear the canvas for the new drawing
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        
        // Set styles for the cells
        ctx.fillStyle = '#00FF00'; // Green for live cells
        ctx.strokeStyle = '#222'; // Dark grid lines

        // Loop through every cell
        for (let r = 0; r < GRID_HEIGHT; r++) {
            for (let c = 0; c < GRID_WIDTH; c++) {
                const index = r * GRID_WIDTH + c;

                // If the cell value is 1 (alive), fill the rectangle
                if (gridData[index] === 1) {
                    ctx.fillRect(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                }

                // Always draw the cell border
                ctx.strokeRect(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE);
            }
        }
    }

    canvas.addEventListener('click', (event) => {
        // Get the bounding rectangle of the canvas.
        const rect =  canvas.getBoundingClientRect();

        // Mouse coords relative to the canvas
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;

        // Convert the pixel coordinates to grid cell coordinates.
        const col = Math.floor(x / CELL_SIZE);
        const row = Math.floor(y / CELL_SIZE);

        // Call the C++ function to toggle the state of the cell 
        setCell(row, col);

        // Redraw the grid
        drawGrid();
    })
    initGrid(GRID_WIDTH, GRID_HEIGHT);

    setCell(10, 10);
    setCell(10, 11);
    setCell(10, 12);
    
    drawGrid();
};
