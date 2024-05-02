#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "RenderWindow.hpp"
#include "Utilities.hpp"

#include "Perlin.hpp"
#include "AStar.hpp"

std::vector<std::vector<double>> getNoise(int w, int h, int o = 8, double bias = 2.0, double scale = 350.0) {
    std::vector<std::vector<double>> output;

    for (int i = 0; i < h; i++) {
        output.emplace_back();
        for (int j = 0; j < w; j++) {
            double val = 0.0, freq = 1.0, amp = 1.0;

            for (int k = 0; k < o; k++) {
                val += perlin(i * freq / scale, j * freq / scale) * amp;

                freq *= bias;
                amp /= bias;
            }

            if (val > 1.0) {val = 1.0;}
            else if (val < -1.0) {val = -1.0;}

            output[i].emplace_back(val * 0.5 + 0.5);
        }
    }

    return output;
}

// SDL_Texture* getNoiseTexture(int w, int h, int o = 8, double bias = 2.0, double scale = 350.0) {
//     std::vector<std::vector<Uint8>> noise = getNoise(w, h, o, bias, scale);
//     SDL_Surface* surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
//     SDL_LockSurface(surface);
//     Uint8* pixels = (Uint8*)surface->pixels;
//     for (unsigned long int i = 0; i < noise.size(); i++) {
//         for (unsigned long int j = 0; j < noise[0].size(); j++) {
//             const unsigned char val = (unsigned char)((noise[i][j] * 0.5 + 0.5) * 255.0);
//             pixels[i * surface->pitch + j * surface->format->BytesPerPixel + 0] = val;
//             pixels[i * surface->pitch + j * surface->format->BytesPerPixel + 1] = val;
//             pixels[i * surface->pitch + j * surface->format->BytesPerPixel + 2] = val;
//         }
//     }
//     SDL_UnlockSurface(surface);
//     SDL_SaveBMP(surface, "dev/map.bmp");
//     return nullptr;
// }

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

const std::vector<unsigned char> tilesetVals = {47, 2, 8, 10, 11, 16, 18, 22, 24, 26, 27, 30, 31, 64, 66, 72, 74, 75, 80, 82, 86, 88, 90, 91, 94, 95, 104, 106, 107, 120, 122, 123, 126, 127, 208, 210, 214, 216, 218, 219, 222, 223, 248, 250, 251, 254, 255, 0};
// const std::vector<std::pair<unsigned char, unsigned char>> a = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {3, 0}, {3, 1}, {3, 2}, {3, 3}};

std::vector<std::vector<unsigned char>> grid2TileIndexes(const std::vector<std::vector<double>> &grid, const double &cutoffVal = 1.0) {
    std::vector<std::vector<unsigned char>> output;
    unsigned char cellSum = 0;
    for (unsigned long int i = 0; i < grid.size(); i++) {
        output.emplace_back();
        for (unsigned long int j = 0; j < grid.at(i).size(); j++) {
            if (i > 0) {
                cellSum += grid.at(i - 1).at(j) >= cutoffVal ? 2 : 0;
                if (j > 0) {
                    cellSum += grid.at(i - 1).at(j - 1) >= cutoffVal ? 1 : 0;
                }
                if (j < grid.at(i).size() - 1) {
                    cellSum += grid.at(i - 1).at(j + 1) >= cutoffVal ? 4 : 0;
                }
            }
            if (j > 0) {
                cellSum += grid.at(i).at(j - 1) >= cutoffVal ? 8 : 0;
            }
            if (j < grid.at(i).size() - 1) {
                cellSum += grid.at(i).at(j + 1) >= cutoffVal ? 16 : 0;
            }
            if (i < grid.size() - 1) {
                cellSum += grid.at(i + 1).at(j) >= cutoffVal ? 64 : 0;
                if (j > 0) {
                    cellSum += grid.at(i + 1).at(j - 1) >= cutoffVal ? 32 : 0;
                }
                if (j < grid.at(i).size() - 1) {
                    cellSum += grid.at(i + 1).at(j + 1) >= cutoffVal ? 128 : 0;
                }
            }
            for (unsigned long int k = 0; k < tilesetVals.size(); k++) {
                if (tilesetVals.at(k) == cellSum) {
                    output.emplace_back(tilesetVals.at(k));
                    std::cout << (int)tilesetVals.at(k) << " ";
                    break;
                }
            }
            cellSum = 0;
        }
        std::cout << "\n";
    }
    return output;
}
std::pair<unsigned char, unsigned char> getTextureLocation(const unsigned char &index) {return std::make_pair(index % 6, index / 6 % 8);}

double HireTime_Sec() {return SDL_GetTicks() * 0.01f;}
int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {std::cout << "Error initializing SDL2\nERROR: " << SDL_GetError() << "\n";}
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {std::cout << "Error initializing SDL2_image\nERROR: " << IMG_GetError() << "\n";}
    if (TTF_Init() == -1) {std::cout << "Error initializing SDL2_ttf\nERROR: " << TTF_GetError() << "\n";}

    RenderWindow Window("Trailblazer", 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY);
    // RenderWindow Window("Trailblazer", 1260, 630, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY);
    SDL_Event Event;
    const Uint8 *Keystate = SDL_GetKeyboardState(NULL);

    TTF_Font *font = TTF_OpenFont("dev/fonts/GNU-Unifont.ttf", 20);

    struct {
        SDL_Point PosC = {0, 0};
        SDL_Point PosR = {0, 0};
        SDL_Point Rel = {0, 0};
        double Sensitivity = 10;

        bool Motion = false;
        bool Released = true;
        bool Captured = false;

        bool Pressed[5] = {false, false, false, false, false};
    } MouseInfo;

    struct {
        int Quit = SDL_SCANCODE_ESCAPE;
        int ToggleCapture = SDL_SCANCODE_F1;
        int AStar_Cardinal = SDL_SCANCODE_1;
        int AStar_Compass_Free = SDL_SCANCODE_2;
        int AStar_General_Free = SDL_SCANCODE_3;
        int AStar_Compass_NoPhase = SDL_SCANCODE_4;
        int AStar_General_NoPhase = SDL_SCANCODE_5;
        int AStar_Compass_NoTouch = SDL_SCANCODE_6;
        int AStar_General_NoTouch = SDL_SCANCODE_7;
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

    // std::vector<std::vector<double>> Noise = getNoise(Window.getW(), Window.getH());

    // Window.clear();
    // for (unsigned long int i = 0; i < Noise.size(); i++) {
    //     for (unsigned long int j = 0; j < Noise[0].size(); j++) {
    //         Window.drawPixel(-Window.getW_2() + j, Window.getH_2() - i, {Noise[i][j], Noise[i][j], Noise[i][j], 255});
    //     }
    // }
    // Window.renderText(font, u"hola", {0, 0});
    // Window.show();

    struct {
        double Strength = 1.0;
        double StrengthMax = 5.0;
        double StrengthMin = 1.0;
        int Radius = 0;
        int RadiusMax = 5;
        int RadiusMin = 0;
    } Stroke;

    struct {
        // For 1280x720, should be 1, 2, 4, 5, 8, 10, 16, 20, 40, or 80
        int Size = 20;
        double MaxVal = 20.0;
        double MinVal = 0.0;
    } GridCell;

    SDL_Point mousePosMaze = {0, 0}, prevMousePosMaze = mousePosMaze;
    double maxAscent = 2.0, maxDescent = 3.0;
    
    std::vector<std::vector<double>> grid;
    std::pair<unsigned long int, unsigned long int> start = std::make_pair(0, 0), goal = std::make_pair(Window.getH() / GridCell.Size - 1, Window.getW() / GridCell.Size - 1);
    std::vector<std::pair<unsigned long int, unsigned long int>> pathNodes;
    bool madeChanges = true;

    for (int i = 0; i < Window.getH() / GridCell.Size; i++) {
        grid.emplace_back();
        for (int j = 0; j < Window.getW() / GridCell.Size; j++) {
            grid[i].emplace_back(GridCell.MinVal);
        }
    }

    // grid = {
    //     // {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
    //     // {0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
    //     // {0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
    //     // {0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    //     // {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0},
    //     // {0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0},
    //     // {0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0},
    //     // {1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0},
    //     // {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
    //     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    //     {0.0, 0.0, 1.0, 0.0, 0.0, 0.0},
    //     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
    // };
    // SDL_Texture *tileset = Window.loadTexture("dev/png/tilesetCompass.png");
    // std::vector<SDL_Rect> tileFrames;
    // std::vector<std::vector<unsigned char>> tileIndexes = grid2TileIndexes(grid, Stroke.StrengthMax);
    // for (unsigned long int i = 0; i < tileIndexes.size(); i++) {
    //     for (unsigned long int j = 0; j < tileIndexes.at(i).size(); j++) {
            
    //         std::pair<unsigned char, unsigned char> tilePos = getTextureLocation(tileIndexes.at(i).at(j));
    //         // std::cout << (int)tilePos.first << " " << (int)tilePos.second << "\n";
    //         tileFrames.emplace_back();
    //         tileFrames.at(tileFrames.size() - 1).x = tilePos.first * 16;
    //         tileFrames.at(tileFrames.size() - 1).y = tilePos.second * 16;
    //         tileFrames.at(tileFrames.size() - 1).h = 16;
    //         tileFrames.at(tileFrames.size() - 1).w = 16;
    //     }
    // }

    bool running = true;
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
                        MouseInfo.Motion = true;
                        SDL_GetMouseState(&MouseInfo.PosR.x, &MouseInfo.PosR.y);
                        MouseInfo.PosC.x = MouseInfo.PosR.x - Window.getW_2();
                        MouseInfo.PosC.y = Window.getH() - MouseInfo.PosR.y - Window.getH_2();

                        if (!MouseInfo.Released) {
                            MouseInfo.Rel.x = Event.motion.xrel;
                            MouseInfo.Rel.y = -Event.motion.yrel;
                        } else {
                            MouseInfo.Released = false;
                            MouseInfo.Rel = {0, 0};
                        }

                        mousePosMaze = {MouseInfo.PosR.x / GridCell.Size, MouseInfo.PosR.y / GridCell.Size};
                        break;
                    case SDL_KEYDOWN:
                        if (!Event.key.repeat) {
                            if (Keystate[Keybinds.ToggleCapture]) {
                                MouseInfo.Captured = !MouseInfo.Captured;
                                if (MouseInfo.Captured) {
                                    SDL_SetRelativeMouseMode(SDL_TRUE);
                                    MouseInfo.Released = true;
                                    MouseInfo.Rel = {0, 0};
                                } else {
                                    SDL_SetRelativeMouseMode(SDL_FALSE);
                                    Window.centerMouse();
                                    MouseInfo.PosC = {0, 0};
                                    MouseInfo.PosR = {0, 0};
                                }
                            }
                            if (Keystate[Keybinds.AStar_Cardinal]) {
                                madeChanges = true;
                                pathNodes = AStar.cardinal(grid, start, goal, maxAscent, maxDescent);
                                std::cout << "Cardinal\n";
                            }
                            if (Keystate[Keybinds.AStar_Compass_Free]) {
                                madeChanges = true;
                                pathNodes = AStar.diagonal(grid, start, goal, maxAscent, maxDescent, ASTAR_MOVE_NOBOUND);
                                std::cout << "Diagonal - No Bounds\n";
                            }
                            if (Keystate[Keybinds.AStar_General_Free]) {
                                madeChanges = true;
                                pathNodes = AStar.euclidean(grid, start, goal, maxAscent, maxDescent, ASTAR_MOVE_NOBOUND);
                                std::cout << "Euclidean - No Bounds\n";
                            }
                            if (Keystate[Keybinds.AStar_Compass_NoPhase]) {
                                madeChanges = true;
                                pathNodes = AStar.diagonal(grid, start, goal, maxAscent, maxDescent, ASTAR_MOVE_NOPHASE);
                                std::cout << "Diagonal - No Phasing\n";
                            }
                            if (Keystate[Keybinds.AStar_General_NoPhase]) {
                                madeChanges = true;
                                pathNodes = AStar.euclidean(grid, start, goal, maxAscent, maxDescent, ASTAR_MOVE_NOPHASE);
                                std::cout << "Euclidean - No Phasing\n";
                            }
                            if (Keystate[Keybinds.AStar_Compass_NoTouch]) {
                                madeChanges = true;
                                pathNodes = AStar.diagonal(grid, start, goal, maxAscent, maxDescent, ASTAR_MOVE_NOTOUCH);
                                std::cout << "Diagonal - No Touching\n";
                            }
                            if (Keystate[Keybinds.AStar_General_NoTouch]) {
                                madeChanges = true;
                                pathNodes = AStar.diagonal(grid, start, goal, maxAscent, maxDescent, ASTAR_MOVE_NOTOUCH);
                                std::cout << "Euclidean - No Touching\n";
                            }
                            if (Keystate[Keybinds.ClearMaze]) {
                                for (unsigned long int i = 0; i < grid.size(); i++) {
                                    for (unsigned long int j = 0; j < grid.at(i).size(); j++) {
                                        grid[i][j] = GridCell.MinVal;
                                    }
                                }
                                std::cout << "Grid cleared\n";
                                madeChanges = true;
                            }
                            if (Keystate[Keybinds.PlaceStart]) {
                                start = std::make_pair(MouseInfo.PosR.y / GridCell.Size, MouseInfo.PosR.x / GridCell.Size);
                                std::cout << "Start moved to " << start.first << ", " << start.second << "\n";
                                madeChanges = true;
                            }
                            if (Keystate[Keybinds.PlaceGoal]) {
                                goal = std::make_pair(MouseInfo.PosR.y / GridCell.Size, MouseInfo.PosR.x / GridCell.Size);
                                std::cout << "Goal moved to " << goal.first << ", " << goal.second << "\n";
                                madeChanges = true;
                            }
                            if (Keystate[Keybinds.HardBrush]) {
                                grid = brushGrid(grid, MouseInfo.PosR.y / GridCell.Size, MouseInfo.PosR.x / GridCell.Size, GridCell.MaxVal, Stroke.Radius, GridCell.MaxVal, GridCell.MinVal);
                                madeChanges = true;
                            }
                            if (Keystate[Keybinds.HardErase]) {
                                grid = brushGrid(grid, MouseInfo.PosR.y / GridCell.Size, MouseInfo.PosR.x / GridCell.Size, -GridCell.MaxVal, Stroke.Radius, GridCell.MaxVal, GridCell.MinVal);
                                madeChanges = true;
                            }
                            if (Keystate[SDL_SCANCODE_Q]) {
                                Stroke.Radius += 1.0;
                                if (Stroke.Radius > Stroke.RadiusMax) {Stroke.Radius = Stroke.RadiusMax;}
                                std::cout << "Increased stroke radius, now: " << Stroke.Radius << "\n";
                            }
                            if (Keystate[SDL_SCANCODE_W]) {
                                Stroke.Radius -= 1.0;
                                if (Stroke.Radius < Stroke.RadiusMin) {Stroke.Radius = Stroke.RadiusMin;}
                                std::cout << "Decreased stroke radius, now: " << Stroke.Radius << "\n";
                            }
                            if (Keystate[SDL_SCANCODE_A]) {
                                Stroke.Strength += 1.0;
                                if (Stroke.Strength > Stroke.StrengthMax) {Stroke.Strength = Stroke.StrengthMax;}
                                std::cout << "Increased stroke strength, now: " << Stroke.Strength << "\n";
                            }
                            if (Keystate[SDL_SCANCODE_S]) {
                                Stroke.Strength -= 1.0;
                                if (Stroke.Strength < Stroke.StrengthMin) {Stroke.Strength = Stroke.StrengthMin;}
                                std::cout << "Decreased stroke strength, now: " << Stroke.Strength << "\n";
                            }
                        }
                        break;
                    case SDL_WINDOWEVENT:
                        Window.handleEvent(Event.window);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        MouseInfo.Pressed[Event.button.button] = true;
                        switch (Event.button.button) {
                            case SDL_BUTTON_LEFT:
                                grid = brushGrid(grid, MouseInfo.PosR.y / GridCell.Size, MouseInfo.PosR.x / GridCell.Size, Stroke.Strength, Stroke.Radius, GridCell.MaxVal, GridCell.MinVal);
                                madeChanges = true;
                                break;
                            case SDL_BUTTON_RIGHT:
                                grid = brushGrid(grid, MouseInfo.PosR.y / GridCell.Size, MouseInfo.PosR.x / GridCell.Size, -Stroke.Strength, Stroke.Radius, GridCell.MaxVal, GridCell.MinVal);
                                madeChanges = true;
                                break;
                        }
                        break;
                    case SDL_MOUSEBUTTONUP:
                        MouseInfo.Pressed[Event.button.button] = false;
                        break;
                }
                if (!running) {break;}
            }
            if (!running || Keystate[Keybinds.Quit]) {
                running = false;
                break;
            }

            if ((mousePosMaze.x != prevMousePosMaze.x) || (mousePosMaze.y != prevMousePosMaze.y)) {
                prevMousePosMaze = mousePosMaze;

                if (MouseInfo.Pressed[SDL_BUTTON_LEFT]) {
                    grid = brushGrid(grid, MouseInfo.PosR.y / GridCell.Size, MouseInfo.PosR.x / GridCell.Size, Stroke.Strength, Stroke.Radius, GridCell.MaxVal, GridCell.MinVal);
                    madeChanges = true;
                } else if (MouseInfo.Pressed[SDL_BUTTON_RIGHT]) {
                    grid = brushGrid(grid, MouseInfo.PosR.y / GridCell.Size, MouseInfo.PosR.x / GridCell.Size, -Stroke.Strength, Stroke.Radius, GridCell.MaxVal, GridCell.MinVal);
                    madeChanges = true;
                }
                if (Keystate[Keybinds.HardBrush]) {
                    grid = brushGrid(grid, MouseInfo.PosR.y / GridCell.Size, MouseInfo.PosR.x / GridCell.Size, GridCell.MaxVal, Stroke.Radius, GridCell.MaxVal, GridCell.MinVal);
                    madeChanges = true;
                }
                if (Keystate[Keybinds.HardErase]) {
                    grid = brushGrid(grid, MouseInfo.PosR.y / GridCell.Size, MouseInfo.PosR.x / GridCell.Size, -GridCell.MaxVal, Stroke.Radius, GridCell.MaxVal, GridCell.MinVal);
                    madeChanges = true;
                }
            }

            t += dt;
            accumulator -= dt;
            MouseInfo.Motion = false;
        }
        if (!running) {break;}

        if (madeChanges) {
            madeChanges = false;
            Window.clear();

            for (unsigned long int i = 0; i < grid.size(); i++) {
                for (unsigned long int j = 0; j < grid.at(i).size(); j++) {
                    const unsigned char shade = 255 - btils::map<double, unsigned char>(grid.at(i).at(j), GridCell.MinVal, GridCell.MaxVal, 0, 255);
                    Window.fillRectangle(-Window.getW_2() + j * GridCell.Size, Window.getH_2() - i * GridCell.Size, GridCell.Size, GridCell.Size, {shade, shade, shade, 255});
                    Window.drawRectangle(-Window.getW_2() + j * GridCell.Size, Window.getH_2() - i * GridCell.Size, GridCell.Size, GridCell.Size, PresetColors[COLOR_LIGHT_GRAY]);

                    // const SDL_Rect tilesetDst = {-Window.getW_2() + (int)j * GridCell.Size, Window.getH_2() - (int)i * GridCell.Size, GridCell.Size, GridCell.Size};
                    // Window.renderTexture(tileset, tileFrames.at(i * 8 + j), tilesetDst);
                }
            }
            Window.fillRectangle(-Window.getW_2() + start.second * GridCell.Size, Window.getH_2() - start.first * GridCell.Size, GridCell.Size, GridCell.Size, PresetColors[COLOR_TEAL]);
            Window.fillRectangle(-Window.getW_2() +  goal.second * GridCell.Size, Window.getH_2() -  goal.first * GridCell.Size, GridCell.Size, GridCell.Size, PresetColors[COLOR_MAROON]);

            for (unsigned long int i = 1; i < pathNodes.size(); i++) {
                Window.drawLine(-Window.getW_2() + GridCell.Size / 2 + pathNodes.at(i - 1).second * GridCell.Size, Window.getH_2() - GridCell.Size / 2 - pathNodes.at(i - 1).first  * GridCell.Size, -Window.getW_2() + GridCell.Size / 2 + pathNodes.at(i).second * GridCell.Size, Window.getH_2() - GridCell.Size / 2 - pathNodes.at(i).first * GridCell.Size, PresetColors[COLOR_LIME]);
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
