#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
#include "SimpleGraphics.h"
#include <iostream>
#include "model.h"
#include <assert.h>
#include "tgaimage.h"
#include "geometry.h"

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
using simple_graphics::SimpleColor;
using simple_graphics::SimpleGraphics;


VOID OnPaint(HDC hdc)
{
    SimpleColor red(255, 0, 0, 255);
    SimpleColor blue(0, 0, 255, 255);
    SimpleColor green(0, 255, 0, 255);
    SimpleGraphics graphics(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
   
    graphics.SetColor(red);
    Model model(R"(C:/Users/mhoro/source/repos/TinyRendererWindowsApp/TinyRendererWindowsApp/african_head.obj)");
    Vec3f direction(0, 0, -1);
    Vec3f light_direction(1, 0, -1.1);
    light_direction.normalize();
    TGAImage tga_img;
    tga_img.read_tga_file("C:/Users/mhoro/Downloads/african_head_diffuse.tga");
    tga_img.flip_vertically();
    float image_plane_dist = 10000;
    Matrix projection = Matrix::identity(4);
    projection[3][2] = -1.0f / image_plane_dist;



    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<FaceIndexes> face = model.face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        Vec3f normals[3];
        Vec2f textures[3];
        for (int j = 0; j < 3; j++) {
            const FaceIndexes& indexes = face[j];
            world_coords[j] = model.vert(indexes.vert_idx);
            world_coords[j].z -= 0.3;
            
            Matrix reprojected = projection * world_coords[j].AsHomogenous();

            normals[j] = model.vert_normal(indexes.vert_normal_idx);
            textures[j] = model.texture(indexes.texture_idx);
            screen_coords[j] = Vec3f((reprojected[0][0] / reprojected[3][0] + 1.) * WINDOW_WIDTH / 2., (reprojected[1][0] / reprojected[3][0] + 1.)  *WINDOW_HEIGHT / 2., world_coords[j].z);
        }
        float component = graphics.DirectionalComponent(world_coords, direction);
        if (component < 0) {
            continue;
        }
        graphics.DrawTriangle(screen_coords,normals,textures,tga_img,light_direction);
    }
    /*
    
    tga_img.flip_vertically();
    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            TGAColor color = tga_img.get(x, y);
            graphics.SetColor(SimpleColor(color.bgra[2], color.bgra[1], color.bgra[0], 255));
            graphics.DrawPoint(x, y);
        }
    }*/
    graphics.Render();
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
    HWND                hWnd;
    MSG                 msg;
    WNDCLASS            wndClass;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = TEXT("GettingStarted");

    RegisterClass(&wndClass);

    hWnd = CreateWindow(
        TEXT("GettingStarted"),   // window class name
        TEXT("Getting Started"),  // window caption
        WS_OVERLAPPEDWINDOW,      // window style
        CW_USEDEFAULT,            // initial x position
        CW_USEDEFAULT,            // initial y position
        WINDOW_WIDTH + 20,            // initial x size
        WINDOW_HEIGHT + 50,            // initial y size
        NULL,                     // parent window handle
        NULL,                     // window menu handle
        hInstance,                // program instance handle
        NULL);                    // creation parameters

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    HDC          hdc;
    PAINTSTRUCT  ps;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
} // WndProc