#version 330

layout(location = 0) in vec3 v_position;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 tex_coords;

void main()
{
    tex_coords = v_position;
    
    // Remove translation from view matrix (skybox follows camera)
    mat4 viewNoTranslation = mat4(mat3(View));
    
    vec4 pos = Projection * viewNoTranslation * Model * vec4(v_position, 1.0);
    
    // Set z = w so after perspective divide z/w = 1.0 (far plane)
    gl_Position = pos.xyww;
}