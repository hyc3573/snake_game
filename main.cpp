#include <SFML/Graphics.hpp>
#include <deque>
#include <utility>
#include <iterator>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <Windows.h>
#include "ConnectionIii-Rj3W.hpp"

// 매크로 남용의 극치
#define SNAKE 1
#define APPLE 2
#define EMPTY 0
#define ILLEGAL 3
#define WALL 3

#define BWIDTH 40 // B means Board
#define BHEIGHT 40
#define BMARGIN 2 // px

#define SWIDTH 800 // S means Screen(window)
#define SHEIGHT 800

#define DIED 2
#define GROWN 1
#define NOTHING 0

#define SPEED .055f // second per move

#define TEXT 4
#define TEXTSIZE 36
#define TEXTMARGINX 50
#define TEXTMARGINY 50

typedef int Square;

using namespace std;
using namespace sf;

class Board
{
private:
    Square board[BWIDTH][BHEIGHT];

public:
    Board() : board{ 0 } {};

    bool isLegal(int X, int Y)
    {
        return 0 <= X && X < BWIDTH && 0 <= Y && Y < BHEIGHT;
    }

    Square getSquare(int X, int Y)
    {
        return board[X][Y];
    }

    void setSquare(int X, int Y, Square value)
    {
        board[X][Y] = value;
    }
};

class Snake
{
private:
    deque<pair<int, int>> body; // first element is head, last element is tail
    int dirX;
    int dirY;
    Board& board;

public:
    Snake(int X, int Y, Board& board, int length = 5) : dirX(1), dirY(0), board(board)
    {
        for (int i = 0;i < length;i++)
        {
            board.setSquare(X + dirX * i, Y + dirY * i, SNAKE);
            body.push_front(pair<int, int>(X + dirX * i, Y + dirY * i));
        }
    }

    Snake(Board& board) : dirX(1), dirY(0), body(deque<pair<int, int>>()), board(board) {};

    Snake(const Snake& lhs) : dirX(lhs.dirX), dirY(lhs.dirY), board(lhs.board), body(lhs.body) {};

    Snake& operator=(const Snake& lhs)
    {
        dirX = lhs.dirX;
        dirY = lhs.dirY;
        board = lhs.board;
        body = lhs.body;
        return *this;
    }

    int move()
    {
        auto head = body.front();
        auto tail = body.back();
        int goalX = head.first + dirX;
        int goalY = head.second + dirY;
        bool grow = false;

        if (not board.isLegal(goalX, goalY)) // if it crashed to wall
        {
            return DIED;
        }

        Square element = board.getSquare(goalX, goalY);
        if (element == SNAKE) // if it crashed to itself
        {
            return DIED;
        }

        grow = element == APPLE; // true if it ate apple

        body.push_front(pair<int, int>(goalX, goalY));
        board.setSquare(goalX, goalY, SNAKE);

        if (not grow)
        {
            body.pop_back();
            board.setSquare(tail.first, tail.second, EMPTY);
            return NOTHING;
        }
        return GROWN;
    }

    void setXdir(int value)
    {
        if (-value != dirX) // to prevent player from crashing to itself from nowhere by going backwards
        {
            dirX = value;
            dirY = 0;
        }
    }

    void setYdir(int value)
    {
        if (-value != dirY) // same as above
        {
            dirY = value;
            dirX = 0;
        }
    }
};

void newApple(Board& board)
{
    int i = 0;
    int j = 0;

    do
    {
        i = rand() % BWIDTH;
        j = rand() % BHEIGHT;
    } while (board.getSquare(i, j) != EMPTY); // to prevent apple from generating as the same position as other things

    board.setSquare(i, j, APPLE);
}

void reset(Board& board, Snake& snake, int& score)
{
    board = Board();
    snake = Snake(5, BHEIGHT / 2 + 1, board);
    score = 0;
    newApple(board);
}

int main(int argc, char* argv[])
{
    unsigned int t = GetTickCount64();
    srand(t);

    RenderWindow window(VideoMode(SWIDTH, SHEIGHT), L"크고♂아름다운♂게이ㅁ");

    Event event;

    Board board;
    Snake snake(board);

    Clock clock;

    // color schemes :     square              snake              apple               wall                 score
    Color colorTheme[] = { Color(11, 36, 217), Color(0, 240, 60), Color(255, 13, 23), Color(217, 170, 11), Color(217, 170, 11) };

    vector<vector<RectangleShape>> squares = vector<vector<RectangleShape>>(BWIDTH, vector<RectangleShape>(BHEIGHT, RectangleShape()));
    // generates BWIDTH * BHEIGHT vector that contains each square's shape 

    int score = 0;

    int result = 0;

    Font font;
    if (!font.loadFromMemory(ConnectionIii_Rj3W_otf, ConnectionIii_Rj3W_otf_len))
    {
        cout << "Error!" << endl;
        return 1;
    }

    Text text;
    text.setFont(font);
    text.setFillColor(colorTheme[TEXT]);
    text.setCharacterSize(TEXTSIZE);
    text.setPosition(TEXTMARGINX, TEXTMARGINY);

    reset(board, snake, score);

    for (int i = 0;i < BWIDTH;i++)
    {
        for (int j = 0;j < BHEIGHT;j++)
        {
            // populating the vector
            squares[i][j] = RectangleShape(Vector2f(SWIDTH / BWIDTH, SHEIGHT / BHEIGHT));
            squares[i][j].setFillColor(colorTheme[EMPTY]);
            squares[i][j].setOutlineColor(colorTheme[WALL]);
            squares[i][j].setOutlineThickness(BMARGIN / 2);
            squares[i][j].setPosition(SWIDTH / BWIDTH * i, SHEIGHT / BHEIGHT * j); 
        }
    }

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed:
                window.close();
                break;

            case Event::KeyPressed:
                switch (event.key.code)
                {
                // use WASD for controlling
                case Keyboard::W:
                    snake.setYdir(-1);
                    break;
                case Keyboard::S:
                    snake.setYdir(1);
                    break;
                case Keyboard::A:
                    snake.setXdir(-1);
                    break;
                case Keyboard::D:
                    snake.setXdir(1);
                    break;
                }
                break;
            }
        }

        if (clock.getElapsedTime().asSeconds() > SPEED)
        {
            clock.restart();
            result = snake.move();

            switch (result)
            {
            case DIED:
                reset(board, snake, score);
                break;

            case GROWN:
                score++;
                newApple(board); // if player ate an apple, then generate new one
            }
        }

        for (int i = 0;i < BWIDTH;i++) // update the vector, and drawing them
        {
            for (int j = 0;j < BHEIGHT;j++)
            {
                squares[i][j].setFillColor(colorTheme[board.getSquare(i, j)]);
                window.draw(squares[i][j]);
            }
        }
        text.setString(to_string(score));
        window.draw(text);

        window.display();
    }
    return 0;
}