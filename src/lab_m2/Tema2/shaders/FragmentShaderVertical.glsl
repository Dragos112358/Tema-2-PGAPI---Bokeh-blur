#version 330

in vec2 tex_coord;
out vec4 out_color;

uniform sampler2D u_texture;
uniform vec2 screenSize;
uniform int blurRadius; // Primit din taste (UP/DOWN)

void main()
{
    vec4 centerPixel = texture(u_texture, tex_coord);
    float blurAmount = centerPixel.a; 
    float currentRadius = float(blurRadius) * blurAmount;
    int r = int(currentRadius);
    /*if (r < 1) {
        out_color = vec4(centerPixel.rgb, 1.0);
        return;
    }*/

    vec3 colorSum = vec3(0.0);
    float weightSum = 0.0;
    float sigma1 = max(currentRadius * 0.5, 0.1);
    float sigma2 = sigma1 * 1.5;

    vec2 texelSize = 1.0 / screenSize;

    for (int i = -r; i <= r; i++) {
        float yOffset = float(i);
        float w1 = exp(-(yOffset * yOffset) / (2.0 * sigma1 * sigma1));
        float w2 = exp(-(yOffset * yOffset) / (2.0 * sigma2 * sigma2));
        float weight = (0.6 * w1) + (0.4 * w2);
        
        vec2 sampleCoord = tex_coord + vec2(0.0, yOffset * texelSize.y);
        sampleCoord = clamp(sampleCoord, 0.0, 1.0);
        vec3 sampleColor = texture(u_texture, sampleCoord).rgb;
        sampleColor = pow(sampleColor, vec3(2.2)); 

        colorSum += sampleColor * weight;
        weightSum += weight;
    }
    vec3 finalRGB = pow(colorSum / weightSum, vec3(1.0 / 2.2));

    out_color = vec4(finalRGB, 1.0);
}