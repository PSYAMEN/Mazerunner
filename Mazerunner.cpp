#include "MiniFB.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <time.h>
using namespace std;

const int TAILLE = 101;
const int SQUARE = 700;
const int PIXEL = 91;
const int L = 7;
const int LPIXEL = PIXEL / L;

class CMyWin : public CFBWindow {
public:
    CMyWin() : CFBWindow(L"Mazerunner", SQUARE, SQUARE, 0, PIXEL, PIXEL) {}
    void resize(int width, int height) {}
    int direction = 4, keys = 0;
    void keyboard(int key, uint32_t mod, bool isPressed)
    {
        if (!isPressed) return;
        //printf("key %d\r\n", key); 
        if (key == 328 || key == 17) direction = 2;
        if (key == 336 || key == 31) direction = 0;
        if (key == 333 || key == 32) direction = 1;
        if (key == 331 || key == 30) direction = 3;
        if (key == 28) direction = 5;
    }

    void cube(int x, int y, TSFBColor color)
    {
        rect(x * LPIXEL, y * LPIXEL, 100 * PIXEL / SQUARE, 100 * PIXEL / SQUARE, color);
    }

    struct dude {
        int x;
        int y;
        int orientation = 0;
        int score = 0;
        bool death = false;
        int battery = 450;
    };

    struct tile {
        bool wall = false;
        bool visited = false;
        bool trap = false;
        bool mimic = false;
        bool chest = false;
        bool boost = false;
        bool lightup = false;
        bool open = false;// boost resemble a un coffre mais au lieu de donner des point de score donen une agramdissement de la lampe torche
    };

    struct maze {
        int start[2];
        tile board[TAILLE][TAILLE];
    };

    void Exit(maze& game, int dimention)
    {
        int c1 = rand() % 4;
        int c2 = ((rand() % (dimention - 4)) & ~1) + 2;
        switch (c1)
        {
        case 0:
            game.board[c2][1].wall = 0;
            break;
        case 1:
            game.board[dimention - 2][c2].wall = 0;
            break;
        case 2:
            game.board[c2][dimention - 2].wall = 0;
            break;
        case 3:
            game.board[1][c2].wall = 0;
            break;
        }
    }

    int WhereJump(maze& game, int x, int y)
    {
        bool WJ[4] = { false };
        int HowManyJump = 0;
        game.board[x][y].visited = true;
        if (!game.board[x][y + 2].visited) { HowManyJump++; WJ[0] = true; }
        if (!game.board[x - 2][y].visited) { HowManyJump++; WJ[3] = true; }          if (!game.board[x + 2][y].visited) { HowManyJump++; WJ[1] = true; }
        if (!game.board[x][y - 2].visited) { HowManyJump++; WJ[2] = true; }
        if (HowManyJump == 0) return -1;
        while (true) { int dir = rand() % 4; if (WJ[dir]) return dir; }
    }

    void Jump(maze& game, int& x, int& y, int dir)
    {
        switch (dir)
        {
        case 0:
            game.board[x][y + 1].wall = 0;
            y += 2;
            break;
        case 1:
            game.board[x + 1][y].wall = 0;
            x += 2;
            break;
        case 2:
            game.board[x][y - 1].wall = 0;
            y -= 2;
            break;
        case 3:
            game.board[x - 1][y].wall = 0;
            x -= 2;
            break;
        default:
            break;
        }
    }

    void PlaceStuff(maze& game, int size)
    {
        for (int i = 0; i < (size - 2) / 5 + rand() % 3; i++)
        {
            int x = ((rand() % (size - 4)) & ~1) + 2;
            int y = ((rand() % (size - 4)) & ~1) + 2;
            int r = 0;
            while (!game.board[x][y].open)
            {

                r = rand() % 2;
                if (r == 0) { game.board[x][y].chest = true; }
                else { game.board[x][y].mimic = true; }
                game.board[x][y].open = true;
                x = ((rand() % (size - 4)) & ~1) + 2;
                y = ((rand() % (size - 4)) & ~1) + 2;
            }
        }
        for (int i = 0; i < (size - 2) / 5 - rand() % 3; i++)
        {
            int x = ((rand() % (size - 4)) & ~1) + 2;
            int y = ((rand() % (size - 4)) & ~1) + 2;
            while (!game.board[x][y].open)
            {
                game.board[x][y].trap = true;
                game.board[x][y].open = true;
                x = ((rand() % (size - 4)) & ~1) + 2;
                y = ((rand() % (size - 4)) & ~1) + 2;
            }
        }
        for (int i = 0; i < (size - 2) / 10 + rand() % 2; i++)
        {
            int x = ((rand() % (size - 4)) & ~1) + 2;
            int y = ((rand() % (size - 4)) & ~1) + 2;
            while (!game.board[x][y].open)
            {
                game.board[x][y].boost = true;
                game.board[x][y].open = true;
                x = ((rand() % (size - 4)) & ~1) + 2;
                y = ((rand() % (size - 4)) & ~1) + 2;
            }
        }
        /*for (int i = 0; i < (size - 2) / 5 + rand() % 2; i++)
        {
            int x = ((rand() % (size - 4)) & ~1) + 2;
            int y = ((rand() % (size - 4)) & ~1) + 2;
            while (!game.board[x][y].open)
            {
                game.board[x][y].lightup = true;
                game.board[x][y].open = true;
                x = ((rand() % (size - 4)) & ~1) + 2;
                y = ((rand() % (size - 4)) & ~1) + 2;
            }
        }*/
    }

    void BreakWall(maze& game, int dimention)
    {
        for (int i = 0; i < dimention * dimention / 20; i++)
        {
            int dir = rand() % 4;
            int c1 = ((rand() % (dimention - 4)) & ~1) + 2;
            int c2 = ((rand() % (dimention - 4)) & ~1) + 2;
            Jump(game, c1, c2, dir);
        }
    }

    void CreateMaze(maze& game, int dimention)
    {
        for (int i = 1; i < dimention; i += 2)
        {
            for (int j = 1; j < dimention; j++)
            {
                game.board[i][j].wall = game.board[j][i].wall = 1;
            }
        }
        for (int i = 0; i < dimention; i++)
        {
            game.board[0][i].visited = game.board[i][dimention - 1].visited = game.board[dimention - 1][i].visited = game.board[i][0].visited = 1;
        }
        int stage = 0;
        int stack[TAILLE * TAILLE][2];
        stack[0][0] = ((rand() % (dimention - 4)) & ~1) + 2;
        stack[0][1] = ((rand() % (dimention - 4)) & ~1) + 2;
        do
        {
            int x = stack[stage][0];
            int y = stack[stage][1];
            int dir = WhereJump(game, x, y);
            if (dir != -1)
            {
                Jump(game, x, y, dir);
                stage++;
                stack[stage][0] = x;
                stack[stage][1] = y;
            }
            else { stage--; }
        } while (stage != 0);
        game.start[0] = ((rand() % (dimention - 4)) & ~1) + 2;
        game.start[1] = ((rand() % (dimention - 4)) & ~1) + 2;
        game.board[game.start[0]][game.start[1]].open = true;
        PlaceStuff(game, dimention);
        BreakWall(game, dimention);
        for (int i = 0; i < dimention / 10; i++)
        {
            Exit(game, dimention);
        }
    }

    bool move(dude& player, maze game)
    {
        switch (direction)
        {
        case 0:
            if (!game.board[player.x][player.y + 1].wall) player.y++;
            player.orientation = direction;
            break;
        case 1:
            if (!game.board[player.x + 1][player.y].wall) player.x++;
            player.orientation = direction;
            break;
        case 2:
            if (!game.board[player.x][player.y - 1].wall) player.y--;
            player.orientation = direction;
            break;
        case 3:
            if (!game.board[player.x - 1][player.y].wall) player.x--;
            player.orientation = direction;
            break;
        default: return false;
        }
        return true;
    }

    void flashlight0(int boost)
    {
        int x;
        x = 3.5 * LPIXEL;
        int color = 96;
        for (int i = 3.5 * LPIXEL + 1; i < 4 * LPIXEL; i++)
        {
            color -= 3;
            if (boost != 0) color += 1;
            int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
            line(i, i, 7 * LPIXEL - i - 1, i, newcolor);
        }
        if (pixel(3.5 * LPIXEL + 1, 4 * LPIXEL + 1) != 0x000000) {
            for (int i = 4 * LPIXEL; i < 5 * LPIXEL; i++)
            {
                color -= 3;
                if (boost != 0) color += 1;
                int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                if (pixel(3 * LPIXEL - 1, i) != 0x000000 && pixel(4 * LPIXEL + 1, i) != 0x000000) { line(i, i, 7 * LPIXEL - i - 1, i, newcolor); }
                else {
                    if (pixel(3 * LPIXEL - 1, i) != 0x000000 && pixel(4 * LPIXEL + 1, i) == 0x000000) { line(4 * LPIXEL - 1, i, 7 * LPIXEL - i - 1, i, newcolor); }

                    else {
                        if (pixel(3 * LPIXEL - 1, i) == 0x000000 && pixel(4 * LPIXEL + 1, i) != 0x000000) { line(i, i, 3 * LPIXEL, i, newcolor); }
                        else {
                            line(3 * LPIXEL, i, 4 * LPIXEL - 1, i, newcolor);
                        }
                    }
                }
            }
            if (pixel(3.5 * LPIXEL, 5 * LPIXEL + 1) != 0x000000) {
                for (int i = 5 * LPIXEL; i < 6 * LPIXEL; i++)
                {
                    color -= 3;
                    if (boost > 0)color++;
                    int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                    if (pixel(LPIXEL * 3 - 1, i) != 0x000000 && pixel(LPIXEL * 4 + 1, i) != 0x000000) {
                        line(i / 3 + 7.33333 / 3 * LPIXEL, i, -i / 3 + 13.6666 / 3 * LPIXEL, i, newcolor);
                    }
                    else {
                        if (pixel(LPIXEL * 3 - 1, i) != 0x000000 && pixel(4 * LPIXEL + 1, i) == 0x000000) { line(4 * LPIXEL - 1, i, -i / 3 + 13.6666 / 3 * LPIXEL, i, newcolor); }

                        else {
                            if (pixel(LPIXEL * 3 - 1, i) == 0x000000 && pixel(4 * LPIXEL + 1, i) != 0x000000) { line(i / 3 + 7.33333 / 3 * LPIXEL, i, 3 * LPIXEL, i, newcolor); }
                            else {
                                line(3 * LPIXEL, i, 4 * LPIXEL - 1, i, newcolor);
                            }
                        }
                    }
                }
                if (pixel(3.5 * LPIXEL, 6 * LPIXEL + 1) != 0x000000 && boost != 0) {
                    for (int i = 6 * LPIXEL; i < 7 * LPIXEL; i++)
                    {
                        color -= 3;
                        if (boost > 0)color++;
                        int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                        if (pixel(LPIXEL * 3 - 1, i) != 0x000000 && pixel(LPIXEL * 4 + 1, i) != 0x000000) {
                            line(i / 4 + 10.0 / 4 * LPIXEL, i, -i / 4 + 17.75 / 4 * LPIXEL, i, newcolor);
                        }
                        else {
                            if (pixel(LPIXEL * 3 - 1, i) != 0x000000 && pixel(4 * LPIXEL + 1, i) == 0x000000) { line(4 * LPIXEL - 1, i, -i / 4 + 17.75 / 4 * LPIXEL, i, newcolor); }

                            else {
                                if (pixel(LPIXEL * 3 - 1, i) == 0x000000 && pixel(4 * LPIXEL + 1, i) != 0x000000) { line(i / 4 + 10.0 / 4 * LPIXEL, i, 3 * LPIXEL, i, newcolor); }
                                else {
                                    line(3 * LPIXEL, i, 4 * LPIXEL - 1, i, newcolor);
                                }
                            }
                        }
                    }
                }
            }

        }
    }

    void flashlight1(int boost)
    {
        int x;
        x = 3.5 * LPIXEL;
        int color = 96;
        for (int i = 3.5 * LPIXEL + 1; i < 4 * LPIXEL; i++)
        {
            color -= 3;
            if (boost != 0) color += 1;
            int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
            line(i, i, i, 7 * LPIXEL - i - 1, newcolor);
        }
        if (pixel(4 * LPIXEL + 1, 3.5 * LPIXEL + 1) != 0x000000) {
            for (int i = 4 * LPIXEL; i < 5 * LPIXEL; i++)
            {
                color -= 3;
                if (boost != 0) color += 1;
                int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                if (pixel(i, 3 * LPIXEL - 1) != 0x000000 && pixel(i, 4 * LPIXEL + 1) != 0x000000) { line(i, i, i, 7 * LPIXEL - i - 1, newcolor); }
                else {
                    if (pixel(i, 3 * LPIXEL - 1) != 0x000000 && pixel(i, 4 * LPIXEL + 1) == 0x000000) { line(i, 4 * LPIXEL - 1, i, 7 * LPIXEL - i - 1, newcolor); }

                    else {
                        if (pixel(i, 3 * LPIXEL - 1) == 0x000000 && pixel(i, 4 * LPIXEL + 1) != 0x000000) { line(i, i, i, 3 * LPIXEL, newcolor); }
                        else {
                            line(i, 3 * LPIXEL, i, 4 * LPIXEL - 1, newcolor);
                        }
                    }
                }
            }
            if (pixel(5 * LPIXEL + 1, 3.5 * LPIXEL) != 0x000000) {
                for (int i = 5 * LPIXEL; i < 6 * LPIXEL; i++)
                {
                    color -= 3;
                    if (boost > 0)color++;
                    int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                    if (pixel(i, LPIXEL * 3 - 1) != 0x000000 && pixel(i, LPIXEL * 4 + 1) != 0x000000) {
                        line(i, i / 3 + 7.33333 / 3 * LPIXEL, i, -i / 3 + 13.6666 / 3 * LPIXEL, newcolor);
                    }
                    else {
                        if (pixel(i, LPIXEL * 3 - 1) != 0x000000 && pixel(i, 4 * LPIXEL + 1) == 0x000000) { line(i, 4 * LPIXEL - 1, i, -i / 3 + 13.6666 / 3 * LPIXEL, newcolor); }

                        else {
                            if (pixel(i, LPIXEL * 3 - 1) == 0x000000 && pixel(i, 4 * LPIXEL + 1) != 0x000000) { line(i, i / 3 + 7.33333 / 3 * LPIXEL, i, 3 * LPIXEL, newcolor); }
                            else {
                                line(i, 3 * LPIXEL, i, 4 * LPIXEL - 1, newcolor);
                            }
                        }
                    }
                }
                if (pixel(6 * LPIXEL + 1, 3.5 * LPIXEL) != 0x000000 && boost != 0) {
                    for (int i = 6 * LPIXEL; i < 7 * LPIXEL; i++)
                    {
                        color -= 3;
                        if (boost > 0)color++;
                        int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                        if (pixel(i, LPIXEL * 3 - 1) != 0x000000 && pixel(i, LPIXEL * 4 + 1) != 0x000000) {
                            line(i, i / 4 + 10.0 / 4 * LPIXEL, i, -i / 4 + 17.75 / 4 * LPIXEL, newcolor);
                        }
                        else {
                            if (pixel(i, LPIXEL * 3 - 1) != 0x000000 && pixel(i, 4 * LPIXEL + 1) == 0x000000) { line(i, 4 * LPIXEL - 1, i, -i / 4 + 17.75 / 4 * LPIXEL, newcolor); }

                            else {
                                if (pixel(i, LPIXEL * 3 - 1) == 0x000000 && pixel(i, 4 * LPIXEL + 1) != 0x000000) { line(i, i / 4 + 10.0 / 4 * LPIXEL, i, 3 * LPIXEL, newcolor); }
                                else {
                                    line(i, 3 * LPIXEL, i, 4 * LPIXEL - 1, newcolor);
                                }
                            }
                        }
                    }
                }
            }

        }
    }

    void flashlight2(int boost)
    {
        int x;
        x = 3.5 * LPIXEL;
        int color = 96;
        for (int i = 3.5 * LPIXEL - 1; i >= 3 * LPIXEL; i--)
        {
            color -= 3;
            if (boost != 0) color += 1;
            int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
            line(i, i, 7 * LPIXEL - i - 1, i, newcolor);
        }
        if (pixel(3.5 * LPIXEL + 1, 3 * LPIXEL - 1) != 0x000000) {
            for (int i = 3 * LPIXEL - 1; i >= 2 * LPIXEL; i--)
            {
                color -= 3;
                if (boost != 0) color += 1;
                int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                if (pixel(i, i) != 0x000000 && pixel(7 * LPIXEL - i - 1, i) != 0x000000) { line(i, i, 7 * LPIXEL - i - 1, i, newcolor); }
                else {
                    if (pixel(i, i) != 0x000000 && pixel(4 * LPIXEL + 1, i) == 0x000000) { line(i, i, 4 * LPIXEL - 1, i, newcolor); }

                    else {
                        if (pixel(i, i) == 0x000000 && pixel(7 * LPIXEL - i - 1, i) != 0x000000) { line(3 * LPIXEL, i, 7 * LPIXEL - i - 1, i, newcolor); }
                        else {
                            line(3 * LPIXEL, i, 4 * LPIXEL - 1, i, newcolor);
                        }
                    }
                }
            }
            if (pixel(3.5 * LPIXEL, 2 * LPIXEL - 1) != 0x000000) {
                for (int i = 2 * LPIXEL - 1; i >= LPIXEL; i--)
                {
                    color -= 3;
                    if (boost > 0)color++;
                    int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                    if (pixel(LPIXEL * 3 - 1, i) != 0x000000 && pixel(LPIXEL * 4 + 1, i) != 0x000000) {
                        line(i / 3 + 7.33333 / 3 * LPIXEL, i, -i / 3 + 13.6666 / 3 * LPIXEL, i, newcolor);
                    }
                    else {
                        if (pixel(LPIXEL * 3 - 1, i) != 0x000000 && pixel(4 * LPIXEL + 1, i) == 0x000000) { line(i / 3 + 7.33333 / 3 * LPIXEL, i, 4 * LPIXEL - 1, i, newcolor); }

                        else {
                            if (pixel(LPIXEL * 3 - 1, i) == 0x000000 && pixel(4 * LPIXEL + 1, i) != 0x000000) { line(3 * LPIXEL, i, -i / 3 + 13.6666 / 3 * LPIXEL, i, newcolor); }
                            else {
                                line(3 * LPIXEL, i, 4 * LPIXEL - 1, i, newcolor);
                            }
                        }
                    }
                }
                if (pixel(3.5 * LPIXEL, LPIXEL - 1) != 0x000000 && boost != 0) {
                    for (int i = LPIXEL - 1; i >= 0; i--)
                    {
                        color -= 3;
                        if (boost > 0)color++;
                        int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                        if (pixel(LPIXEL * 3 - 1, i) != 0x000000 && pixel(LPIXEL * 4 + 1, i) != 0x000000) {
                            line(i / 4 + 11.25 / 4 * LPIXEL, i, -i / 4 + 16.75 / 4 * LPIXEL, i, newcolor);
                        }
                        else {
                            if (pixel(LPIXEL * 3 - 1, i) != 0x000000 && pixel(4 * LPIXEL + 1, i) == 0x000000) { line(i / 4 + 11.25 / 4 * LPIXEL, i, 4 * LPIXEL - 1, i, newcolor); }

                            else {
                                if (pixel(LPIXEL * 3 - 1, i) == 0x000000 && pixel(4 * LPIXEL + 1, i) != 0x000000) { line(3 * LPIXEL, i, -i / 4 + 16.75 / 4 * LPIXEL, i, newcolor); }
                                else {
                                    line(3 * LPIXEL, i, 4 * LPIXEL - 1, i, newcolor);
                                }
                            }
                        }
                    }
                }
            }

        }
    }

    void flashlight3(int boost)
    {
        int x;
        x = 3.5 * LPIXEL;
        int color = 96;
        for (int i = 3.5 * LPIXEL - 1; i >= 3 * LPIXEL; i--)
        {
            color -= 3;
            if (boost != 0) color += 1;
            int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
            line(i, i, i, 7 * LPIXEL - i - 1, newcolor);
        }
        if (pixel(3 * LPIXEL - 1, 3.5 * LPIXEL + 1) != 0x000000) {
            for (int i = 3 * LPIXEL - 1; i >= 2 * LPIXEL; i--)
            {
                color -= 3;
                if (boost != 0) color += 1;
                int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                if (pixel(i, i) != 0x000000 && pixel(i, 4 * LPIXEL + 1) != 0x000000) { line(i, i, i, 7 * LPIXEL - i - 1, newcolor); }
                else {
                    if (pixel(i, i) != 0x000000 && pixel(i, 4 * LPIXEL + 1) == 0x000000) { line(i, i, i, 4 * LPIXEL - 1, newcolor); }

                    else {
                        if (pixel(i, i) == 0x000000 && pixel(i, 4 * LPIXEL + 1) != 0x000000) { line(i, 3 * LPIXEL, i, 7 * LPIXEL - i - 1, newcolor); }
                        else {
                            line(i, 3 * LPIXEL, i, 4 * LPIXEL - 1, newcolor);
                        }
                    }
                }
            }
            if (pixel(2 * LPIXEL - 1, 3.5 * LPIXEL + 1) != 0x000000) {
                for (int i = 2 * LPIXEL - 1; i >= LPIXEL; i--)
                {
                    color -= 3;
                    if (boost > 0)color++;
                    int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                    if (pixel(i, LPIXEL * 3 - 1) != 0x000000 && pixel(i, LPIXEL * 4 + 1) != 0x000000) {
                        line(i, i / 3 + 7.33333 / 3 * LPIXEL, i, -i / 3 + 13.6666 / 3 * LPIXEL, newcolor);
                    }
                    else {
                        if (pixel(i, LPIXEL * 3 - 1) != 0x000000 && pixel(i, 4 * LPIXEL + 1) == 0x000000) { line(i, i / 3 + 7.33333 / 3 * LPIXEL, i, 4 * LPIXEL - 1, newcolor); }

                        else {
                            if (pixel(i, LPIXEL * 3 - 1) == 0x000000 && pixel(i, 4 * LPIXEL + 1) != 0x000000) { line(i, 3 * LPIXEL, i, -i / 3 + 13.6666 / 3 * LPIXEL, newcolor); }
                            else {
                                line(i, 3 * LPIXEL, i, 4 * LPIXEL - 1, newcolor);
                            }
                        }
                    }
                }
                if (pixel(LPIXEL - 1, 3.5 * LPIXEL) != 0x000000 && boost != 0) {
                    for (int i = LPIXEL - 1; i >= 0; i--)
                    {
                        color -= 3;
                        if (boost > 0)color++;
                        int newcolor = color * 16 * 16 + color * 16 * 16 * 16 * 16;
                        if (pixel(i, LPIXEL * 3 - 1) != 0x000000 && pixel(i, LPIXEL * 4 + 1) != 0x000000) {
                            line(i, i / 4 + 11.25 / 4 * LPIXEL, i, -i / 4 + 16.75 / 4 * LPIXEL, newcolor);
                        }
                        else {
                            if (pixel(i, LPIXEL * 3 - 1) != 0x000000 && pixel(i, 4 * LPIXEL + 1) == 0x000000) { line(i, i / 4 + 11.25 / 4 * LPIXEL, i, 4 * LPIXEL - 1, newcolor); }

                            else {
                                if (pixel(i, LPIXEL * 3 - 1) == 0x000000 && pixel(i, 4 * LPIXEL + 1) != 0x000000) { line(i, 3 * LPIXEL, i, -i / 4 + 16.75 / 4 * LPIXEL, newcolor); }
                                else {
                                    line(i, 3 * LPIXEL, i, 4 * LPIXEL - 1, newcolor);
                                }
                            }
                        }
                    }
                }
            }

        }
    }

    void flashlight(dude player, int boost)
    {
        int stack[7 * 11 * 11][2];
        int x, y;
        x = y = 3.5 * LPIXEL;
        switch (player.orientation)
        {
        case 2:
            flashlight2(boost);
            break;
        case 3:
            flashlight3(boost);
            break;
        case 0:
            flashlight0(boost);
            break;
        case 1:
            flashlight1(boost);
        default:
            break;
        }


    }

    void Items(int i, int j, dude player, maze game)
    {
        if (game.board[player.x + j - L / 2][player.y + i - L / 2].chest == true && pixel(j * LPIXEL + LPIXEL / 2, i * LPIXEL + LPIXEL / 2) >= 0x222200)
        {
            rect(j * LPIXEL + 1, i * LPIXEL + 2, LPIXEL - 2, LPIXEL - 4, 0xffff00);
        }
        if (game.board[player.x + j - L / 2][player.y + i - L / 2].mimic == true && pixel(j * LPIXEL + LPIXEL / 2, i * LPIXEL + LPIXEL / 2) >= 0x222200)
        {
            rect(j * LPIXEL + 1, i * LPIXEL + 2, LPIXEL - 2, LPIXEL - 4, 0x999900);
        }
        if (game.board[player.x + j - L / 2][player.y + i - L / 2].trap == true && pixel(j * LPIXEL + LPIXEL / 2, i * LPIXEL + LPIXEL / 2) >= 0x222200)
        {
            circle(j * LPIXEL + LPIXEL / 2, i * LPIXEL + LPIXEL / 2, LPIXEL / 2 - 1, 0x666666);
        }
        if (game.board[player.x + j - L / 2][player.y + i - L / 2].boost == true && pixel(j * LPIXEL + LPIXEL / 2, i * LPIXEL + LPIXEL / 2) >= 0x222200)
        {
            rect(j * LPIXEL + 2, i * LPIXEL + 4, LPIXEL - 4, LPIXEL - 8, 0x00ff00);
        }
        if (game.board[player.x + j - L / 2][player.y + i - L / 2].lightup == true && pixel(j * LPIXEL + LPIXEL / 2, i * LPIXEL + LPIXEL / 2) >= 0x222200)
        {
            rect(j * LPIXEL + 4, i * LPIXEL + 8, LPIXEL - 8, LPIXEL - 10, 0x00ff00);
            for (int k = 2; k < LPIXEL; k++)
            {
                line(j * LPIXEL + LPIXEL / 2, i * LPIXEL + 2, (j + 1) * LPIXEL - k, i * LPIXEL + LPIXEL / 2 + 1, 0x00ff00);
            }
        }
    }

    void Interaction(int& struggle, int& boost, dude& player, maze& game)
    {
        if (game.board[player.x][player.y].chest) { player.score += 500; game.board[player.x][player.y].chest = false;  showscore(player); return; }
        if (game.board[player.x][player.y].lightup) { boost = 100; game.board[player.x][player.y].lightup = false; player.score += 100; showscore(player); return; }
        if (game.board[player.x][player.y].boost) { player.battery = 450; game.board[player.x][player.y].boost = false; player.score += 100; return; }
        if (game.board[player.x][player.y].mimic)
        {
            if (direction != 4) { struggle--; }
            else { struggle++; }
            if (struggle == 0) { game.board[player.x][player.y].mimic = false; player.score += 200; }
            if (struggle == 30)player.death = true; return;
        }
        if (game.board[player.x][player.y].trap)
        {
            if (direction != 4) { struggle--; }
            if (struggle == 0) { game.board[player.x][player.y].trap = false; player.score += 100; }
        }
    }

    void SetStruggle(maze game, dude player, int& struggle)
    {
        if ((game.board[player.x][player.y].mimic || game.board[player.x][player.y].trap) && struggle == 0) { struggle = 10; }
    }

    void affichage(dude player, maze game, int dimention, int& boost)
    {
        for (int i = 0; i < L; i++)
        {
            for (int j = 0; j < L; j++)
            {
                if ((player.x + j - 3 < 1) || (player.x + j - 3 >= dimention - 1) || (player.y + i - 3 < 1) || (player.y + i - 3 >= dimention - 1)) {
                }
                else {
                    cube(j, i, game.board[player.x + j - L / 2][player.y + i - L / 2].wall ? 0x000000 : 0x010101);
                }
            }
        }
        flashlight(player, boost);
        for (int i = 0; i < L; i++)
        {
            for (int j = 0; j < L; j++)
            {
                if ((player.x + j - L / 2 < 1) || (player.x + j - L / 2 >= dimention - 1) || (player.y + i - L / 2 < 1) || (player.y + i - L / 2 >= dimention - 1)) {

                    cube(j, i, 0xffee00);
                }
                else
                {
                    Items(i, j, player, game);
                }
            }
        }
        disque(3.5 * LPIXEL, 3.5 * LPIXEL, LPIXEL / 2 - 1, ClRed);
    }

    void toStr(char s[], int v)
    {
        int div = 100;
        for (int i = 0; i < 3; i++)
        {
            s[i] = (v / div) + '0';
            v = v - v / div * div;
            div /= 10;
        }
    }

    void toStrscore(char s[], int v)
    {
        int div = 10000;
        for (int i = 0; i < 5; i++)
        {
            s[i] = (v / div) + '0';
            v = v - v / div * div;
            div /= 10;
        }
    }

    void Difficulty(int& size)
    {
        while (direction != 5)
        {
            if (size - 2 >= 9 && size + 2 <= 101)
            {
                switch (direction)
                {
                case 2:
                    if (size + 2 <= 101) size += 2;
                    break;
                case 0:
                    if (size - 2 >= 13)size -= 2;
                    break;
                default:
                    break;
                }
            }
            direction = 4;
            rect(0, 0, w, h, ClBlack);
            char T[4];
            toStr(T, size - 2);
            T[3] = '\0';
            text(18, 20, 700, "Difficulty", 0xffffff, ClTrensparent);
            text(30, 40, 700, T, 0xffffff, ClTrensparent, 2);
            Sleep(200);
        }
    }

    bool win(dude player, int dimention)
    {
        if (player.x == dimention - 1 || player.x == 0 || player.y == dimention - 1 || player.y == 0) return true;
        return false;
    }

    void addchainscore(char score[], char num[])
    {
        for (int i = 6; i < 11; i++)
            score[i] = num[i - 6];
    }

    void showscore(dude player)
    {
        char num[6];
        toStrscore(num, player.score);
        num[5] = '\0';
        text(0, 0, 700, num, 0x888888, ClTrensparent, 1);
    }

    void win(dude player) {
        text(23, 20, 700, "WIN", 0x77777, ClTrensparent, 2);
        char num[6];
        toStrscore(num, player.score);
        num[5] = '/0';
        char score[16] = "score:";
        addchainscore(score, num);
        text(10, 40, 700, score, 0x777777, ClTrensparent, 1);
    }

    void lightbar(int L) {
        rect(63, 1, 26, 5, 0x555555);
        if (L > 380)  rect(64, 2, 4, 3, 0x55ff55);
        if (L > 270)  rect(69, 2, 4, 3, 0x55ff55);
        if (L > 180)  rect(74, 2, 4, 3, 0x55ff55);
        if (L > 90)  rect(79, 2, 4, 3, 0x55ff55);
        if (L > 0)  rect(84, 2, 4, 3, 0x55ff55);
    }

    void Tutorial(int& page)
    {
        rect(0, 0, w, h, 0x000000);
        if (page == 0) {
            disque(4.5 * LPIXEL + 4, 8, LPIXEL / 2 - 1, ClRed);
            text(1 + 1.5 * LPIXEL, 5, 91, "You :", 0x666622);
            text(1, 20, 91, "Your goal is to", 0x666622);
            text(1, 30, 91, "get out of the", 0x666622);
            text(1, 40, 91, "maze before", 0x666622);
            text(1, 50, 91, "your flashlight", 0x666622);
            text(1, 60, 91, "runs out of", 0x666622);
            text(1, 70, 91, "battery.", 0x666622);
            text(1, 80, 91, "Good luck!", 0x666622);
            vline(87, 6, 5, 0x444444);
            vline(88, 7, 3, 0x444444);
            vline(89, 8, 1, 0x444444);
        }
        if (page == 1) {
            rect(4.5 * LPIXEL + 7, 4, LPIXEL - 2, LPIXEL - 4, 0xffff00);
            text(3 + LPIXEL, 5, 91, "Chest :", 0x666622);
            text(1, 20, 91, "Will increase", 0x666622);
            text(1, 30, 91, "your score but", 0x666622);
            text(1, 40, 91, "some may not be", 0x666622);
            text(1, 50, 91, "what they seem", 0x666622);
            text(1, 60, 91, "and will try to", 0x666622);
            text(1, 70, 91, "eat you.Move to", 0x666622);
            text(1, 80, 91, "free yourself.", 0x666622);
            vline(87, 6, 5, 0x444444);
            vline(88, 7, 3, 0x444444);
            vline(89, 8, 1, 0x444444);
            vline(4, 6, 5, 0x444444);
            vline(3, 7, 3, 0x444444);
            vline(2, 8, 1, 0x444444);
        }
        if (page == 2) {
            circle(5 * LPIXEL + 2, 8, LPIXEL / 2 - 1, 0x666666);
            text(1.5 * LPIXEL * 1, 5, 91, "Trap :", 0x666622);
            text(1, 20, 91, "Will dig into", 0x666622);
            text(1, 30, 91, "your legs and", 0x666622);
            text(1, 40, 91, "slow your ", 0x666622);
            text(1, 50, 91, "progression.", 0x666622);
            text(1, 60, 91, "Move to free", 0x666622);
            text(1, 70, 91, "yourself.", 0x666622);
            vline(87, 6, 5, 0x444444);
            vline(88, 7, 3, 0x444444);
            vline(89, 8, 1, 0x444444);
            vline(4, 6, 5, 0x444444);
            vline(3, 7, 3, 0x444444);
            vline(2, 8, 1, 0x444444);
        }
        if (page == 3) {
            rect(4 * LPIXEL + 18, 6, LPIXEL - 4, LPIXEL - 8, 0x00ff00);
            text(12, 5, 91, "Battery :", 0x666622);
            text(1, 20, 91, "Will recharge", 0x666622);
            text(1, 30, 91, "your flashlight", 0x666622);
            text(1, 40, 91, "to max capacity", 0x666622);
            /*vline(87, 6, 5, 0x444444);
            vline(88, 7, 3, 0x444444);
            vline(89, 8, 1, 0x444444);*/
            vline(4, 6, 5, 0x444444);
            vline(3, 7, 3, 0x444444);
            vline(2, 8, 1, 0x444444);
        }
        if (page == 4) {
            rect(5 * LPIXEL + 8, 10, LPIXEL - 8, LPIXEL - 10, 0x00ff00);
            for (int k = -4; k < LPIXEL - 6; k++)
            {
                line(5 * LPIXEL + LPIXEL / 2 + 4, 0 * LPIXEL + 4, (5 + 1) * LPIXEL - k - 2, 0 * LPIXEL + LPIXEL / 2 + 3, 0x00ff00);
            }
            text(10, 5, 91, "Lightup :", 0x666622);
            text(1, 20, 91, "Will increase", 0x666622);
            text(1, 30, 91, "the range of", 0x666622);
            text(1, 40, 91, "your flashlight", 0x666622);
            vline(4, 6, 5, 0x444444);
            vline(3, 7, 3, 0x444444);
            vline(2, 8, 1, 0x444444);
        }
        if (direction == 3 && page != 0)page--;
        if (direction == 1 && page != 3)page++;
    }

    bool StartMenu()
    {
        rect(0, 0, w, h, ClBlack);
        disque(PIXEL / 2, PIXEL / 2, LPIXEL / 2 - 1, ClRed);
        Sleep(100);
        if (pixel(3, PIXEL / 2) != 0x000000)
        {
            text(1, PIXEL / 2 - 3, 700, "Start", 0x55110099);
        }
        disque(PIXEL / 2, PIXEL / 2, LPIXEL / 2 - 1, ClRed);
        Sleep(500);
        rect(0, 0, w, h, ClBlack);
        disque(PIXEL / 2, PIXEL / 2, LPIXEL / 2 - 1, ClRed);
        Sleep(200);
        direction = 2;
        int select = 2;
        while (direction != 5 || select == 2)
        {
            rect(0, 0, w, h, ClBlack);
            if (direction == 3) select = 0;
            if (direction == 1) select = 1;
            if (direction == 2)select = 2;
            if (direction == 0) select = 3;
            if (select == 0) {
                for (int i = 3 * LPIXEL - 2 * LPIXEL / 3 + 3; i < 5 * LPIXEL - 2 * LPIXEL / 3; i++)
                {
                    line(0, i, PIXEL / 2 - 5, PIXEL / 2, 0x66888800);

                }
                disque(PIXEL / 2, PIXEL / 2, LPIXEL / 2 - 1, ClRed);
                select = 0;
            }
            if (select == 1) {
                for (int i = 3 * LPIXEL - 2 * LPIXEL / 3 + 3; i < 5 * LPIXEL - 2 * LPIXEL / 3; i++)
                {
                    line(PIXEL, i, PIXEL / 2 + 5, PIXEL / 2, 0x66888800);
                }
                disque(PIXEL / 2, PIXEL / 2, LPIXEL / 2 - 1, ClRed);
                select = 1;
            }
            if (select == 2) {
                for (int i = 0.4 * LPIXEL; i < 6.6 * LPIXEL; i++)
                {
                    line(PIXEL / 2, PIXEL / 2 - 6, i, 0, 0x66888800);

                }
                disque(PIXEL / 2, PIXEL / 2, LPIXEL / 2 - 1, ClRed);
                select = 2;
            }
            if (select == 3)
            {
                for (int i = 0.4 * LPIXEL; i < 6.6 * LPIXEL; i++)
                {
                    line(PIXEL / 2, PIXEL / 2 + 6, i, 7 * LPIXEL - 1, 0x66888800);

                }
                disque(PIXEL / 2, PIXEL / 2, LPIXEL / 2 - 1, ClRed);
                select = 3;
            }
            if (pixel(3, PIXEL / 2) != 0x000000)
            {
                text(1, PIXEL / 2 - 3, 700, "Start", 0x55110099);
            }
            if (pixel(PIXEL - 4, PIXEL / 2) != 0x000000)
            {
                text(PIXEL / 2 + 20, PIXEL / 2 - 3, 700, "Exit", 0x55110099);
            }
            if (pixel(PIXEL / 2, 0) != 0x000000)
            {
                text(18, 5, 700, "MazeRunner", 0x55110099);
            }
            if (pixel(PIXEL / 2, 7 * LPIXEL - 1) != 0x000000)
            {
                text(23, 76, 700, "Handbook", 0x55110099);
            }
            Sleep(100);
            if (direction == 5 && select == 2)
            {
                direction = 4;
                int flip = 0;
                while (direction != 5 && flip != 30)
                {
                    if (flip == 0) {
                        rect(0, 0, w, h, 0x111111);
                        text(23, 4, 700, "Credits", 0x999999);
                        text(16, 15, 700, "Developper", 0x999944);
                        text(24, 25, 91, "PSYAMEN", 0x999944);
                        text(21, 40, 700, "Graphics", 0x999944);
                        text(30, 50, 700, "HPmad", 0x999944);
                        text(28, 65, 700, "Ideas", 0x999944);
                        text(24, 75, 91, "Vincent", 0x999944);
                    }
                    if (flip == 11) {
                        rect(0, 0, w, h, 0x111111);
                        text(7, 35, 700, "Moral support", 0x999944);
                        text(35, 45, 91, "Lina", 0x999944);
                    }
                    if (flip == 21)
                    {
                        rect(0, 0, w, h, 0x111111);
                        text(2, 75, 91, "and You", 0x999944);
                    }
                    Sleep(500);
                    flip++;
                }
                direction = 2;
            }
            if (direction == 5 && select == 3)
            {
                direction = 6;
                int page = 0;
                int savepage;
                while (direction != 5)
                {
                    savepage = page;
                    if (direction != 4)Tutorial(page);
                    direction = 4;
                    if (savepage != page) direction = 6;
                    Sleep(100);
                }
                direction = 2;
            }
        }
        direction = 4;
        if (select == 0)return true;
        return false;
    }

    bool End(dude player, maze game, int dimention, int lightup, bool& revive)
    {
        bool select = true;
        revive = false;
        direction = 0;
        player.score += 2 * player.battery;
        while (direction != 5)
        {
            if (direction != 4) {
                for (int i = 0; i < PIXEL; i++)
                {
                    for (int j = 0; j < PIXEL; j++)
                    {
                        if ((player.x + j - 3 < 1) || (player.x + j - 3 >= dimention - 1) || (player.y + i - 3 < 1) || (player.y + i - 3 >= dimention - 1)) {
                            cube(j, i, 0xffee00);
                        }
                        else {
                            cube(j, i, game.board[player.x + j - L / 2][player.y + i - L / 2].wall ? 0x000000 : 0x010101);
                        }
                    }
                }
                if (player.death == true) {
                    disque(3.5 * LPIXEL, 3.5 * LPIXEL, LPIXEL / 2 - 1, ClRed);
                    text(PIXEL / 2 - 20, PIXEL / 2 - 20, 700, "YOU", 0xaa2222, ClTrensparent, 2);
                    text(PIXEL / 2 - 23, PIXEL / 2, 700, "LOSE", 0xaa2222, ClTrensparent, 2);
                }
                else {
                    disque(3.5 * LPIXEL, 3.5 * LPIXEL, LPIXEL / 2 - 1, ClRed);
                    win(player);
                }
                if (direction == 0) { revive = true; select = true; }
                if (direction == 3) { select = true;  revive = false; }
                if (direction == 1) { select = false; revive = false; }
                if (select && revive) rect(PIXEL / 2 - 19, PIXEL / 2 + 29, 37, 9, 0x888800);
                if (select && !revive) rect(0, PIXEL / 2 + 19, 42, 9, 0x888800);
                if (!select && !revive) rect(PIXEL / 2 + 19, PIXEL / 2 + 19, 24, 9, 0x888800);
                text(PIXEL / 2 + 20, PIXEL / 2 + 20, 700, "Exit", 0x110099);
                text(1, PIXEL / 2 + 20, 700, "Restart", 0x110099);
                text(PIXEL / 2 - 18, PIXEL / 2 + 30, 700, "Revive", 0x110099);
                direction = 4;
            }
            Sleep(100);
        }
        return select;
    }

    void exec()
    {
        bool keepgoing = false;
        while (keepgoing || StartMenu())
        {
            direction = 4;
            srand(time(NULL));
            dude player;
            maze game;
            int size = 13;
            int boost = 0;
            int struggle = 0;
            int score = 0;
            int lightup = 0;
            Difficulty(size);
            CreateMaze(game, size);
            affichage(player, game, size, boost);
            showscore(player);
            bool revive = true;
            struggle = 0;
            while (revive)
            {
                player.x = game.start[0];
                player.y = game.start[1];
                affichage(player, game, size, lightup);
                showscore(player);
                player.battery = 450;
                player.death = false;
                boost = 0;
                score = 0;
                lightup = 0;
                struggle = 0;
                while (!win(player, size) && player.death == false)
                {
                    if (struggle == 0) {
                        if (move(player, game))
                        {
                            affichage(player, game, size, lightup);
                            showscore(player);
                        }
                    }
                    else
                    {
                        affichage(player, game, size, lightup);
                        int n = 4287102976 - 16777216 * struggle * 5;
                        rect(0, 0, w, h, n);
                    }
                    SetStruggle(game, player, struggle);
                    Interaction(struggle, lightup, player, game);
                    direction = 4;
                    if (boost != 0) { boost--; }
                    if (lightup != 0) { lightup--; if (lightup == 0) { affichage(player, game, size, lightup); } }
                    player.battery--;
                    lightbar(player.battery);
                    if (player.battery == 0) player.death = true;
                    Sleep(100);
                }
                keepgoing = End(player, game, size, lightup, revive);
            }
        }
        exit(0);
    }
};


int main()
{
    srand(time(NULL));
    CMyWin fb; fb.setFps(20);
    fb.run();
    exit(0);
    return 0;
}
