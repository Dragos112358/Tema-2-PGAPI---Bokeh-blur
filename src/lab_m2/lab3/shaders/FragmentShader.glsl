#version 330

// Input
in vec2 f_texture_coord;

// Uniform properties
uniform sampler2D texture_1;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec4 tex_color = texture(texture_1, f_texture_coord);
    if (tex_color.a < 0.75)
        discard;
    out_color = tex_color;
}
