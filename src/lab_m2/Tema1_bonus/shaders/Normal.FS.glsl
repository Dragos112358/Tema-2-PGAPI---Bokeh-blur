#version 330

// Input
in vec3 tex_coords;

// Uniform properties
uniform samplerCube texture_cubemap;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec3 color = texture(texture_cubemap, tex_coords).xyz;
    out_color = vec4(color, 1.0);
}
