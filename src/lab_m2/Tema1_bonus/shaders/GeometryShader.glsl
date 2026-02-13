#version 330

// Input and output topologies
// TODO(student): First, generate a curve (via line strip),
// then a rotation/translation surface (via triangle strip)
layout(lines) in;
//layout(line_strip) in;

layout(triangle_strip, max_vertices = 300) out;
uniform mat4 Model;
// Uniform properties
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p0, control_p1, control_p2, control_p3;
uniform int no_of_instances;
// TODO(student): Declare any other uniforms here

uniform mat4 viewMatrices[6];
uniform int render_to_cubemap; // 0 = normal, 1 = cubemap

uniform int no_of_generated_points;
uniform float max_translate;
uniform float max_rotate;
uniform int SURFACE_TYPE;
uniform vec2 uvScale;


uniform mat4 LightSpaceMatrix;

out vec4 frag_pos_light_space;


out vec3 gColor;
out vec3 frag_pos;
out vec3 gNormal;
out vec2 gUV;
flat out int is_tv;

// Input
in int instance[2];



vec3 rotateY(vec3 point, float u)
{
    float x = point.x * cos(u) - point.z *sin(u);
    float z = point.x * sin(u) + point.z *cos(u);
    return vec3(x, point.y, z);
}


vec3 translateX(vec3 point, float t)
{
    return vec3(point.x + t, point.y, point.z);
}
vec3 translateY(vec3 point, float t)
{
    return vec3(point.x, point.y + t, point.z);
}
vec3 translateZ(vec3 point, float t)
{
    return vec3(point.x, point.y, point.z + t);
}



vec3 bezier(float t)
{
    return mix(control_p0, control_p3, t); // echivalent cu (1-t)*P0 + t*P3
}

vec3 bezier2(float t) 
{ 
    vec3 base = control_p0 * pow(1.0 - t, 3) + 
                control_p1 * 3.0 * t * pow(1.0 - t, 2) + 
                control_p2 * 3.0 * pow(t, 2) * (1.0 - t) + 
                control_p3 * pow(t, 3); 
    return base; 
}
vec3 bezier_derivative(float t)
{
    // derivata curbei Bezier cubic
    return 3.0 * pow(1.0 - t, 2) * (control_p1 - control_p0)
         + 6.0 * (1.0 - t) * t * (control_p2 - control_p1)
         + 3.0 * t * t * (control_p3 - control_p2);
}

vec3 getColorForAngle(float angleDeg)
{
    angleDeg = mod(angleDeg, 360.0);

    if (angleDeg < 120.0)
        return mix(vec3(1, 0, 0), vec3(0, 1, 0), angleDeg / 120.0);
    else if (angleDeg < 240.0)
        return mix(vec3(0, 1, 0), vec3(0, 0, 1), (angleDeg - 120.0) / 120.0);
    else
        return mix(vec3(0, 0, 1), vec3(1, 0, 0), (angleDeg - 240.0) / 120.0);
}

void EmitVertexForLayer(vec3 position, vec3 color, vec2 uv, vec3 normal, int layer)
{
    // Transform to world space
    vec4 worldPos = Model * vec4(position, 1.0);

    // Transform normal to world space
    vec3 worldNormal = mat3(transpose(inverse(Model))) * normal;
    worldNormal = normalize(worldNormal);

    // Compute view-space position (camera is at origin in view space)
    vec3 viewPos = (View * worldPos).xyz; // position in view space
    // Transform normal to view space for reliable dot product (no translation)
    vec3 viewNormal = mat3(View) * worldNormal; // assuming View has only rotation/translation
    viewNormal = normalize(viewNormal);

    // If normal points away from camera (dot < 0 means normal points opposite camera direction),
    // flip it so it faces the camera.
    // Vector from surface to camera in view space is -viewPos (camera at origin).
    if (dot(viewNormal, -viewPos) < 0.0) {
        worldNormal = -worldNormal;
        viewNormal = -viewNormal;
    }

    gColor = color;
    gUV = uv;
    gNormal = worldNormal; // world-space normal
    frag_pos = worldPos.xyz;

    if (render_to_cubemap == 1) {
        gl_Layer = layer;
        gl_Position = Projection * viewMatrices[layer] * worldPos;
    } else {
        gl_Position = Projection * View * worldPos;
    }

    frag_pos_light_space = LightSpaceMatrix * worldPos;

    EmitVertex();
}



void main()
{
    float brightness1;
    float brightness2;
    const int SURFACE_TYPE_ROTATION     = 0;
    const int SURFACE_TYPE_TRANSLATION  = 1;
    const int SURFACE_TYPE_PLANE = 2;
    const int SURFACE_TYPE_VASE = 4;
    const int SURFACE_TYPE_LAMP = 5;
    const int SURFACE_TYPE_SCREEN = 6;
    //render_to_cubemap2=0;
    const int TV = 3;

    vec3 color;
    int instance_id = instance[0];

    if (instance_id % 3 == 0)
        color = vec3(1.0, 0.0, 0.0);
    else if (instance_id % 3 == 1)
        color = vec3(0.0, 1.0, 0.0);  
    else
        color = vec3(0.0, 0.0, 1.0);

    vec3 baseColor, curveStart, curveEnd;
    vec3 point_row1_a, point_row1_b, point_row2_a, point_row2_b;


    if (SURFACE_TYPE == SURFACE_TYPE_PLANE) {
        curveStart = bezier(0.0);
        curveEnd = bezier(1.0);
    } else {
        curveStart = bezier2(0.0);
        curveEnd = bezier2(1.0);
    }

    float curveLength = length(curveEnd - curveStart);

    if (instance[0] < no_of_instances - 1)
    {
        for (int i = 0; i < no_of_generated_points - 1; i++)
        {
            float t1 = float(i) / float(no_of_generated_points - 1);
            float t2 = float(i + 1) / float(no_of_generated_points - 1);

            float u1 = t1;
            float u2 = t2;

            float v1 = float(instance[0]) / float(no_of_instances - 1);
            float v2 = float(instance[0] + 1) / float(no_of_instances - 1);

            if (SURFACE_TYPE == SURFACE_TYPE_PLANE) {
                curveStart = bezier(0.0);
                curveEnd = bezier(1.0);

                point_row1_a = bezier(t1);
                point_row1_b = bezier(t2);

                point_row2_a = bezier(t1);
                point_row2_b = bezier(t2);
            } else {
                curveStart = bezier2(0.0);
                curveEnd = bezier2(1.0);


                point_row1_a = bezier2(t1);
                point_row1_b = bezier2(t2);

                point_row2_a = bezier2(t1);
                point_row2_b = bezier2(t2);
            }

            if (SURFACE_TYPE == SURFACE_TYPE_TRANSLATION || SURFACE_TYPE == SURFACE_TYPE_PLANE) {
                point_row1_a -= curveStart;
                point_row1_b -= curveStart;
                point_row2_a -= curveStart;
                point_row2_b -= curveStart;
            
                // Scale to unit length if needed
                if (curveLength > 0.0001) {
                    point_row1_a /= curveLength;
                    point_row1_b /= curveLength;
                    point_row2_a /= curveLength;
                    point_row2_b /= curveLength;
                }
            } else if (SURFACE_TYPE == SURFACE_TYPE_ROTATION) {
                // Normalize only Y to make height = 1, keep X radius proportional
                float heightDiff = curveEnd.y - curveStart.y;
                if (heightDiff > 0.0001) {
                    point_row1_a.y = (point_row1_a.y - curveStart.y) / heightDiff;
                    point_row1_b.y = (point_row1_b.y - curveStart.y) / heightDiff;
                    point_row2_a.y = (point_row2_a.y - curveStart.y) / heightDiff;
                    point_row2_b.y = (point_row2_b.y - curveStart.y) / heightDiff;
                }
            } else if (SURFACE_TYPE == SURFACE_TYPE_LAMP) {
                vec3 minBounds = min(curveStart, curveEnd);
                vec3 maxBounds = max(curveStart, curveEnd);
    
                minBounds = min(minBounds, control_p0);
                minBounds = min(minBounds, control_p1);
                minBounds = min(minBounds, control_p2);
                minBounds = min(minBounds, control_p3);
    
                maxBounds = max(maxBounds, control_p0);
                maxBounds = max(maxBounds, control_p1);
                maxBounds = max(maxBounds, control_p2);
                maxBounds = max(maxBounds, control_p3);
    
                vec3 size = maxBounds - minBounds;
    
                if (size.x > 0.0001) {
                    point_row1_a.x = (point_row1_a.x - minBounds.x) / (2.0 * size.x);
                    point_row1_b.x = (point_row1_b.x - minBounds.x) / (2.0 * size.x);
                    point_row2_a.x = (point_row2_a.x - minBounds.x) / (2.0 * size.x);
                    point_row2_b.x = (point_row2_b.x - minBounds.x) / (2.0 * size.x);
                }
    
                // Y stays 0-1 (height = 1)
                if (size.y > 0.0001) {
                    point_row1_a.y = (point_row1_a.y - minBounds.y) / size.y;
                    point_row1_b.y = (point_row1_b.y - minBounds.y) / size.y;
                    point_row2_a.y = (point_row2_a.y - minBounds.y) / size.y;
                    point_row2_b.y = (point_row2_b.y - minBounds.y) / size.y;
                }
    
                // Normalize Z to 0-0.5 range as well
                if (size.z > 0.0001) {
                    point_row1_a.z = (point_row1_a.z - minBounds.z) / (2.0 * size.z);
                    point_row1_b.z = (point_row1_b.z - minBounds.z) / (2.0 * size.z);
                    point_row2_a.z = (point_row2_a.z - minBounds.z) / (2.0 * size.z);
                    point_row2_b.z = (point_row2_b.z - minBounds.z) / (2.0 * size.z);
                }
            }



            if (SURFACE_TYPE == SURFACE_TYPE_ROTATION || SURFACE_TYPE == SURFACE_TYPE_LAMP)
            {
                is_tv = 0;
                point_row1_a = rotateY(point_row1_a, max_rotate * instance[0] / (no_of_instances - 1));
                point_row1_b = rotateY(point_row1_b, max_rotate * instance[0] / (no_of_instances - 1));
                point_row2_a = rotateY(point_row2_a, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
                point_row2_b = rotateY(point_row2_b, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
            }
            else if (SURFACE_TYPE == SURFACE_TYPE_TRANSLATION)
            {
                is_tv = 0;
        
                point_row1_a = translateX(point_row1_a, float(instance[0]) / float((no_of_instances - 1))); 
                point_row1_b = translateX(point_row1_b, float(instance[0]) / float((no_of_instances - 1))); 
                point_row2_a = translateX(point_row2_a, float((instance[0] + 1)) / float((no_of_instances - 1))); 
                point_row2_b = translateX(point_row2_b, float((instance[0] + 1)) / float((no_of_instances - 1))); 
            }
            else if(SURFACE_TYPE == SURFACE_TYPE_PLANE) { 
                is_tv = 0;
                point_row1_a = translateX(point_row1_a, float(instance[0]) / float((no_of_instances - 1))); 
                point_row1_b = translateX(point_row1_b, float(instance[0]) / float((no_of_instances - 1))); 
                point_row2_a = translateX(point_row2_a, float((instance[0] + 1)) / float((no_of_instances - 1))); 
                point_row2_b = translateX(point_row2_b, float((instance[0] + 1)) / float((no_of_instances - 1))); 
            }
            else if(SURFACE_TYPE == SURFACE_TYPE_VASE) { 
                is_tv = 1;
                point_row1_a = rotateY(point_row1_a, max_rotate * instance[0] / (no_of_instances - 1));
                point_row1_b = rotateY(point_row1_b, max_rotate * instance[0] / (no_of_instances - 1));
                point_row2_a = rotateY(point_row2_a, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
                point_row2_b = rotateY(point_row2_b, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
            }
             else if(SURFACE_TYPE == SURFACE_TYPE_SCREEN)
            {
                point_row1_a = rotateY(point_row1_a, max_rotate * instance[0] / (no_of_instances - 1));
                point_row1_b = rotateY(point_row1_b, max_rotate * instance[0] / (no_of_instances - 1));
                point_row2_a = rotateY(point_row2_a, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
                point_row2_b = rotateY(point_row2_b, max_rotate * (instance[0] + 1) / (no_of_instances - 1));   
            }

            //calcul normale
            vec3 dT_original = point_row1_b - point_row1_a;  // tangent along curve (before rotation)
            vec3 normal_row1_a, normal_row1_b, normal_row2_a, normal_row2_b;

            if (SURFACE_TYPE == SURFACE_TYPE_ROTATION || SURFACE_TYPE == SURFACE_TYPE_LAMP) {
                // Compute normals before rotation
                vec3 radial1a = normalize(vec3(point_row1_a.x, 0.0, point_row1_a.z));
                vec3 radial1b = normalize(vec3(point_row1_b.x, 0.0, point_row1_b.z));
                vec3 radial2a = normalize(vec3(point_row2_a.x, 0.0, point_row2_a.z));
                vec3 radial2b = normalize(vec3(point_row2_b.x, 0.0, point_row2_b.z));
    
                // Blend with vertical component based on curve slope
                vec3 tangent = normalize(point_row1_b - point_row1_a);
                float slopeFactor = tangent.y;
    
                normal_row1_a = normalize(radial1a + vec3(0, -slopeFactor * 0.5, 0));
                normal_row1_b = normalize(radial1b + vec3(0, -slopeFactor * 0.5, 0));
                normal_row2_a = normalize(radial2a + vec3(0, -slopeFactor * 0.5, 0));
                normal_row2_b = normalize(radial2b + vec3(0, -slopeFactor * 0.5, 0));
            }




            float angleDeg = degrees(max_rotate) * float(instance_id) / float(no_of_instances - 1);
            baseColor = getColorForAngle(angleDeg);
            brightness1 = mix(0.0, 1.0, t1);
            brightness2 = mix(0.0, 1.0, t2);



           if (SURFACE_TYPE == SURFACE_TYPE_TRANSLATION || SURFACE_TYPE == SURFACE_TYPE_PLANE) {
                vec3 dT = point_row1_b - point_row1_a;
                vec3 dU = point_row2_a - point_row1_a;  
                vec3 normal = normalize(cross(dT, dU));
                vec3 viewPos = (View * Model * vec4(point_row1_a, 1.0)).xyz;
                vec3 viewDir = normalize(-viewPos);
                vec3 viewNormal = normalize(mat3(View) * normal);
                if(dot(viewNormal, viewDir) < 0.0)
                    normal = -normal;
                normal_row1_a = normal;
                normal_row1_b = normal;
                normal_row2_a = normal;
                normal_row2_b = normal;
            }


            if (render_to_cubemap == 0)
            {
                // Normal rendering - single view
                EmitVertexForLayer(point_row1_a, baseColor * brightness1, vec2(u1, v1) * uvScale, normal_row1_a, 0);
                EmitVertexForLayer(point_row2_a, baseColor * brightness1, vec2(u1, v2) * uvScale, normal_row2_a, 0);
                EmitVertexForLayer(point_row1_b, baseColor * brightness2, vec2(u2, v1) * uvScale, normal_row1_b, 0);
                EmitVertexForLayer(point_row2_b, baseColor * brightness2, vec2(u2, v2) * uvScale, normal_row2_b, 0);
                EndPrimitive();
            }
            else
            {
                // Cubemap rendering - 6 views
                for (int face = 0; face < 6; face++)
                {
                    EmitVertexForLayer(point_row1_a, baseColor * brightness1, vec2(u1, v1) * uvScale, normal_row1_a, face);
                    EmitVertexForLayer(point_row2_a, baseColor * brightness1, vec2(u1, v2) * uvScale, normal_row2_a, face);
                    EmitVertexForLayer(point_row1_b, baseColor * brightness2, vec2(u2, v1) * uvScale, normal_row1_b, face);
                    EmitVertexForLayer(point_row2_b, baseColor * brightness2, vec2(u2, v2) * uvScale, normal_row2_b, face);
                    EndPrimitive();
                }
            }
                
            // EndPrimitive();
        }
    }
}