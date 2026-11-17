#include <iostream>
#include <deque>
#include "raylib.h"
#include <raymath.h>
using namespace std;

Color bgcolor = {173, 204, 96, 255};
Color dark_green = {43, 51, 24, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdate = 0;

bool eventTriggered(double interval) // this is to short the update time , which means the update was being called a lot of time this will reduce
{
    double current_time = GetTime();
    if(current_time - lastUpdate > interval)
    {
        lastUpdate = current_time;
        return true;
    } 
    return false;
}

bool ElementInDeque(Vector2 element, deque<Vector2> arr)
{
    for (Vector2 vec : arr)
    {
        if(Vector2Equals(vec, element))
        {
            return true;
        }
    }
    return false;
}

class Food 
{
    Texture2D texture;

    public:
        Vector2 position; 

        Food(deque<Vector2> Snakebody)
        {
            Image image = LoadImage("graphics/food.png");
            ImageResize(&image, 30, 30);
            texture = LoadTextureFromImage(image);
            position = GenerateRandomPos(Snakebody);
            UnloadImage(image);
        }

        ~Food()
        {
            UnloadTexture(texture);
        }
        
        void Draw()
        {
            DrawTexture(texture, offset + position.x*cellSize, offset + position.y*cellSize, WHITE);
        }

        Vector2 GenerateRandomCell()
        {
            float x = GetRandomValue(0,cellCount - 1);
            float y = GetRandomValue(0,cellCount - 1);
            return {x,y};
        }

        Vector2 GenerateRandomPos(deque<Vector2> Snakebody)
        {
            Vector2 pos = GenerateRandomCell();
            while(ElementInDeque(pos, Snakebody))
            {
                pos = GenerateRandomCell();
            }
            return pos;
        }
};

class Snake
{
    public:
    deque<Vector2> body = {{6,9}, {5,9}, {4,9}};
    Vector2 direction = {1,0};
    Vector2 lastdirection = {1,0}; // preventing the reverse direction my pressing multiple direction
    bool segment = false; // adding segment

    Image headImage = LoadImage("graphics/Head.png");
    Image bodyImage = LoadImage("graphics/Body.png");
    Image cornerImage = LoadImage("graphics/Corner_body.png");

    Texture2D headTexture;
    Texture2D bodyTexture;
    Texture2D cornerTexture;

    Vector2 origin = {(float)cellSize/2, (float)cellSize/2}; // texture(Head and body) origin

    Rectangle head_source = {0, 0, 30, 30}; // Head texture source
    int head_rotation = 0;

    Rectangle body_source = {0, 0, 30, 30}; // Body Texture source
    int body_rotation = 0;

    Snake()
    {
        headTexture = LoadTextureFromImage(headImage);
        bodyTexture = LoadTextureFromImage(bodyImage);
        cornerTexture = LoadTextureFromImage(cornerImage);
    
        UnloadImage(headImage);
        UnloadImage(bodyImage);
        UnloadImage(cornerImage);
    }

    ~Snake()
    {
        UnloadTexture(headTexture);
        UnloadTexture(bodyTexture);
        UnloadTexture(cornerTexture);
    }

    void Draw()
    {
        Texture2D Draw_texture; // for body texture purpose

        for(int i = 0; i<body.size(); i++)
        {
            float draw_x = offset + body[i].x*cellSize;
            float draw_y = offset + body[i].y*cellSize;   
            Rectangle destination = {draw_x + cellSize/2, draw_y + cellSize/2, (float)cellSize, (float)cellSize}; // texture(body and head) Destination

            if(i == 0) // Head Orientation
            {                
                if(lastdirection.x == -1 && lastdirection.y == 0)// left facing
                {
                    head_rotation = 0;
                    head_source.width = -headTexture.width; // to flip the image on the y axis
                }
                else if(lastdirection.x == 1 && lastdirection.y == 0)// right facing
                {
                    head_rotation = 0;
                    head_source.width = headTexture.width; // to flip the image on the y axis
                }
                else if(lastdirection.x == 0 && lastdirection.y == -1)// top facing
                {
                    if(head_source.width < 0) head_rotation = 90;
                    else head_rotation = -90; // to rotate the image
                }
                else if(lastdirection.x == 0 && lastdirection.y == 1)// bottom facing
                {
                    if(head_source.width < 0) head_rotation = -90;
                    else head_rotation = 90;
                }

                DrawTexturePro(headTexture, head_source, destination, origin, head_rotation, WHITE);
            }
            else 
            {
                
                if(i == body.size() - 1)
                {
                    if(body[i-1].x - body[i].x == 0) body_rotation = 90;
                    else body_rotation = 0;
                    Draw_texture = bodyTexture;
                }
                else
                {
                    if(abs(body[i].x - body[i-1].x) == 1 && abs(body[i].x - body[i+1].x) == 1)
                    {
                        // Vertical
                        body_rotation = 0;
                        Draw_texture = bodyTexture;
                    }
                    else if(body[i].x - body[i-1].x == 0 && body[i].x - body[i+1].x == 0)
                    {
                        // Horizontal
                        body_rotation = 90;
                        Draw_texture = bodyTexture;
                    }
                    else if(body[i].x - body[i-1].x == -1 && body[i+1].y - body[i].y == -1 || body[i].y - body[i-1].y == 1 && body[i+1].x - body[i].x == 1)
                    {
                        // Top Right Corner
                        body_rotation = 0;
                        Draw_texture = cornerTexture;
                    }
                    else if(body[i].x - body[i-1].x == 1 && body[i+1].y - body[i].y == -1 || body[i].y - body[i-1].y == 1 && body[i+1].x - body[i].x == -1)
                    {
                        // Top Left Corner
                        body_rotation = 270;
                        Draw_texture = cornerTexture;
                    }
                    else if(body[i].x - body[i-1].x == 1 && body[i+1].y - body[i].y == 1 || body[i].y - body[i-1].y == -1 && body[i+1].x - body[i].x == -1)
                    {
                        // Bottom Left Corner
                        body_rotation = 180;
                        Draw_texture = cornerTexture;
                    }
                    else if(body[i].x - body[i-1].x == -1 && body[i+1].y - body[i].y == 1 || body[i].y - body[i-1].y == -1 && body[i+1].x - body[i].x == 1)
                    {
                        // Bottom Right Corner
                        body_rotation = 90;
                        Draw_texture = cornerTexture;
                    }
                }
                DrawTexturePro(Draw_texture, body_source, destination, origin, body_rotation, WHITE);
            }
        }
    }

    void update()
    {
        lastdirection = direction;

        body.push_front(Vector2Add(body[0], direction));
        if(segment)
        {
            segment = false;
        }
        else 
        {
            body.pop_back();
        }
    }

    void Reset()
    {
        body = {{6,9}, {5,9}, {4,9}};
        direction = {1,0};

        segment = false;

        head_source.width = headTexture.width;
        head_source.height = headTexture.height;
        head_rotation = 0;
        lastdirection = direction;
    }
};

class Game
{
    public:

    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("sounds/eat.wav");
        wallSound = LoadSound("sounds/gameover.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        snake.Draw();
        food.Draw();
    }

    void update()
    {
        if(running)
        {
            snake.update();
            checkCollisionWithFood();
            checkCollisionWithEdge();
            checkCollisionWithTail();
        }
    }

    void checkCollisionWithFood()
    {
        if(Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.segment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void checkCollisionWithEdge()
    {
        if(snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if(snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void checkCollisionWithTail()
    {
        deque <Vector2> headless = snake.body;
        headless.pop_front();
        if(ElementInDeque(snake.body[0], headless))
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }
};

int main()
{
    InitWindow(2*offset + cellSize * cellCount, 2*offset + cellSize * cellCount, "Apple Byte Game");
    SetTargetFPS(60);
    Texture2D bordertexture = LoadTexture("graphics/border.png");

    Game game = Game();
    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        while (eventTriggered(0.17))
        {
            game.update();
        }
        
        if(IsKeyPressed(KEY_UP) && game.snake.lastdirection.y != 1)
        {
            game.snake.direction = {0,-1};
            game.running = true;
        }
        else if(IsKeyPressed(KEY_DOWN) && game.snake.lastdirection.y != -1)
        {
            game.snake.direction = {0,1};
            game.running = true;
        }
        else if(IsKeyPressed(KEY_LEFT) && game.snake.lastdirection.x != 1)
        {
            game.snake.direction = {-1,0};
        }
        else if(IsKeyPressed(KEY_RIGHT) && game.snake.lastdirection.x != -1)
        {
            game.snake.direction = {1,0};
            game.running = true;
        }

        ClearBackground(bgcolor);

        DrawTexture(bordertexture, 0, 0, WHITE);
        DrawText("Apple byte", offset-5, 10, 30, dark_green);
        DrawText("SCORE :", (cellCount*cellSize) - 100, 10, 30, dark_green);
        DrawText(TextFormat("%03i", game.score), (cellCount*cellSize) + offset - 45, 10, 30, dark_green);
        game.Draw();
        EndDrawing();
    }
    UnloadTexture(bordertexture);
    CloseWindow();
    return 0;
}