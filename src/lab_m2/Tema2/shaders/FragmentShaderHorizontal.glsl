#version 330

in vec2 tex_coord;
out vec4 out_color;

uniform sampler2D u_texture;
uniform vec2 screenSize;
uniform vec2 focusPos;
uniform float focusRadius;
uniform int blurRadius; // Primit din C++ (tastele UP/DOWN)

void main()
{
    vec2 flippedCoord = vec2(tex_coord.x, 1.0 - tex_coord.y);
    vec2 fragPos = flippedCoord * screenSize; 
    float dist = distance(fragPos, focusPos);
    float t = clamp((dist - focusRadius) / (focusRadius * 2.0), 0.0, 1.0);
    float blurAmount = pow(t, 1.001); 
    float noise = fract(sin(dot(tex_coord, vec2(12.9898, 78.233))) * 43758.5453) - 0.5;
    float currentRadius = float(blurRadius) * blurAmount + noise;
    int r = int(max(0.0, currentRadius));

    vec3 colorSum = vec3(0.0);
    float weightSum = 0.0;
    vec2 texelSize = 1.0 / screenSize;
    float sigma1 = max(float(r) * 0.5, 0.1);
    float sigma2 = sigma1 * 1.5;

    for (int i = -r; i <= r; i++) {
        float xOffset = float(i);
        float g1 = exp(-(xOffset * xOffset) / (2.0 * sigma1 * sigma1));
        float g2 = exp(-(xOffset * xOffset) / (2.0 * sigma2 * sigma2));
        float weight = (0.6 * g1) + (0.4 * g2);
        
        vec2 sampleCoord = tex_coord + vec2(xOffset * texelSize.x, 0.0);
        sampleCoord = clamp(sampleCoord, 0.0, 1.0);
        vec3 col = texture(u_texture, sampleCoord).rgb;
        col = pow(col, vec3(2.2)); 

        colorSum += col * weight;
        weightSum += weight;
    }
    vec3 finalRGB = pow(colorSum / weightSum, vec3(1.0 / 2.2));
    out_color = vec4(finalRGB, blurAmount);
}