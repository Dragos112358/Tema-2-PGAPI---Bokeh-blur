#version 330

in vec2 texture_coord;
in vec3 world_position;
in vec3 world_normal;

uniform sampler2D texture_1;
uniform sampler2D depth_texture;
uniform vec3 eye_position;
uniform vec3 light_direction;
uniform vec3 light_color;

uniform mat4 light_space_view;
uniform mat4 light_space_projection;
uniform float light_space_far_plane;

layout(location = 0) out vec4 out_color;

const vec3 KA = vec3(0.25);
const vec3 KD = vec3(0.5);
const vec3 KS = vec3(0.3);
const float SPECULAR_EXPONENT = 40.0;
const vec3 SHADOW_COLOR = vec3(0.2, 0.2, 0.4);

vec3 PhongLight(vec3 L)
{
    vec3 N = normalize(world_normal);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = KD * diff;
    vec3 specular = vec3(0.0);

    if (diff > 0.0)
        specular = KS * pow(max(dot(N, H), 0.0), SPECULAR_EXPONENT);

    return diffuse + specular;
}

float ShadowFactor()
{
    vec4 light_space_pos = light_space_projection * light_space_view * vec4(world_position, 1.0);
    light_space_pos /= light_space_pos.w;

    vec2 depth_map_pos = light_space_pos.xy * 0.5 + 0.5;
    if (depth_map_pos.x < 0.0 || depth_map_pos.x > 1.0 ||
        depth_map_pos.y < 0.0 || depth_map_pos.y > 1.0)
        return 1.0;

    float light_space_depth = light_space_pos.z * 0.5 + 0.5;
    if (light_space_depth < 0.0 || light_space_depth > 1.0)
        return 1.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depth_texture, 0);
    vec3 N = normalize(world_normal);
    vec3 L = normalize(-light_direction);
    float bias = max(0.005 * (1.0 - dot(N, L)), 0.001);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            vec2 offset = vec2(x, y) * texelSize;
            float pcfDepth = texture(depth_texture, depth_map_pos + offset).r;
            shadow += (light_space_depth - bias < pcfDepth) ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;
    return shadow;
}

void main()
{
    vec4 texColor = texture(texture_1, texture_coord);
    if (texColor.a < 0.1)
        discard;

    vec3 L = normalize(-light_direction);
    vec3 lighting = PhongLight(L);
    float shadow = ShadowFactor();

    vec3 color = (KA + mix(SHADOW_COLOR, lighting, shadow)) * texColor.rgb;
    out_color = vec4(color, texColor.a);
}
