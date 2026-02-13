#version 330

in vec2 gUV;
in vec3 frag_pos;
in vec3 gNormal;
in vec4 frag_pos_light_space;
flat in int is_tv;

uniform sampler2D myTexture;
uniform sampler2D shadowMap;
uniform float time;

uniform vec3 light_position;
uniform vec3 light_direction;
uniform float light_intensity;
uniform float spot_angle;
uniform vec3 camera_position;
uniform vec3 light_color;


layout(location = 0) out vec4 out_color;

float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;  // Outside light frustum = lit
    
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
    
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

void main()
{
    vec4 texColor = texture(myTexture, gUV);
    vec3 N = normalize(gNormal);

    if (gl_FrontFacing) {
        N = -N;
    }
    
    // Direction from fragment to light
    vec3 L = normalize(light_position - frag_pos);
    
    // Spotlight calculation
    float theta = dot(-L, normalize(light_direction));
    float innerCutoff = cos(radians(spot_angle));
    float outerCutoff = cos(radians(spot_angle + 15.0));
    float spotIntensity = clamp((theta - outerCutoff) / (innerCutoff - outerCutoff), 0.0, 1.0);
    
    // Shadow
    float shadow = CalculateShadow(frag_pos_light_space, N, L);
    
    // Diffuse lighting
    float diff = max(dot(N, L), 0.0);
    
    // Specular (Blinn-Phong)
    vec3 V = normalize(camera_position - frag_pos);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 32.0);
    
    // Distance attenuation
    float dist = length(light_position - frag_pos);
    float attenuation = 1.0 / (1.0 + 0.01 * dist + 0.001 * dist * dist);
    
    // Combine
    vec3 ambient = 0.2 * * light_color * texColor.rgb;  // Higher ambient so scene isn't pitch black
    vec3 diffuse = diff * texColor.rgb * light_color * light_intensity;
    vec3 specular = spec * vec3(0.3) * light_intensity * light_color;
    
    vec3 lighting = (diffuse + specular) * attenuation * spotIntensity * (1.0 - shadow);
    vec3 finalColor = ambient + lighting;
    
    out_color = vec4(finalColor, texColor.a);
}
