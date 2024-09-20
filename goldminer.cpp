#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
#include <vector>

// 設定窗口大小
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// 顏色定義
SDL_Color GOLD = {255, 215, 0, 255};
SDL_Color GRAY = {169, 169, 169, 255};
SDL_Color BLACK = {0, 0, 0, 255};

// 物品類型
struct Item {
    int x, y;
    int value;
    SDL_Color color;
    bool grabbed = false;

    Item(int x_, int y_, int value_, SDL_Color color_) : x(x_), y(y_), value(value_), color(color_) {}
};

// 挖礦爪類型
class GoldMiner {
public:
    int x = SCREEN_WIDTH / 2; // 鉤爪的固定點 X 坐標
    int y = 50; // 鉤爪的固定點 Y 坐標
    int angle = 0; // 鉤爪當前角度
    int length = 100; // 鉤爪的長度
    int direction = 1;
    int speed = 2;
    bool grabbing = false; // 是否正在抓取
    Item* grabbedItem = nullptr; // 被抓取的物體指針

    void update() {
        if (!grabbing) {
            // 擺動挖礦爪從左下到右下 (角度範圍：-80 到 80)
            angle += speed * direction;
            if (angle >= 160) {
                angle = 160;  // 確保不超過 170 度
                direction *= -1;
            } else if (angle <= 20) {
                angle = 20;   // 確保不小於 10 度
                direction *= -1;
            }
        } else if (grabbedItem) {
            
            // 抓取後將物體拉回來
            length -= 5;
            if (length <= 100) {
                // 拉回原點後重置狀態
                grabbing = false;
                grabbedItem->grabbed = false;
                grabbedItem = nullptr;
                length = 100;
            }
        } else {
            // 抓取但未碰到物體，爪子自動拉回
            length += 5;
            if (length > SCREEN_HEIGHT) {
                grabbing = false;
                length = 100;
            }
        }
    }

    void grab(std::vector<Item>& items) {
        if (!grabbing) {
            grabbing = true;
            for (auto& item : items) {
                if (checkCollision(item)) {
                    std::cout<<"Nice grab !"<<std::endl;
                    grabbedItem = &item;
                    item.grabbed = true;
                    break;
                } else {
                    std::cout<<"SUCK !"<<std::endl;
                }
            }
        }
    }

    bool checkCollision(Item& item) {
        // 計算爪子的末端位置
        float radianAngle = angle * M_PI / 180.0f;  // 將角度轉換為弧度
        int end_x = x + length * cos(radianAngle);  // 計算末端 x 坐標
        int end_y = y + length * sin(radianAngle);  // 計算末端 y 坐標
        std::cout<<end_x<<","<<end_y<<"vs"<<item.x<<","<<item.y<<std::endl;

        // 檢查爪子的末端是否與物品碰撞


        return item.x - 20 < end_x && end_x < item.x + 20 && 
            item.y - 20 < end_y && end_y < item.y + 20;
    }


    void draw(SDL_Renderer* renderer) {
        float radianAngle = angle * M_PI / 180.0f;
        int end_x = x + length * cos(radianAngle);
        int end_y = y + length * sin(radianAngle);
        SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
        SDL_RenderDrawLine(renderer, x, y, end_x, end_y);
        SDL_Rect rect = {end_x - 5, end_y - 5, 10, 10};
        SDL_RenderFillRect(renderer, &rect);

        // 如果抓住了物體，將物體隨著爪子移動
        if (grabbedItem) {
            grabbedItem->x = end_x;
            grabbedItem->y = end_y;
        }
    }
};

// 繪製物品
void drawItem(SDL_Renderer* renderer, Item& item) {
    SDL_SetRenderDrawColor(renderer, item.color.r, item.color.g, item.color.b, item.color.a);
    SDL_Rect rect = {item.x - 20, item.y - 20, 40, 40};
    SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL 初始化失敗: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("黃金礦工", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "創建窗口失敗: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "創建渲染器失敗: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 創建挖礦者和物品
    GoldMiner miner;
    std::vector<Item> items = {Item(200, 300, 100, GOLD), Item(500, 400, 50, GRAY)};

    bool quit = false;
    SDL_Event e;

    // 遊戲主循環
    while (!quit) {
        // 事件處理
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                miner.grab(items);
            }
        }

        // 更新遊戲邏輯
        miner.update();

        // 清屏
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // 畫出物品
        for (auto& item : items) {
            if (!item.grabbed) {
                drawItem(renderer, item);
            }
        }

        // 畫出挖礦爪
        miner.draw(renderer);

        // 更新屏幕
        SDL_RenderPresent(renderer);

        // 控制幀率
        SDL_Delay(1000 / 60);
    }

    // 清理資源
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
