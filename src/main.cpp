#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>

#include "RenderWindow.hpp"
#include "Utilities.hpp"
#include "AStar.hpp"

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
    const int arrowButtonSize = 32;

    struct {
        SDL_Point PosC = {0, 0};
        SDL_Point PosR = {0, 0};
        SDL_Point Rel = {0, 0};
        double Sensitivity = 10;

        bool Motion = false;
        bool Released = true;
        bool Captured = false;

        bool Pressed[5] = {false, false, false, false, false};
    } MouseState;

    struct {
        int Quit = SDL_SCANCODE_ESCAPE;
        int ToggleCapture = SDL_SCANCODE_F1;
        int Pathfind = SDL_SCANCODE_1;
        int PlaceStart = SDL_SCANCODE_Z;
        int PlaceGoal = SDL_SCANCODE_X;
        int ClearMaze = SDL_SCANCODE_C;
        int HardBrush = SDL_SCANCODE_F;
        int HardErase = SDL_SCANCODE_G;
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
        int Radius = 0;
        int RadiusMax = 15;
        int RadiusMin = 0;
    } Tool;

    struct {
        std::vector<std::vector<double>> Grid;
        
        const std::vector<int> Sizes = {1, 2, 3, 4, 6, 8, 9, 12, 16, 18, 24, 36, 48, 72, 144};
        int CellSize = 6;
        SDL_Point Dims = {720 / CellSize, 576 / CellSize};
        
        double MaxVal = 100.0;
        double MinVal = 0.0;
        
        std::pair<unsigned long int, unsigned long int> Start = std::make_pair(0, 0), Goal = std::make_pair(Dims.y - 1, Dims.x - 1);
        SDL_Point Pos = {0, 0}, PrevPos = Pos;
        SDL_Point Offset = {72, 40};
    } Map;

    for (int i = 0; i < Map.Dims.y; i++) {
        Map.Grid.emplace_back();
        for (int j = 0; j < Map.Dims.x; j++) {
            Map.Grid[i].emplace_back(Map.MinVal);
        }
    }

    struct {
        std::vector<std::pair<unsigned long int, unsigned long int>> Nodes;
        double MaxUp = 20.0, MaxDown = 25.0;
    } Pathfinder;

    std::u16string dummyString;

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
                        MouseState.Motion = true;
                        SDL_GetMouseState(&MouseState.PosR.x, &MouseState.PosR.y);
                        MouseState.PosC.x = MouseState.PosR.x - Window.getW_2();
                        MouseState.PosC.y = Window.getH() - MouseState.PosR.y - Window.getH_2();

                        if (!MouseState.Released) {
                            MouseState.Rel.x = Event.motion.xrel;
                            MouseState.Rel.y = -Event.motion.yrel;
                        } else {
                            MouseState.Released = false;
                            MouseState.Rel = {0, 0};
                        }

                        Map.Pos = {(MouseState.PosR.x - Map.Offset.x) / Map.CellSize, (MouseState.PosR.y - Map.Offset.y) / Map.CellSize};
                        break;
                    case SDL_KEYDOWN:
                        if (!Event.key.repeat) {
                            if (Keystate[Keybinds.ToggleCapture]) {
                                MouseState.Captured = !MouseState.Captured;
                                if (MouseState.Captured) {
                                    SDL_SetRelativeMouseMode(SDL_TRUE);
                                    MouseState.Released = true;
                                    MouseState.Rel = {0, 0};
                                } else {
                                    SDL_SetRelativeMouseMode(SDL_FALSE);
                                    Window.centerMouse();
                                    MouseState.PosC = {0, 0};
                                    MouseState.PosR = {0, 0};
                                }
                            }
                            else if (Keystate[Keybinds.Pathfind]) {
                                Pathfinder.Nodes = AStar.euclidean(Map.Grid, Map.Start, Map.Goal, Pathfinder.MaxUp, Pathfinder.MaxDown, ASTAR_MOVE_NOBOUND);
                                if (Pathfinder.Nodes.size() <= 1) {std::cout << "[Pathfinding] No path found\n";}
                                else {std::cout << "[Path] Path found\n";}
                                madeChanges = true;
                            }
                            else if (Keystate[Keybinds.ClearMaze]) {
                                for (unsigned long int i = 0; i < Map.Grid.size(); i++) {
                                    for (unsigned long int j = 0; j < Map.Grid.at(i).size(); j++) {
                                        Map.Grid[i][j] = Map.MinVal;
                                    }
                                }
                                std::cout << "[Grid] Grid cleared\n";
                                madeChanges = true;
                            }
                            else if (Keystate[Keybinds.PlaceStart]) {
                                Map.Start = std::make_pair(Map.Pos.y, Map.Pos.x);
                                std::cout << "[Grid] Start moved to " << Map.Start.first << ", " << Map.Start.second << "\n";
                                madeChanges = true;
                            }
                            else if (Keystate[Keybinds.PlaceGoal]) {
                                Map.Goal = std::make_pair(Map.Pos.y, Map.Pos.x);
                                std::cout << "[Grid] Goal moved to " << Map.Goal.first << ", " << Map.Goal.second << "\n";
                                madeChanges = true;
                            }
                            else if (Keystate[Keybinds.HardBrush]) {
                                Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, Map.MaxVal, Tool.Radius, Map.MaxVal, Map.MinVal);
                                madeChanges = true;
                            }
                            else if (Keystate[Keybinds.HardErase]) {
                                Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, -Map.MaxVal, Tool.Radius, Map.MaxVal, Map.MinVal);
                                madeChanges = true;
                            }
                            else if (Keystate[SDL_SCANCODE_Q]) {
                                Tool.Radius += 1.0;
                                if (Tool.Radius > Tool.RadiusMax) {Tool.Radius = Tool.RadiusMax;}
                                std::cout << "[Tool] Increased radius - now " << Tool.Radius << "\n";
                                madeChanges = true;
                            }
                            else if (Keystate[SDL_SCANCODE_W]) {
                                Tool.Radius -= 1.0;
                                if (Tool.Radius < Tool.RadiusMin) {Tool.Radius = Tool.RadiusMin;}
                                std::cout << "[Tool] Decreased radius - now " << Tool.Radius << "\n";
                                madeChanges = true;
                            }
                            else if (Keystate[SDL_SCANCODE_A]) {
                                Tool.Strength += 1.0;
                                if (Tool.Strength > Tool.StrengthMax) {Tool.Strength = Tool.StrengthMax;}
                                std::cout << "[Tool] Increased strength - now " << Tool.Strength << "\n";
                                madeChanges = true;
                            }
                            else if (Keystate[SDL_SCANCODE_S]) {
                                Tool.Strength -= 1.0;
                                if (Tool.Strength < Tool.StrengthMin) {Tool.Strength = Tool.StrengthMin;}
                                std::cout << "[Tool] Decreased strength - now " << Tool.Strength << "\n";
                                madeChanges = true;
                            }
                        }
                        break;
                    case SDL_WINDOWEVENT:
                        Window.handleEvent(Event.window);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        MouseState.Pressed[Event.button.button] = true;
                        switch (Event.button.button) {
                            case SDL_BUTTON_LEFT:
                                Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, Tool.Strength, Tool.Radius, Map.MaxVal, Map.MinVal);
                                madeChanges = true;
                                break;
                            case SDL_BUTTON_RIGHT:
                                Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, -Tool.Strength, Tool.Radius, Map.MaxVal, Map.MinVal);
                                madeChanges = true;
                                break;
                        }
                        break;
                    case SDL_MOUSEBUTTONUP:
                        MouseState.Pressed[Event.button.button] = false;
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

                if (MouseState.Pressed[SDL_BUTTON_LEFT]) {
                    Map.Grid = brushGrid(Map.Grid, Map.Pos.y, Map.Pos.x, Tool.Strength, Tool.Radius, Map.MaxVal, Map.MinVal);
                    madeChanges = true;
                } else if (MouseState.Pressed[SDL_BUTTON_RIGHT]) {
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
            MouseState.Motion = false;
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
            Window.fillRectangle(-556, -295, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-556, -295,   5, 45, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-556, -335, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-352, -295,   5, 45, PresetColors[COLOR_WHITE]);

            Window.fillRectangle(-310, -295, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-310, -295,   5, 45, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-310, -335, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-106, -295,   5, 45, PresetColors[COLOR_WHITE]);

            Window.fillRectangle(-64, -295, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-64, -295,   5, 45, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(-64, -335, 209,  5, PresetColors[COLOR_WHITE]);
            Window.fillRectangle(140, -295,   5, 45, PresetColors[COLOR_WHITE]);

            // Pathfinding text
            Window.renderText(font, u"Generate Path", {-453, -318}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, u"Place Start", {-207, -318}, 0, PresetColors[COLOR_WHITE]);
            Window.renderText(font, u"Place Goal", {40, -318}, 0, PresetColors[COLOR_WHITE]);

            // Path Settings text
            Window.renderText(font, u"Path Settings:", {373, 279}, 0, PresetColors[COLOR_WHITE]);
            dummyString = u"Up:   " + btils::to_u16string<std::string>(btils::tstr_AddZeros<int>(Pathfinder.MaxUp, 4, 3, false));
            Window.renderText(font, dummyString.c_str(), {373, 232}, 0, PresetColors[COLOR_WHITE]);
            dummyString = u"Down: " + btils::to_u16string<std::string>(btils::tstr_AddZeros<int>(Pathfinder.MaxDown, 4, 3, false));
            Window.renderText(font, dummyString.c_str(), {373, 165}, 0, PresetColors[COLOR_WHITE]);

            // Grid Settings text
            Window.renderText(font, u"Grid Settings:", {373, 95}, 0, PresetColors[COLOR_WHITE]);
            dummyString = u"Cell Size: " + btils::to_u16string<std::string>(btils::tstr_Length<int>(Map.CellSize, 3, false, false));
            Window.renderText(font, dummyString.c_str(), {373, 48}, 0, PresetColors[COLOR_WHITE]);
            dummyString = u"Min:  " + btils::to_u16string<std::string>(btils::tstr_AddZeros<int>(Map.MinVal, 4, 3, false));
            Window.renderText(font, dummyString.c_str(), {373, -20}, 0, PresetColors[COLOR_WHITE]);
            dummyString = u"Max:  " + btils::to_u16string<std::string>(btils::tstr_AddZeros<int>(Map.MaxVal, 4, 3, false));
            Window.renderText(font, dummyString.c_str(), {373, -87}, 0, PresetColors[COLOR_WHITE]);

            // Tool Settings text
            Window.renderText(font, u"Tool Settings:", {373, -157}, 0, PresetColors[COLOR_WHITE]);
            dummyString = u"Size:      " + btils::to_u16string<std::string>(btils::tstr_Length<int>(Tool.Radius, 3, false, false));
            Window.renderText(font, dummyString.c_str(), {373, -204}, 0, PresetColors[COLOR_WHITE]);
            dummyString = u"Strength:  " + btils::to_u16string<std::string>(btils::tstr_Length<int>(Tool.Strength, 3, false, false));
            Window.renderText(font, dummyString.c_str(), {373, -271}, 0, PresetColors[COLOR_WHITE]);

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
                    Window.fillRectangle(-Window.getW_2() + j * Map.CellSize + Map.Offset.x, Window.getH_2() - i * Map.CellSize - Map.Offset.y, Map.CellSize, Map.CellSize, {shade, shade, shade, 255});
                }
            }

            // Path
            Window.fillRectangle(-Window.getW_2() + Map.Start.second * Map.CellSize + Map.Offset.x, Window.getH_2() - Map.Start.first * Map.CellSize - Map.Offset.y, Map.CellSize, Map.CellSize, PresetColors[COLOR_TEAL]);
            Window.fillRectangle(-Window.getW_2() +  Map.Goal.second * Map.CellSize + Map.Offset.x, Window.getH_2() -  Map.Goal.first * Map.CellSize - Map.Offset.y, Map.CellSize, Map.CellSize, PresetColors[COLOR_MAROON]);
            for (unsigned long int i = 1; i < Pathfinder.Nodes.size(); i++) {
                Window.drawLine(-Window.getW_2() + Map.CellSize / 2 + Pathfinder.Nodes.at(i - 1).second * Map.CellSize + Map.Offset.x, Window.getH_2() - Map.CellSize / 2 - Pathfinder.Nodes.at(i - 1).first  * Map.CellSize - Map.Offset.y, -Window.getW_2() + Map.CellSize / 2 + Pathfinder.Nodes.at(i).second * Map.CellSize + Map.Offset.x, Window.getH_2() - Map.CellSize / 2 - Pathfinder.Nodes.at(i).first * Map.CellSize - Map.Offset.y, PresetColors[COLOR_LIME]);
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
