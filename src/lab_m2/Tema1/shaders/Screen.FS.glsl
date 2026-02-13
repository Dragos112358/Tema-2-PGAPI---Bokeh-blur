#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniform properties
uniform samplerCube texture_cubemap;
uniform sampler2D fireworks_texture;

uniform int type;
uniform int direction;

uniform vec3 camera_position;

// Output
layout(location = 0) out vec4 out_color;
in vec2 gUV;
in float aspect_correction;

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
    vec4 out_color2;
    if (type == 0 && direction == 0)
    {
        out_color2 = vec4(myReflect(), 0.0);
    }
    if (type == 0 && direction == 1) {
        out_color2 = vec4(myReflect_inv(), 0.0);
    }
    if (type == 1 && direction == 0) {
        out_color2 = vec4(myRefract(1.33), 0);
    }
    if (type == 1 && direction == 1) {
        out_color2 = vec4(myRefract_inv(1.33), 0);
    }
    vec3 fireworkColor = texture(fireworks_texture, gUV).rgb;
    vec3 finalColor = mix(out_color2.rgb, fireworkColor, 0.5);
    out_color = vec4(finalColor, 1.0);

}


/*#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniform properties
uniform samplerCube texture_cubemap;
uniform sampler2D fireworks_texture;

uniform int type;
uniform int direction;

uniform vec3 camera_position;

// Output
layout(location = 0) out vec4 out_color;
in vec2 gUV;
in float aspect_correction;

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
    vec4 out_color2;
    if (type == 0 && direction == 0)
    {
        out_color2 = vec4(myReflect(), 0.0);
    }
    if (type == 0 && direction == 1) {
        out_color2 = vec4(myReflect_inv(), 0.0);
    }
    if (type == 1 && direction == 0) {
        out_color2 = vec4(myRefract(1.33), 0);
    }
    if (type == 1 && direction == 1) {
        out_color2 = vec4(myRefract_inv(1.33), 0);
    }
    //vec3 fireworkColor = texture(fireworks_texture, gUV).rgb;
     vec2 correctedUV = vec2(gUV.x * aspect_correction, gUV.y);
    vec3 fireworkColor = texture(fireworks_texture, correctedUV).rgb;
    vec3 finalColor = mix(out_color2.rgb, fireworkColor, 0.5);
    out_color = vec4(finalColor, 1.0);

}
*/
