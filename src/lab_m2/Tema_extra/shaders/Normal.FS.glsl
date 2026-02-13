#version 330

in vec3 text_coord;

uniform samplerCube texture_cubemap;
layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec4 out_color;

void main()
{
    vec3 color = texture(texture_cubemap, normalize(text_coord)).xyz;

    
    out_position = vec3(text_coord) * 0.1; 
    out_normal = vec3(1, 1, 1);                
    out_color = vec4(color, 1.0);            
}