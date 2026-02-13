#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output
out vec3 text_coord;


void main()
{
    vec3 centered = v_position + vec3(0.25f, 0.25f, 0.25f); // if mesh is 0..1
    text_coord = normalize(centered);

    //gl_Position = Projection * View * Model * vec4(v_position, 1); 
    mat4 viewNoTranslation = mat4(mat3(View));

    gl_Position = Projection * viewNoTranslation * Model * vec4(v_position, 1.0);
}
