#version 330

in vec2 gUV;
in vec3 frag_pos;
in vec3 gNormal;
flat in int is_tv;

uniform sampler2D myTexture;
uniform sampler2D myTexture2;
uniform float time;
uniform float x_direction;
uniform float y_direction;
uniform float z_direction;

layout(location = 0) out vec4 out_color;

void main()
{
    vec3 finalColor = vec3(0.0); 
    vec4 texColor;
    vec3 N = normalize(gNormal);
    //vec3 lightPos = vec3(18.0 , 8.2 + 5.0 * sin(time), 5.0);
    float intensity = 10.0f* sin(time)* sin(time);
    vec3 lightColor = vec3(intensity,intensity, intensity);
    /*if(is_tv == 0) {
        texColor = texture(myTexture, gUV);
    }
    else
    {
        texColor = texture(myTexture2, gUV);
    }*/
     texColor = texture(myTexture, gUV);
    //vec3 lightPos = vec3(18.0 , 8.2 + 5.0 * sin(time), 5.0);
    vec3 lightPos = vec3(18.0, 8.2f, 5.0f);
   // vec3 lightDir = normalize(vec3(x_direction,y_direction,z_direction)); // lumina merge în jos
   vec3 lightDir = normalize(vec3(-9,0,0));
    vec3 L = normalize(lightPos - frag_pos);
    float spotAngle = radians(90.0); // unghiul cone-ului
    float angleFactor = dot(-L, lightDir);

    if(angleFactor > cos(spotAngle)) {
        float diff = max(dot(N, L), 0.0);

        float spec = 0.0;
        if(diff > 0.0) {
            vec3 V = normalize(-frag_pos);
            vec3 R = reflect(-L, N);
            spec = pow(max(dot(R, V), 0.0), 32.0);
        }

        float dist = length(lightPos - frag_pos);
        float attenuation = 1.0 / (dist + 3.0f);

        vec3 ambient  = 0.2 * texColor.rgb;
        vec3 diffuse  = diff * texColor.rgb * lightColor;
        vec3 specular = spec * lightColor;

        finalColor = ambient + attenuation * (diffuse + specular);

        //out_color = vec4(finalColor, texColor.a);
    } else {
        finalColor = vec3(0.20 * texColor.rgb); // doar ambient
    }
    vec4 texColor2 = texture(myTexture, gUV); 
    //out_color = texColor2;
    out_color = vec4(finalColor, texColor.a);
}
