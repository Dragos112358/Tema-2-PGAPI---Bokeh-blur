#version 330

// --- INPUTS ---
in vec2 gUV;
in vec3 frag_pos;
in vec3 gNormal;
in vec4 frag_pos_light_space;
flat in int is_tv; // 1 = Obiect reflectant, 0 = Obiect normal

// --- UNIFORMS ---
uniform sampler2D myTexture;
uniform sampler2D shadowMap;
uniform samplerCube texture_cubemap;
uniform sampler2D fireworks_texture;

uniform vec3 light_position;
uniform vec3 light_direction;
uniform float light_intensity;
uniform float spot_angle;
uniform vec3 light_color;
uniform vec3 camera_position;

// --- OUTPUT ---
layout(location = 0) out vec4 out_color;

// -------------------------------------------------------------------------
// HELPER: Calculeaza Umbra (PCF 3x3)
// -------------------------------------------------------------------------
float CalculateShadow(vec4 fragPosLightSpace)
{
    // Perspective divide & transform to [0,1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // Daca suntem in afara frustum-ului luminii, nu e umbra
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = 0.0005; // Bias fix (conform cod original)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    // PCF Loop
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

// -------------------------------------------------------------------------
// HELPER: Calculeaza Spotlight Intensity (Soft Edges)
// -------------------------------------------------------------------------
float CalculateSpotlight(vec3 L, vec3 lightDir)
{
    float theta = dot(-L, normalize(lightDir));
    float innerCutoff = cos(radians(spot_angle));
    float outerCutoff = cos(radians(spot_angle + 15.0)); // +15 grade fade
    
    // Clamp pentru tranzitie lina
    return clamp((theta - outerCutoff) / (innerCutoff - outerCutoff), 0.0, 1.0);
}

// -------------------------------------------------------------------------
// HELPER: Calculeaza Atenuarea distantei
// -------------------------------------------------------------------------
float CalculateAttenuation(vec3 lightPos, vec3 fragPos)
{
    float dist = length(lightPos - fragPos);
    // Coeficienti pastrati din original: 1.0, 0.01, 0.001
    return 1.0 / (1.0 + 0.01 * dist + 0.001 * dist * dist);
}

// -------------------------------------------------------------------------
// MAIN
// -------------------------------------------------------------------------
void main()
{
    vec3 N = normalize(gNormal);
    vec4 baseTexColor = texture(myTexture, gUV);

    // ==========================================
    // 1. REFLEXII (TV / STICLA) - EARLY EXIT
    // ==========================================
    if (is_tv == 1) 
    {
        // Vector incident si reflectat
        vec3 viewDir = normalize(frag_pos - camera_position);
        vec3 reflectDir = reflect(viewDir, N);
        
        // Sampling
        vec3 reflectionColor = texture(texture_cubemap, reflectDir).rgb;
        vec3 fireworkColor   = texture(fireworks_texture, gUV).rgb;
        
        // Mixaj conform originalului: 75% reflexie + 25% artificii
        vec3 finalReflex = (reflectionColor * 0.75) + (fireworkColor * 0.25);
        
        out_color = vec4(finalReflex, 1.0);
        return; // Iesim, nu mai calculam lumina (performanta maxima)
    }

    // ==========================================
    // 2. ILUMINARE STANDARD (Blinn-Phong)
    // ==========================================
    
    // Vectori geometrici
    vec3 L = normalize(light_position - frag_pos);      // Light Vector
    vec3 V = normalize(camera_position - frag_pos);     // View Vector
    vec3 H = normalize(L + V);                          // Halfway Vector

    // Factori de intensitate
    float diffFactor = max(dot(N, L), 0.0);
    float specFactor = pow(max(dot(N, H), 0.0), 32.0); // Shininess 32

    // Calcul componente auxiliare
    float shadow = 0.0;
    if (diffFactor > 0.0) { // Calculam umbra doar daca suprafata e luminata
        shadow = CalculateShadow(frag_pos_light_space);
    }
    
    float spotIntensity = CalculateSpotlight(L, light_direction);
    float attenuation   = CalculateAttenuation(light_position, frag_pos);

    // Combinare componente lumina
    // Ambient (0.2 conform original)
    vec3 ambient  = 0.2 * baseTexColor.rgb * light_color;
    
    // Diffuse & Specular afectate de lumina, umbra, spotlight si atenuare
    vec3 diffColor = diffFactor * baseTexColor.rgb * light_intensity * light_color;
    vec3 specColor = specFactor * vec3(0.3) * light_intensity * light_color; // specular strength 0.3

    vec3 lighting = (diffColor + specColor) * attenuation * spotIntensity * (1.0 - shadow);
    
    // Rezultat final
    out_color = vec4(ambient + lighting, baseTexColor.a);
}