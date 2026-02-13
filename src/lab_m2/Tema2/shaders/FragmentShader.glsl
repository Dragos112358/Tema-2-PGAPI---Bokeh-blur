#version 330

// Input
in vec2 texture_coord;

// Uniform properties
uniform sampler2D textureImage;
uniform ivec2 screenSize;
uniform int flipVertical;
uniform int outputMode; // 0: original, 1: grayscale, 2: blur, 3: median, 4: canny
layout(location = 0) out vec4 out_color;
vec2 uv_flipped = vec2(texture_coord.x, (flipVertical != 0) ? 1 - texture_coord.y : texture_coord.y);

float get_pixel_int_255(ivec2 coord)
{
    vec4 color = texelFetch(textureImage, coord, 0);
    float gray01 = 0.21 * color.r + 0.71 * color.g + 0.07 * color.b;
    return floor(gray01 * 255.0);
}

float get_blur_int_255(ivec2 centerCoord)
{
    float sum = 0.0;
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            sum += get_pixel_int_255(centerCoord + ivec2(i, j));
        }
    }
    return floor(sum / 9.0);
}

vec4 canny_cpu_match()
{
    ivec2 texSize = textureSize(textureImage, 0);
    ivec2 pixelCoord = ivec2(uv_flipped * texSize);
    if (pixelCoord.x < 1 || pixelCoord.y < 1 || pixelCoord.x >= texSize.x - 1 || pixelCoord.y >= texSize.y - 1)
    {
        return vec4(0, 0, 0, 1);
    }
    float Gx = 0.0;
    float Gy = 0.0;

    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            float val = get_blur_int_255(pixelCoord + ivec2(i, j));
            float wx = 0.0;
            if (i == -1) wx = (j == 0) ? -2.0 : -1.0;
            if (i ==  0) wx = 0.0;
            if (i ==  1) wx = (j == 0) ?  2.0 :  1.0;
            float wy = 0.0;
            if (j == -1) wy = (i == 0) ?  2.0 :  1.0; 
            if (j ==  0) wy = 0.0;
            if (j ==  1) wy = (i == 0) ? -2.0 : -1.0;

            Gx += val * wx;
            Gy += val * wy;
        }
    }
    float magnitude = sqrt(Gx * Gx + Gy * Gy);
    if (magnitude > 100.0)
    {
        return vec4(1.0, 1.0, 1.0, 1.0); // Alb
    }
    else
    {
        return vec4(0.0, 0.0, 0.0, 1.0); // Negru
    }
}
vec4 grayscale() { return vec4(vec3(get_pixel_int_255(ivec2(uv_flipped * textureSize(textureImage, 0))) / 255.0), 1.0); }
vec4 blur(int r) { return texture(textureImage, uv_flipped); }
vec4 median() { return texture(textureImage, uv_flipped); }

void main()
{
    switch (outputMode)
    {
        case 1: out_color = grayscale(); break;
        case 2: out_color = blur(3); break;
        case 3: out_color = median(); break;
        case 4: 
            out_color = canny_cpu_match(); 
            break;
        default: out_color = texture(textureImage, uv_flipped); break;
    }
}