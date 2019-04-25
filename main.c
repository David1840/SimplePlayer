#include <stdio.h>
#include <SDL2/SDL.h>

int WinMain() {

    int quit = 1;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *sdlTexture = NULL;
    SDL_Event event;
    SDL_Rect rect; // 长方形，原点在左上角
    rect.w = 50;
    rect.h = 50;

    SDL_Init(SDL_INIT_VIDEO);//初始化函数,可以确定希望激活的子系统

    window = SDL_CreateWindow("My First Window",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640,
                              480,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);// 创建窗口

    if (!window) {
        return -1;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);//基于窗口创建渲染器
    if (!renderer) {
        return -1;
    }
//    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); //设置渲染器颜色
//    SDL_RenderClear(renderer); //用指定的颜色清空缓冲区
//    SDL_RenderPresent(renderer); //将缓冲区中的内容输出到目标窗口上

    sdlTexture = SDL_CreateTexture(renderer,
                                   SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_TARGET,
                                   640,
                                   480); //创建纹理

    if (!sdlTexture) {
        return -1;
    }

    while (quit) {
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                SDL_Log("quit");
                quit = 0;
                break;
            default:
                SDL_Log("event type:%d", event.type);
        }


        rect.x = rand() % 600;
        rect.y = rand() % 400;

        SDL_SetRenderTarget(renderer, sdlTexture); // 设置渲染目标为纹理
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // 纹理背景为黑色
        SDL_RenderClear(renderer); //清屏

        SDL_RenderDrawRect(renderer, &rect); //绘制一个长方形
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //长方形为白色
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderTarget(renderer, NULL); //恢复默认，渲染目标为窗口
        SDL_RenderCopy(renderer, sdlTexture, NULL, NULL); //拷贝纹理到CPU

        SDL_RenderPresent(renderer); //输出到目标窗口上
    }

    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window); //销毁窗口
    SDL_Quit();
    return 0;
}