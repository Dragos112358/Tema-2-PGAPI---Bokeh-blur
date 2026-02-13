#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniform properties
uniform samplerCube texture_cubemap;
uniform int type;
uniform int direction;

uniform vec3 camera_position;

// Output
layout(location = 0) out vec4 out_color;


vec3 myReflect()
{
    // TODO(student): Compute the reflection color value
    vec3 I = normalize(world_position - camera_position);
    //vec3 I = normalize(world_position - camera_position);
    vec3 R = reflect(I, normalize(world_normal));
    return texture(texture_cubemap, R).xyz;

}


vec3 myRefract(float refractive_index)
{
    // TODO(student): Compute the refraction color value
    vec3 I = normalize(world_position - camera_position);
    vec3 R = refract(I, normalize(world_normal),1.0f/refractive_index);
    return texture(texture_cubemap,R).xyz;

}

vec3 myReflect_inv()
{
    // TODO(student): Compute the reflection color value
    vec3 I = normalize(camera_position - world_position);
    //vec3 I = normalize(world_position - camera_position);
    vec3 R = reflect(I, normalize(world_normal));
    R.y = -R.y;
    R.x = -R.x;
    return texture(texture_cubemap, R).xyz;

}


vec3 myRefract_inv(float refractive_index)
{
    // TODO(student): Compute the refraction color value
    vec3 I = normalize(camera_position - world_position);
    vec3 R = refract(I, normalize(world_normal),1.0f/refractive_index);
    return texture(texture_cubemap,R).xyz;

}


void main()
{

    if (type == 0 && direction == 0)
    {
        out_color = vec4(myReflect(), 0.0);
    }
    if (type == 0 && direction == 1) {
        out_color = vec4(myReflect_inv(), 0.0);
    }
    if (type == 1 && direction == 0) {
        out_color = vec4(myRefract(1.33), 0);
    }
    if (type == 1 && direction == 1) {
        out_color = vec4(myRefract_inv(1.33), 0);
    }

}
