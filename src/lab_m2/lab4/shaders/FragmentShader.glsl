#version 330

in vec3 gColor;   
// Output
layout(location = 0) out vec4 out_color;


void main()
{
    out_color = vec4(gColor, 1.0);
}
