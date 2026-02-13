#version 330

layout(location = 0) in vec3 v_position;

uniform mat4 Model;

flat out int instance;

void main()
{
    instance = gl_InstanceID;

    gl_Position = vec4(v_position, 1.0);
}
