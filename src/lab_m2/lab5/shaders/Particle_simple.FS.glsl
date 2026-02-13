#version 430

// Input
layout(location = 0) in vec2 texture_coord;

// Uniform properties
uniform sampler2D texture_1;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec2 uv = texture_coord - vec2(0.5);
    float r = length(uv);               
    float angle = atan(uv.y, uv.x);

    // Base radius of the circle
    float radius = 0.25;

    // Sinusoidal perturbation around the edge
    float wave = 0.03 * sin(12.0 * angle);
    float r_mod = radius + wave;

    if (r <= r_mod)
    {
        out_color = vec4(1.0, 1.0, 0.0, 1.0); // yellow
    }
    else
    {
        discard; // outside
    }
}
