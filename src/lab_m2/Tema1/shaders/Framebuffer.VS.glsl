#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform int cube_firework;
out vec3 geom_position;
out vec2 geom_texture_coord;

void main()
{
    geom_position = v_position;
    geom_texture_coord = v_texture_coord;
    if (cube_firework == 0) {
    gl_Position = Model * vec4(v_position, 1); 
    }
    else {
        gl_Position = Projection * View * Model * vec4(v_position, 1); 
    }
}
