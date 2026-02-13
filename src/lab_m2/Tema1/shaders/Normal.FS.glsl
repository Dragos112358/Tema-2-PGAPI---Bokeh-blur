#version 330

// Input
in vec3 text_coord;

// Uniform properties
uniform samplerCube texture_cubemap;
uniform float time;
// Output
layout(location = 0) out vec4 out_color;

in vec3 frag_pos;
in vec3 gNormal;


void main()
{
    vec3 N = normalize(gNormal);
    vec3 texColor = texture(texture_cubemap, text_coord).rgb;
    float intensity = 10.0f * sin(time) * sin(time);
    vec3 lightColor = vec3(intensity);
    vec3 lightPos = vec3(18.0, 8.2f, 5.0f);

    vec3 lightDir = normalize(vec3(-9, 0, 0));
    vec3 L = normalize(lightPos - frag_pos);

    float spotAngle = radians(90.0);
    float angleFactor = dot(-L, lightDir);

    vec3 finalColor = vec3(0.0);

    if (angleFactor > cos(spotAngle))
    {
        float diff = max(dot(N, L), 0.0);

        float spec = 0.0;
        if (diff > 0.0)
        {
            vec3 V = normalize(-frag_pos);
            vec3 R = reflect(-L, N);
            spec = pow(max(dot(R, V), 0.0), 32.0);
        }
        float dist = length(lightPos - frag_pos);
        float attenuation = 1.0 / (dist + 3.0f);
        vec3 ambient  = 0.15 * texColor;
        vec3 diffuse  = diff * texColor * lightColor;
        vec3 specular = spec * lightColor;
        finalColor = ambient + attenuation * (diffuse + specular);
    }
    else
    {
        finalColor = 0.01 * texColor;
    }

    out_color = vec4(finalColor, 1.0);
}
