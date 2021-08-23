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
#include "gaurad_shader.h"
#include "normal_map_shader.h"
#include "darboux_shader.h"

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
using simple_graphics::IShader;
using simple_graphics::SimpleColor;
using simple_graphics::SimpleGraphics;
using simple_graphics::ModelView;
using simple_graphics::Projection;
using simple_graphics::Viewport;
//using simple_graphics::GauraudShader;
using simple_graphics::NormalMapShader;
using simple_graphics::DarbouxShader;
using simple_graphics::DrawZBuffer;
using simple_graphics::ShadowZBuffer;

constexpr float PI = 3.14159265359;

class ShadowShader : public IShader {
public:
    ShadowShader(Model* model) : model_(model) {}

    // Inherited via IShader
    virtual vec<3> vertex(int iface, int nthvert) override
    {
        vec3 vert = 
         from_homogenous(Viewport * Projection * ModelView * embed<4>(model_->vert(iface, nthvert)));
        verts_.set_col(nthvert, vert);
        return vert;
    }
    virtual bool fragment(const vec<3>& bar, SimpleColor& color) override
    {
        float z = ((verts_ * bar).z + 1) /2.0;
        color = SimpleColor(255, 255, 255, 255) * z;
        return false;
    }
private :
    Model* model_;
    mat<3,3> verts_;
};

void DrawShader(SimpleGraphics& graphics, Model& model, IShader& shader, simple_graphics::ZBuffer& z_buffer) {
    const vec3 forward(0, 0, -1);
    for (int i = 0; i < model.nfaces(); i++) {
        vec3 triangle[3];
        for (int j = 0; j < 3; j++) {
            triangle[j] = shader.vertex(i, j);
        }
        if (graphics.DirectionalComponent(triangle, forward) < -0.01) {
            continue;
        }
        graphics.DrawTriangle(triangle, shader, z_buffer);
    }
}

class GrayWall : public Model {
public:

    GrayWall(float width, float height, float z) : width_(width), height_(height), z_(z) {}

    virtual int nverts() const override {
        return 6;
    }
    virtual int nfaces() const override { return 2; };
    virtual vec3 normal(const int iface, const int nthvert) const override {
        return vec3(0, 0, 1);
    };  // per triangle corner normal vertex
    virtual vec3 normal(const vec2& uv) const override {
        return vec3(0, 0, 1);
    }                      // fetch the normal vector from the normal map texture
    virtual vec3 vert(const int i) const override {
        assert(false);
        return vec3(0, 0, 0);
    }
    virtual vec3 vert(const int iface, const int nthvert) const override {
        vec3 ret(0, 0, 0);
        if (iface == 0) {
            vec3 vals[3] = { vec3(-1,-1,z_), vec3(-1, 1,z_), vec3(1,-1, z_) };
            ret =  vals[2 - nthvert];
        }
        else if (iface == 1) {
            vec3 vals[3] = { vec3(-1,1,z_), vec3(1, 1,z_), vec3(1,-1,z_) };
            ret =  vals[2- nthvert];
        }
        
        ret.x *= (width_ / 2);
        ret.y *= (height_ / 2);
        return ret;
    };
    virtual vec2 uv(const int iface, const int nthvert) const override {
        if (iface == 0) {
            vec2 vals[3] = { vec2(0,0), vec2(0, 1), vec2(1,0) };
            return vals[2- nthvert];
        }
        else if (iface == 1) {
            vec2 vals[3] = { vec2(0,1), vec2(1, 1), vec2(1,0) };
            return vals[2 - nthvert];
        }
        return vec2(0, 0);
    }
    virtual TGAColor diffuse(const vec2& uv) const override { return TGAColor(255, 128, 128); }
    virtual double specular(const vec2& uv) const override { return 4; };

private:
    float width_;
    float height_;
    double z_ = -1;

};

VOID OnPaint(HDC hdc, long time)
{
    SimpleColor red(255, 0, 0, 255);
    SimpleColor blue(0, 0, 255, 255);
    SimpleColor green(0, 255, 0, 255);
    SimpleGraphics graphics(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
   
    graphics.SetColor(red);
    //Model model(R"(C:/Users/mhoro/OneDrive/Documents/diablo_pose/diablo3_pose.obj)");
    //Model model("C:/Users/mhoro/OneDrive/Documents/AfricanHead/african_head.obj");
    
    std::vector<Model*> models = { //new FileModel("C:/Users/mhoro/OneDrive/Documents/boggie/head.obj"),
       //new FileModel("C:/Users/mhoro/OneDrive/Documents/boggie/body.obj"),
        new GrayWall(0.2,0.2,0),
        new GrayWall(0.2,0.2,-1)
        //new GrayWall(0.5,0.4,-1)
        //new FileModel("C:/Users/mhoro/OneDrive/Documents/AfricanHead/african_head.obj")
    };
    
    float radian = PI /2 - 4 * PI * time / 2000;
     vec3 light_direction(std::sin(radian), 0.5, std::cos(radian));
   //vec3 light_direction(2, 0, 0);
    light_direction.normalize();
    
    vec3 center(0.00, 0.000, 0);
    vec3 up(0.000, 1, 0.00);
    simple_graphics::lookat(light_direction, center, up);
    //simple_graphics::lookat(light_direction, center, up);
    simple_graphics::projection(0);
    //simple_graphics::viewport(0, 0, ShadowZBuffer.width , ShadowZBuffer.height);
    float scale = 2.5;
    simple_graphics::viewport(scale , scale , ShadowZBuffer.width /scale, ShadowZBuffer.height /scale);
    //Matrix m_inverse_transpose = ModelView.transpose().inverse();
    mat<4, 4> shadow_viewport_projection_view_mat =  Viewport * Projection * ModelView;
    
    for ( Model* model : models) {
        ShadowShader shadow_shader(model);
        DrawShader(graphics, *model, shadow_shader, DrawZBuffer);
    }

    vec3 eye(0, 0, 300);
    center = vec3(0, 0, 0);
    simple_graphics::lookat(eye, center, up);
    simple_graphics::projection(-1.f / (100));
    simple_graphics::viewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    /*
    for (Model* model : models) {
        NormalMapShader shader(&graphics, shadow_viewport_projection_view_mat, Projection * ModelView, model, light_direction);
        DrawShader(graphics, *model, shader, DrawZBuffer);
    }*/

    for (Model* model : models) {
        delete model;
    }

    
    /*
    simple_graphics::projection();
    simple_graphics::viewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    simple_graphics::lookat(eye, center, up);
    mat<4, 4> projection_view_mat = Projection * ModelView;
    NormalMapShader draw_shader(&graphics, shadow_projection_view_mat,projection_view_mat, &model, light_direction);
    DrawShader(graphics, model, draw_shader, false);*/
    //GauraudShader shader(&model, &tga_img, light_direction);
    //DarbouxShader shader(&model, &tga_img, &normal_map_tangent, light_direction);
   
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

RECT rcCurrent = { 0,0,0,0 };
POINT aptStar[6] = { 10,1, 1,19, 19,6, 1,6, 19,19, 10,1 };
int X = 2, Y = -1, idTimer = -1;
BOOL fVisible = FALSE;
HDC hdc;
long passed_time = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
   
    PAINTSTRUCT  ps;
    
    RECT rc;

    switch (message)
    {
    case WM_CREATE:
        // Calculate the starting point.  

        GetClientRect(hWnd, &rc);
        //OffsetRect(&rcCurrent, rc.right / 2, rc.bottom / 2);

        // Initialize the private DC.  

        hdc = GetDC(hWnd);
        SetViewportOrgEx(hdc, rcCurrent.left,
            rcCurrent.top, NULL);
        SetROP2(hdc, R2_NOT);

        // Start the timer.  

        SetTimer(hWnd, idTimer = 1, 1, NULL);
        return 0;
    case WM_TIMER:
        passed_time += 10;
        GetClientRect(hWnd, &rc);
        SetViewportOrgEx(hdc, rcCurrent.left,
            rcCurrent.top, NULL);
        OnPaint(hdc, passed_time);
        return 0;
    case WM_PAINT:
            BeginPaint(hWnd, &ps);
            
            EndPaint(hWnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
} // WndProc