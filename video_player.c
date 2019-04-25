#include <stdio.h>
#include <SDL.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#define REFRESH_EVENT  (SDL_USEREVENT + 1)

#define BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit = 0;
int thread_pause = 0;

int video_refresh_thread(void *data) {
    thread_exit = 0;
    thread_pause = 0;

    while (!thread_exit) {
        if (!thread_pause) {
            SDL_Event event;
            event.type = REFRESH_EVENT;
            SDL_PushEvent(&event);
        }
        SDL_Delay(40);
    }
    thread_exit = 0;
    thread_pause = 0;
    //Break
    SDL_Event event;
    event.type = BREAK_EVENT;
    SDL_PushEvent(&event);

    return 0;
}


int WinMain(int argc, char *argv[]) {
    int ret = -1;

    AVFormatContext *pFormatCtx = NULL; //for opening multi-media file

    int i, videoStream;

    AVCodecParameters *pCodecParameters = NULL; //codec context
    AVCodecContext *pCodecCtx = NULL;

    AVCodec *pCodec = NULL; // the codecer
    AVFrame *pFrame = NULL;
    AVPacket packet;

    SDL_Rect rect;
    Uint32 pixformat;

    char *file = "C:\\Users\\lenovo\\Desktop\\fengjing.mp4";
    //for render
    SDL_Window *win = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;

    SDL_Thread *video_thread;
    SDL_Event event;

    //set defualt size of window
    int w_width = 640;
    int w_height = 480;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL - %s\n", SDL_GetError());
        return ret;
    }


    // Open video file
    if (avformat_open_input(&pFormatCtx, file, NULL, NULL) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open video file!");
        goto __FAIL; // Couldn't open file
    }

    videoStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    if (videoStream == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Din't find a video stream!");
        goto __FAIL;// Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    pCodecParameters = pFormatCtx->streams[videoStream]->codecpar;

    // Find the decoder for the video stream
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unsupported codec!\n");
        goto __FAIL; // Codec not found
    }

    // Copy context
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(pCodecCtx, pCodecParameters) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't copy codec context");
        goto __FAIL;// Error copying codec context
    }

    // Open codec
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open decoder!\n");
        goto __FAIL; // Could not open codec
    }

    // Allocate video frame
    pFrame = av_frame_alloc();

    w_width = pCodecCtx->width;
    w_height = pCodecCtx->height;

    win = SDL_CreateWindow("Media Player",
                           SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED,
                           w_width, w_height,
                           SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window by SDL");
        goto __FAIL;
    }

    renderer = SDL_CreateRenderer(win, -1, 0);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create Renderer by SDL");
        goto __FAIL;
    }

    pixformat = SDL_PIXELFORMAT_IYUV;
    texture = SDL_CreateTexture(renderer,
                                pixformat,
                                SDL_TEXTUREACCESS_STREAMING,
                                w_width,
                                w_height);

    SDL_CreateThread(video_refresh_thread, "Video Thread", NULL);


    for (;;) {
        SDL_WaitEvent(&event);
        if (event.type == REFRESH_EVENT) {
            while (1) {
                if (av_read_frame(pFormatCtx, &packet) < 0)
                    thread_exit = 1;


                if (packet.stream_index == videoStream)
                    break;
            }

            if (packet.stream_index == videoStream) {

                avcodec_send_packet(pCodecCtx, &packet);
                while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {

                    SDL_UpdateYUVTexture(texture, NULL,
                                         pFrame->data[0], pFrame->linesize[0],
                                         pFrame->data[1], pFrame->linesize[1],
                                         pFrame->data[2], pFrame->linesize[2]);

                    // Set Size of Window
                    rect.x = 0;
                    rect.y = 0;
                    rect.w = pCodecCtx->width;
                    rect.h = pCodecCtx->height;

                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, &rect);
                    SDL_RenderPresent(renderer);
                }
                av_packet_unref(&packet);
            }
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_SPACE) {
                thread_pause = !thread_pause;
            }
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                thread_exit = 1;
            }
        } else if (event.type == SDL_QUIT) {
            thread_exit = 1;
        } else if (event.type == BREAK_EVENT) {
            break;
        }
    }

    __QUIT:
    ret = 0;

    __FAIL:
    // Free the YUV frame
    if (pFrame) {
        av_frame_free(&pFrame);
    }

    // Close the codec
    if (pCodecCtx) {
        avcodec_close(pCodecCtx);
    }


    if (pCodecParameters) {
        avcodec_parameters_free(&pCodecParameters);
    }

    // Close the video file
    if (pFormatCtx) {
        avformat_close_input(&pFormatCtx);
    }


    if (win) {
        SDL_DestroyWindow(win);
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

    if (texture) {
        SDL_DestroyTexture(texture);
    }

    SDL_Quit();

    return ret;
}
