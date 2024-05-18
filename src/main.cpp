#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>

#include "RenderWindow.hpp"
#include "Utilities.hpp"
#include "AStar.hpp"

#include "CursorBox.hpp"

std::vector<std::vector<double>> brushGrid(const std::vector<std::vector<double>> &grid, const int &row, const int &col, const double &strength, const int &radius, const double &maxVal, const double &minVal = 0.0) {
    if (row < 0 || row >= (int)grid.size() || col < 0 || col >= (int)grid.at(row).size()) {return grid;}
    std::vector<std::vector<double>> output = grid;
    for (int i = 0; i < radius; i++) {
        for (int j = radius - i; j < radius + i + 1; j++) {
            const int rowT = row - radius + i, rowB = row + radius - i, c = col - radius + j;
            if (rowT >= 0 && c >= 0 && c < (int)grid.at(i).size()) {
                output[rowT][c] += strength;
                if (output[rowT][c] > maxVal) {output[rowT][c] = maxVal;}
                else if (output[rowT][c] < minVal) {output[rowT][c] = minVal;}
            }
            if (rowB < (int)grid.size() && c >= 0 && c < (int)grid.at(i).size()) {
                output[rowB][c] += strength;
                if (output[rowB][c] > maxVal) {output[rowB][c] = maxVal;}
                else if (output[rowB][c] < minVal) {output[rowB][c] = minVal;}
            }
        }
    }
    for (int i = col - radius; i < col + radius + 1; i++) {
        if (i >= 0 && i < (int)grid.at(row).size()) {
            output[row][i] += strength;
            if (output[row][i] > maxVal) {output[row][i] = maxVal;}
            else if (output[row][i] < minVal) {output[row][i] = minVal;}
        }
    }
    return output;
}

double HireTime_Sec() {return SDL_GetTicks() * 0.01f;}
int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {std::cout << "Error initializing SDL2\nERROR: " << SDL_GetError() << "\n";}
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {std::cout << "Error initializing SDL2_image\nERROR: " << IMG_GetError() << "\n";}
    if (TTF_Init() == -1) {std::cout << "Error initializing SDL2_ttf\nERROR: " << TTF_GetError() << "\n";}

    RenderWindow Window("Trailblazer", 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY);
    SDL_Event Event;
    const Uint8 *Keystate = SDL_GetKeyboardState(NULL);

    TTF_Font *font = TTF_OpenFont("dev/fonts/GNU-Unifont.ttf", 30);

    Texture tile(Window.loadTexture("dev/png/tile.png"), {32, 32}, {0, 0, 64, 64});
    tile.setMods({64, 64, 64, 255});
    const int tileSize = 64;

    Texture arrowButton(Window.loadTexture("dev/png/btn_arrow.png"), {16, 16}, {0, 0, 32, 32});

    MouseState mstate;

    struct {
        int Quit = SDL_SCANCODE_ESCAPE;
        int ToggleCapture = SDL_SCANCODE_F1;
        int Pathfind = SDL_SCANCODE_SPACE;
        int PlaceStart = SDL_SCANCODE_Z;
        int PlaceGoal = SDL_SCANCODE_X;
        int ClearMaze = SDL_SCANCODE_C;
        int HardBrush = SDL_SCANCODE_S;
        int HardErase = SDL_SCANCODE_D;
    } Keybinds;

    long double t = 0.0;
    double dt = 0.01;

    int startTicks = 0, frameTicks = 0;
    double currentTime = HireTime_Sec();
    long double newTime = 0.0;
    long double frameTime = 0.0;
    double accumulator = 0.0;

    struct {
        double Strength = 10.0;
        double StrengthMax = 100.0;
        double StrengthMin = 1.0;
        int Radius = 5;
        int RadiusMax = 50;
        int RadiusMin = 0;
    } Tool;
    struct {
        std::vector<std::vector<double>> Grid;
        
        const std::vector<int> CellSizes = {1, 2, 3, 4, 6, 8, 9, 12, 16, 18, 24, 36, 48, 72, 144};
        int SizeIndex = 4;
        SDL_Point Dims = {720 / CellSizes[SizeIndex], 576 / CellSizes[SizeIndex]};
        
        double MaxVal = 100.0;
        double MinVal = 0.0;
        
        std::pair<unsigned long int, unsigned long int> Start = std::make_pair(0, 0), Goal = std::make_pair(Dims.y - 1, Dims.x - 1);
        SDL_Point Pos = {0, 0}, PrevPos = Pos;
        SDL_Point Offset = {72, 40};
    } Map;
    struct {
        std::vector<std::pair<unsigned long int, unsigned long int>> Nodes;
        double MaxUp = 5.0, MaxDown = 10.0;
    } Pathfinder;

    for (int i = 0; i < Map.Dims.y; i++) {
        Map.Grid.emplace_back();
        for (int j = 0; j < Map.Dims.x; j++) {
            Map.Grid[i].emplace_back(Map.MinVal);
        }
    }

    CursorBox map({Map.Offset.x, Map.Offset.y, 720, 576}), genPath({8, 655, 209, 45}), placeStart({225, 655, 209, 45}), placeGoal({442, 655, 209, 45}), gridReset({659, 655, 209, 45});
    CursorBox increments[14] = {
        CursorBox({1149, 113, 32, 32}), CursorBox({1197, 113, 32, 32}),
        CursorBox({1149, 180, 32, 32}), CursorBox({1197, 180, 32, 32}),
        CursorBox({1149, 297, 32, 32}), CursorBox({1197, 297, 32, 32}),
        CursorBox({1149, 364, 32, 32}), CursorBox({1197, 364, 32, 32}),
        CursorBox({1149, 431, 32, 32}), CursorBox({1197, 431, 32, 32}),
        CursorBox({1149, 548, 32, 32}), CursorBox({1197, 548, 32, 32}),
        CursorBox({1149, 615, 32, 32}), CursorBox({1197, 615, 32, 32})
    };

    int drawMode = 0;

    bool running = true, madeChanges = true;
    while (running) {
        startTicks = SDL_GetTicks();
        newTime = HireTime_Sec();
        frameTime = newTime - currentTime;
        currentTime = newTime;
        accumulator += frameTime;

        while (accumulator >= dt) {
            while (SDL_PollEvent(&Event)) {
                switch (Event.type) {
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_MOUSEMOTION:
                        mstate.Motion = true;
                        SDL_GetMouseState(&mstate.PosR.x, &mstate.PosR.y);
                        mstate.PosC.x = mstate.PosR.x - Window.getW_2();
                        mstate.PosC.y = Window.getH() - mstate.PosR.y - Window.getH_2();

                        if (!mstate.Released) {
                            mstate.Rel.x = Event.motion.xrel;
                            mstate.Rel.y = -Event.motion.yrel;
                        } else {
                            mstate.Released = false;
                            mstate.Rel = {0, 0};
                        }

                        Map.Pos = {(mstate.PosR.x - Map.Offset.x) / Map.CellSizes[Map.SizeIndex], (mstate.PosR.y - Map.Offset.y) / Map.CellSizes[Map.SizeIndex]};
                        break;
                    case SDL_KEYDOWN:
                        if (!Event.key.repeat) {
                            if (Keystate[Keybinds.ToggleCapture]) {
                                mstate.Captured = !mstate.Captured;
                                if (mstate.Captured) {
                                    SDL_SetRelativeMouseMode(SDL_TRUE);
                                    mstate.Released = true;
                                    mstate.Rel = {0, 0};
                                } else {
                                    SDL_SetRelativeMouseMode(SDL_FALSE);
                                    Window.centerMouse();
                                    mstate.PosC = {0, 0};
                                    mstate.PosR = {0, 0};
                                }
                            }
                        }
                        break;
                    case SDL_WINDOWEVENT:
                        Window.handleEvent(Event.window);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        mstate.Pressed[Event.button.button] = true;
                        switch (Event.button.button) {
                            case SDL_BUTTON_LEFT:
                                if (genPath.check(mstate)) {
                                    Pathfinder.Nodes = AStar.euclidean(Map.Grid, Map.Start, Map.Goal, Pathfinder.MaxUp, Pathfinder.MaxDown, ASTAR_MOVE_NOBOUND);
                                    if (Pathfinder.Nodes.size() <= 1) {std::cout << "[Pathfinding] No path found\n";}
                                    else {
                                        std::cout << "[Path] Path found\n";
                                        madeChanges = true;
                                    }
                                } else if (placeStart.check(mstate)) {
                                    drawMode = 1;
                                } else if (placeGoal.check(mstate)) {
                                    drawMode = 2;
                                } else if (gridReset.check(mstate)) {
                                    for (unsigned long int i = 0; i < Map.Grid.size(); i++) {
                                        for (unsigned long int j = 0; j < Map.Grid.at(i).size(); j++) {
                                            Map.Grid[i][j] = Map.MinVal;
                                        }
                                    }
                                    std::cout << "[Grid] Grid cleared\n";
                                    madeChanges = true;
                                } else {
                                    for (unsigned long int i = 0; i < 14; i++) {
                                        if (increments[i].check(mstate)) {
                                            switch(i) {
                                                case 0:
                                                    Pathfinder.MaxUp += 1.0;
                                                    std::cout << "[Path] Increased upwards mobility - now " << Pathfinder.MaxUp << "\n";
                                                    break;
                                                case 1:
                                                    Pathfinder.MaxUp -= 1.0;
                                                    if (Pathfinder.MaxUp < 0) {Pathfinder.MaxUp = 0;}
                                                    else {std::cout << "[Path] Decreased upwards mobility - now " << Pathfinder.MaxUp << "\n";}
                                                    break;
                                                case 2:
                                                    Pathfinder.MaxDown += 1.0;
                                                    std::cout << "[Path] Increased downwards mobility - now " << Pathfinder.MaxUp << "\n";
                                                    break;
                                                case 3:
                                                    Pathfinder.MaxDown -= 1.0;
                                                    if (Pathfinder.MaxDown < 0) {Pathfinder.MaxDown = 0;}
                                                    else {std::cout << "[Path] Decreased downwards mobility - now " << Pathfinder.MaxUp << "\n";}
                                                    break;
                                                case 4:
                                                    Map.SizeIndex++;
                                                    if (Map.SizeIndex > 14) {Map.SizeIndex = 14;}

                                                    Map.Dims = {720 / Map.CellSizes[Map.SizeIndex], 576 / Map.CellSizes[Map.SizeIndex]};
                                                    Map.Start = std::make_pair(0, 0);
                                                    Map.Goal = std::make_pair(Map.Dims.y - 1, Map.Dims.x - 1);

                                                    Map.Grid.clear();
                                                    for (int i = 0; i < Map.Dims.y; i++) {
                                                        Map.Grid.emplace_back();
                                                        for (int j = 0; j < Map.Dims.x; j++) {
                                                            Map.Grid[i].emplace_back(Map.MinVal);
                                                        }
                                                    }
                                                    Pathfinder.Nodes.clear();
                                                    std::cout << "[Grid] Grid Cleared; Increased cell size - now " << Map.CellSizes[Map.SizeIndex] << " (" << Map.Dims.x << " x " << Map.Dims.y << ")\n";
                                                    break;
                                                case 5:
                                                    Map.SizeIndex--;
                                                    if (Map.SizeIndex < 0) {Map.SizeIndex = 0;}

                                                    Map.Dims = {720 / Map.CellSizes[Map.SizeIndex], 576 / Map.CellSizes[Map.SizeIndex]};
                                                    Map.Start = std::make_pair(0, 0);
                                                    Map.Goal = std::make_pair(Map.Dims.y - 1, Map.Dims.x - 1);

                                                    Map.Grid.clear();
                                                    for (int i = 0; i < Map.Dims.y; i++) {
                                                        Map.Grid.emplace_back();
                                                        for (int j = 0; j < Map.Dims.x; j++) {
                                                            Map.Grid[i].emplace_back(Map.MinVal);
                                                        }
                                                    }
                                                    Pathfinder.Nodes.clear();
                                                    std::cout << "[Grid] Grid Cleared; Decreased cell size - now " << Map.CellSizes[Map.SizeIndex] << " (" << Map.Dims.x << " x " << Map.Dims.y << ")\n";
                                                    break;
                                                case 6:
                                                    Map.MinVal += 1.0;
                                                    std::cout << "[Grid] Increased minimum cell value - now " << Map.MinVal << "\n"; 
                                                    break;
                                                case 7:
                                                    Map.MinVal -= 1.0;
                                                    std::cout << "[Grid] Decreased minimum cell value - now " << Map.MinVal << "\n"; 
                                                    break;
                                                case 8:
                                                    Map.MaxVal += 1.0;
                                                    std::cout << "[Grid] Increased maximum cell value - now " << Map.MinVal << "\n"; 
                                                    break;
                                                case 9:
                                                    Map.MaxVal -= 1.0;
                                                    std::cout << "[Grid] Decreased maximum cell value - now " << Map.MinVal << "\n"; 
                                                    break;
                                                case 10:
                                                    Tool.Radius++;
                                                    if (Tool.Radius > Tool.RadiusMax) {Tool.Radius = Tool.RadiusMax;}
                                                    else {std::cout << "[Tool] Increased radius - now " << Tool.Radius << "\n";}
                                                    break;
                                                case 11:
                                                    Tool.Radius--;
                                                    if (Tool.Radius < Tool.RadiusMin) {Tool.Radius = Tool.RadiusMin;}
                                                    else {std::cout << "[Tool] Decreased radius - now " << Tool.Radius << "\n";}
                                                    break;
                                                case 12:
                                                    Tool.Strength += 1.0;
                                                    if (Tool.Strength > Tool.StrengthMax) {Tool.Strength = Tool.StrengthMax;}
                                                    else {std::cout << "[Tool] Increased strength - now " << Tool.Strength << "\n";}
                                                    break;
                                                case 13:
                                                    Tool.Strength -= 1.0;
                                                    if (Tool.Strength < Tool.StrengthMin) {Tool.Strength = Tool.StrengthMin;}
                                                    else {std::cout << "[Tool] Decreased strength - now " << Tool.Strength << "\n";}
                                                    break;
                                            }
                                            madeChanges = true;
                                            break;
                                        }
                                    }
                                }

                                if (map.check(mstate)) {
                                    switch (drawMode) {
                                        case 0:
                                            Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, Tool.Strength, Tool.Radius, Map.MaxVal, Map.MinVal);
                                            break;
                                        case 1:
                                            Map.Start = std::make_pair(Map.Pos.y, Map.Pos.x);
                                            std::cout << "[Grid] Start moved to " << Map.Start.first << ", " << Map.Start.second << "\n";
                                            break;
                                        case 2:
                                            Map.Goal = std::make_pair(Map.Pos.y, Map.Pos.x);
                                            std::cout << "[Grid] Goal moved to " << Map.Goal.first << ", " << Map.Goal.second << "\n";
                                            break;
                                    }
                                    drawMode = 0;
                                    madeChanges = true;
                                }
                                break;
                            case SDL_BUTTON_RIGHT:
                                if (map.check(mstate)) {
                                    Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, -Tool.Strength, Tool.Radius, Map.MaxVal, Map.MinVal);
                                    madeChanges = true;
                                }
                                break;
                        }
                        break;
                    case SDL_MOUSEBUTTONUP:
                        mstate.Pressed[Event.button.button] = false;
                        break;
                }
                if (!running) {break;}
            }
            if (!running || Keystate[Keybinds.Quit]) {
                running = false;
                break;
            }

            if ((Map.Pos.x != Map.PrevPos.x) || (Map.Pos.y != Map.PrevPos.y)) {
                Map.PrevPos = Map.Pos;

                if (mstate.Pressed[SDL_BUTTON_LEFT]) {
                    Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, Tool.Strength, Tool.Radius, Map.MaxVal, Map.MinVal);
                    madeChanges = true;
                } else if (mstate.Pressed[SDL_BUTTON_RIGHT]) {
                    Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, -Tool.Strength, Tool.Radius, Map.MaxVal, Map.MinVal);
                    madeChanges = true;
                }
                if (Keystate[Keybinds.HardBrush]) {
                    Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, Map.MaxVal, Tool.Radius, Map.MaxVal, Map.MinVal);
                    madeChanges = true;
                }
                if (Keystate[Keybinds.HardErase]) {
                    Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, -Map.MaxVal, Tool.Radius, Map.MaxVal, Map.MinVal);
                    madeChanges = true;
                }
            }

            t += dt;
            accumulator -= dt;
            mstate.Motion = false;
        }
        if (!running) {break;}

        if (madeChanges) {
            madeChanges = false;
            Window.clear();

            // Background
            for (int i = 0; i < Window.getH() / tileSize + 1; i++) {
                for (int j = 0; j < Window.getW() / tileSize; j++) {
                    const SDL_Rect tileFrame = {-Window.getW_2() + j * tileSize, Window.getH_2() + (int)(tileSize * 0.375) - i * tileSize, tileSize, tileSize};
                    Window.renderTexture(tile, tileFrame);
                }
            }

            const SDL_Point tileOffset = {Window.getW() % tileSize, Window.getH() % tileSize};
            for (int i = -1; i < Window.getW() / tileSize + 1; i++) {
                for (int j = -1; j < Window.getH() / tileSize + 1; j++) {
                    const SDL_Point p = {-Window.getW_2() + i * tileSize + tile.getCenter().x + tileOffset.x / 2, Window.getH_2() - j * tileSize - tile.getCenter().y - tileOffset.y / 2};
                    Window.renderTexture(tile, p);
                }
            }

            // Frame surrounding the grid
            Window.fillRectangle(-588, 340, 760, 616, PresetColors[COLOR_DARK_GRAY]);
            Window.fillRectangle(-583, 335, 750, 606, PresetColors[COLOR_LIGHT_GRAY]);
            Window.fillRectangle(-573, 325, 730, 586, PresetColors[COLOR_DARK_GRAY]);

            // Sidebar frame
            Window.fillRectangle(235,  322,  20, 644, PresetColors[COLOR_DARK_GRAY]);
            Window.fillRectangle(609,  322,  20, 644, PresetColors[COLOR_DARK_GRAY]);
            Window.fillRectangle(235,  322, 394,  20, PresetColors[COLOR_DARK_GRAY]);
            Window.fillRectangle(235,  133, 394,  15, PresetColors[COLOR_DARK_GRAY]);
            Window.fillRectangle(235, -118, 394,  15, PresetColors[COLOR_DARK_GRAY]);
            Window.fillRectangle(235, -302, 394,  20, PresetColors[COLOR_DARK_GRAY]);

            Window.fillRectangle(240,  317,  10, 634, PresetColors[COLOR_LIGHT_GRAY]);
            Window.fillRectangle(614,  317,  10, 634, PresetColors[COLOR_LIGHT_GRAY]);
            Window.fillRectangle(240,  317, 384,  10, PresetColors[COLOR_LIGHT_GRAY]);
            Window.fillRectangle(240,  128, 384,   5, PresetColors[COLOR_LIGHT_GRAY]);
            Window.fillRectangle(240, -123, 384,   5, PresetColors[COLOR_LIGHT_GRAY]);
            Window.fillRectangle(240, -307, 384,  10, PresetColors[COLOR_LIGHT_GRAY]);

            // Button outlines
            Window.fillRectangle(-632, -295, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-632, -295,   5, 45, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-632, -335, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-428, -295,   5, 45, PresetColors[COLOR_WHITE]);

            Window.fillRectangle(-415, -295, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-415, -295,   5, 45, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-415, -335, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-211, -295,   5, 45, PresetColors[COLOR_WHITE]);

            Window.fillRectangle(-198, -295, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-198, -295,   5, 45, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-198, -335, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(   6, -295,   5, 45, PresetColors[COLOR_WHITE]);

            Window.fillRectangle( 19, -295, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle( 19, -295,   5, 45, PresetColors[COLOR_WHITE]);
            Window.fillRectangle( 19, -335, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(223, -295,   5, 45, PresetColors[COLOR_WHITE]);

            // Pathfinding text
            Window.renderText(font, u"Generate Path", {-529, -318}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, u"Place Start", {-312, -318}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, u"Place Goal", {-95, -318}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, u"Reset Grid", {123, -318}, 0, PresetColors[COLOR_WHITE]);

            // Path Settings text
            Window.renderText(font, u"Path Settings:", {373, 279}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, (u"Up:   " + btils::to_u16string<std::string>(btils::tstr_AddZeros<int>(Pathfinder.MaxUp, 4, 3, false))).c_str(), {373, 232}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, (u"Down: " + btils::to_u16string<std::string>(btils::tstr_AddZeros<int>(Pathfinder.MaxDown, 4, 3, false))).c_str(), {373, 165}, 0, PresetColors[COLOR_WHITE]);

            // Grid Settings text
            Window.renderText(font, u"Grid Settings:", {373, 95}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, (u"Cell Size: " + btils::to_u16string<std::string>(btils::tstr_Length<int>(Map.CellSizes[Map.SizeIndex], 3, false, false))).c_str(), {373, 48}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, (u"Min:  " + btils::to_u16string<std::string>(btils::tstr_AddZeros<int>(Map.MinVal, 4, 3, false))).c_str(), {373, -20}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, (u"Max:  " + btils::to_u16string<std::string>(btils::tstr_AddZeros<int>(Map.MaxVal, 4, 3, false))).c_str(), {373, -87}, 0, PresetColors[COLOR_WHITE]);

            // Tool Settings text
            Window.renderText(font, u"Tool Settings:", {373, -157}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, (u"Size:      " + btils::to_u16string<std::string>(btils::tstr_Length<int>(Tool.Radius, 3, false, false))).c_str(), {373, -204}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, (u"Strength:  " + btils::to_u16string<std::string>(btils::tstr_Length<int>(Tool.Strength, 3, false, false))).c_str(), {373, -271}, 0, PresetColors[COLOR_WHITE]);

            // Increment/Decrement buttons
            arrowButton.setAngle(0);
            Window.renderTexture(arrowButton, 509,  247);
            Window.renderTexture(arrowButton, 509,  180);
            Window.renderTexture(arrowButton, 509,   63);
            Window.renderTexture(arrowButton, 509,   -4);
            Window.renderTexture(arrowButton, 509,  -71);
            Window.renderTexture(arrowButton, 509, -188);
            Window.renderTexture(arrowButton, 509, -255);

            arrowButton.setAngle(C_PI);
            Window.renderTexture(arrowButton, 557,  247);
            Window.renderTexture(arrowButton, 557,  180);
            Window.renderTexture(arrowButton, 557,   63);
            Window.renderTexture(arrowButton, 557,   -4);
            Window.renderTexture(arrowButton, 557,  -71);
            Window.renderTexture(arrowButton, 557, -188);
            Window.renderTexture(arrowButton, 557, -255);

            // Grid
            for (unsigned long int i = 0; i < Map.Grid.size(); i++) {
                for (unsigned long int j = 0; j < Map.Grid.at(i).size(); j++) {
                    const unsigned char shade = 255 - btils::map<double, unsigned char>(Map.Grid.at(i).at(j), Map.MinVal, Map.MaxVal, 0, 255);
                    Window.fillRectangle(-Window.getW_2() + j * Map.CellSizes[Map.SizeIndex] + Map.Offset.x, Window.getH_2() - i * Map.CellSizes[Map.SizeIndex] - Map.Offset.y, Map.CellSizes[Map.SizeIndex], Map.CellSizes[Map.SizeIndex], {shade, shade, shade, 255});
                }
            }

            // Path
            Window.fillRectangle(-Window.getW_2() + Map.Start.second * Map.CellSizes[Map.SizeIndex] + Map.Offset.x, Window.getH_2() - Map.Start.first * Map.CellSizes[Map.SizeIndex] - Map.Offset.y, Map.CellSizes[Map.SizeIndex], Map.CellSizes[Map.SizeIndex], PresetColors[COLOR_TEAL]);
            Window.fillRectangle(-Window.getW_2() +  Map.Goal.second * Map.CellSizes[Map.SizeIndex] + Map.Offset.x, Window.getH_2() -  Map.Goal.first * Map.CellSizes[Map.SizeIndex] - Map.Offset.y, Map.CellSizes[Map.SizeIndex], Map.CellSizes[Map.SizeIndex], PresetColors[COLOR_MAROON]);
            for (unsigned long int i = 1; i < Pathfinder.Nodes.size(); i++) {
                Window.drawLine(-Window.getW_2() + Map.CellSizes[Map.SizeIndex] / 2 + Pathfinder.Nodes.at(i - 1).second * Map.CellSizes[Map.SizeIndex] + Map.Offset.x, Window.getH_2() - Map.CellSizes[Map.SizeIndex] / 2 - Pathfinder.Nodes.at(i - 1).first  * Map.CellSizes[Map.SizeIndex] - Map.Offset.y, -Window.getW_2() + Map.CellSizes[Map.SizeIndex] / 2 + Pathfinder.Nodes.at(i).second * Map.CellSizes[Map.SizeIndex] + Map.Offset.x, Window.getH_2() - Map.CellSizes[Map.SizeIndex] / 2 - Pathfinder.Nodes.at(i).first * Map.CellSizes[Map.SizeIndex] - Map.Offset.y, PresetColors[COLOR_LIME]);
            }

            Window.show();
        }

        if ((frameTicks = SDL_GetTicks() - startTicks) < 1000 / Window.getRefreshRate()) {SDL_Delay(1000 / Window.getRefreshRate() - frameTicks);}
    }

    TTF_CloseFont(font);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0; 
}
