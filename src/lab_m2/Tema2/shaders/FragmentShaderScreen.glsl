#version 330

in vec2 tex_coord;
out vec4 out_color;

uniform sampler2D texOriginal;
uniform sampler2D texPass1;
uniform sampler2D texPass2;

uniform int viewMode;
uniform vec2 screenSize;
uniform vec2 focusPos;
uniform float focusRadius;

float drawSmoothRing(float currentDist, float targetRadius, float thickness) {
    float d = abs(currentDist - targetRadius);
    return 1.0 - smoothstep(thickness - 1.0, thickness, d);
}

void main()
{
    vec4 finalColor;
    
    // 1. COORDONATA PENTRU TEXTURA (INVERSATA)
    vec2 flippedCoord = vec2(tex_coord.x, 1.0 - tex_coord.y);

    // --- MODIFICARE AICI: Alegem textura in functie de viewMode ---
    if (viewMode == 0 || viewMode == 5) {
        // Modul 0 si Modul 5 folosesc ambele imaginea originala
        finalColor = texture(texOriginal, flippedCoord);
    } 
    else if (viewMode == 1) {
        finalColor = texture(texPass1, flippedCoord);
    } 
    else {
        // Modul 2, 3 si 4 folosesc imaginea procesata (Blur)
        finalColor = texture(texPass2, flippedCoord);
    }
    if (viewMode == 4 || viewMode == 5) {
        vec2 fragPos = tex_coord * screenSize; 
        float dist = distance(fragPos, focusPos);
        float lineThickness = 2.0; 
        vec3 ringColor = vec3(1.0, 1.0, 1.0);
        float ringOpacity = 0.7;

        float totalRingIntensity = 0.0;
        totalRingIntensity += drawSmoothRing(dist, focusRadius * 1.0, lineThickness);
        totalRingIntensity += drawSmoothRing(dist, focusRadius * 0.66, lineThickness);
        totalRingIntensity += drawSmoothRing(dist, focusRadius * 0.33, lineThickness);
        totalRingIntensity += drawSmoothRing(dist, focusRadius * 0.16, lineThickness);
        totalRingIntensity = min(totalRingIntensity, 1.0);
        finalColor.rgb = mix(finalColor.rgb, ringColor, totalRingIntensity * ringOpacity);
        float dotSize = 3.5;
        float dotIntensity = 1.0 - smoothstep(dotSize - 1.0, dotSize, dist);
        finalColor.rgb = mix(finalColor.rgb, vec3(1, 0, 0), dotIntensity);
    }

    out_color = finalColor;
}