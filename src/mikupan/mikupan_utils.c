#include "mikupan_utils.h"

void MikuPan_ConvertPs2ScreenCoordToNDCMaintainAspectRatio(float* out, float screen_width, float screen_height, float x, float y)
{
    float scale_x = screen_width  / PS2_RESOLUTION_X_FLOAT;
    float scale_y = screen_height / PS2_RESOLUTION_Y_FLOAT;
    float scale   = (scale_x < scale_y) ? scale_x : scale_y;

    float viewport_w = PS2_RESOLUTION_X_FLOAT * scale;
    float viewport_h = PS2_RESOLUTION_Y_FLOAT * scale;

    float viewport_x = (screen_width  - viewport_w) * 0.5f;
    float viewport_y = (screen_height - viewport_h) * 0.5f;

    // Compute destination rectangle in screen space
    float x0 = viewport_x + x * scale;
    float y0 = viewport_y + y * scale;

    // Convert screen space to OpenGL NDC (-1 to 1)
    out[0] = (x0 / screen_width) * 2.0f - 1.0f;
    out[1] = 1.0f - (y0 / screen_height) * 2.0f;
}

float MikuPan_ConvertScaleColor(unsigned char color_fragment)
{
    if (color_fragment <= 127)
    {
        color_fragment = color_fragment << 1;
    }
    else
    {
        color_fragment = 0xFF;
    }

    return (float)color_fragment / 255.0f;
}