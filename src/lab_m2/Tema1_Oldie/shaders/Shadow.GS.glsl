#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 100) out;

uniform mat4 LightSpaceMatrix;
uniform mat4 Model;
uniform vec3 control_p0, control_p1, control_p2, control_p3;
uniform int instante;
uniform int puncte_generate;
uniform float max_translate;
uniform float max_rotate;
uniform int SURFACE_TYPE;
uniform vec3 control_points[20];
uniform int num_control_points;

flat in int instance[2];

vec3 rotateY(vec3 point, float u)
{
    float x = point.x * cos(u) - point.z * sin(u);
    float z = point.x * sin(u) + point.z * cos(u);
    return vec3(x, point.y, z);
}

vec3 translateX(vec3 point, float t)
{
    return vec3(point.x + t, point.y, point.z);
}
vec3 bezier(float t)
{
    return mix(control_p0, control_p3, t);
}

vec3 bezier2(float t)
{
    return control_p0 * pow(1.0 - t, 3) +
           control_p1 * 3.0 * t * pow(1.0 - t, 2) +
           control_p2 * 3.0 * pow(t, 2) * (1.0 - t) +
           control_p3 * pow(t, 3);
}
vec3 bezier_general(float t) {
    int n = num_control_points - 1;
    if (n <= 0) return control_points[0];
    if (n >= 20) n = 19; 

    vec3 p = vec3(0.0);
    float coeff = 1.0; 
    float one_minus_t = 1.0 - t;
    
    for(int i = 0; i <= n; i++) {
        float basis = coeff * pow(one_minus_t, float(n - i)) * pow(t, float(i));
        p += basis * control_points[i];
        if (i < n) {
            coeff = coeff * float(n - i) / float(i + 1);
        }
    }
    return p;
}
vec3 getPointOnCurve(float t) {
    if (num_control_points > 0) {
        return bezier_general(t);
    } else {
        return bezier2(t);
    }
}


void main()
{
    const int SURFACE_TYPE_ROTATION = 0;
    const int SURFACE_TYPE_TRANSLATION = 1;
    const int SURFACE_TYPE_PLANE = 2;
    const int SURFACE_TYPE_LAMP = 5;
    const int SURFACE_TYPE_BOTTLE = 7;



    if (instance[0] >= instante - 1)
        return;

    // Handle PLANE type separately
    if (SURFACE_TYPE == SURFACE_TYPE_PLANE) {
        float u1 = float(instance[0]) / float(instante - 1);
        float u2 = float(instance[0] + 1) / float(instante - 1);
        
        for (int i = 0; i < puncte_generate - 1; i++)
        {
            float t1 = float(i) / float(puncte_generate - 1);
            float t2 = float(i + 1) / float(puncte_generate - 1);
            
            // Create horizontal plane (XZ plane at Y=0)
            // This generates a unit quad from (0,0,0) to (1,0,1)
            vec3 point_row1_a = vec3(t1, 0.0, u1);
            vec3 point_row1_b = vec3(t2, 0.0, u1);
            vec3 point_row2_a = vec3(t1, 0.0, u2);
            vec3 point_row2_b = vec3(t2, 0.0, u2);
            
            // Emit vertices transformed to light space
            gl_Position = LightSpaceMatrix * Model * vec4(point_row1_a, 1.0);
            EmitVertex();
            gl_Position = LightSpaceMatrix * Model * vec4(point_row2_a, 1.0);
            EmitVertex();
            gl_Position = LightSpaceMatrix * Model * vec4(point_row1_b, 1.0);
            EmitVertex();
            gl_Position = LightSpaceMatrix * Model * vec4(point_row2_b, 1.0);
            EmitVertex();
            
            EndPrimitive();
        }
        return;
    }

    // Handle ROTATION, TRANSLATION, and LAMP types
    vec3 curveStart = getPointOnCurve(0.0);
    vec3 curveEnd = getPointOnCurve(1.0);
    
    for (int i = 0; i < puncte_generate - 1; i++)
    {
        float t1 = float(i) / float(puncte_generate - 1);
        float t2 = float(i + 1) / float(puncte_generate - 1);

        vec3 point_row1_a = getPointOnCurve(t1);
        vec3 point_row1_b = getPointOnCurve(t2);
        vec3 point_row2_a = getPointOnCurve(t1);
        vec3 point_row2_b = getPointOnCurve(t2);

        // Normalize points based on surface type
        if (SURFACE_TYPE == SURFACE_TYPE_TRANSLATION) {
            float curveLength = length(curveEnd - curveStart);
            
            point_row1_a -= curveStart;
            point_row1_b -= curveStart;
            point_row2_a -= curveStart;
            point_row2_b -= curveStart;
            
            if (curveLength > 0.0001) {
                point_row1_a /= curveLength;
                point_row1_b /= curveLength;
                point_row2_a /= curveLength;
                point_row2_b /= curveLength;
            }
            
            // Apply translation
            point_row1_a = translateX(point_row1_a, float(instance[0]) / float(instante - 1));
            point_row1_b = translateX(point_row1_b, float(instance[0]) / float(instante - 1));
            point_row2_a = translateX(point_row2_a, float(instance[0] + 1) / float(instante - 1));
            point_row2_b = translateX(point_row2_b, float(instance[0] + 1) / float(instante - 1));
        } 
        else if (SURFACE_TYPE == SURFACE_TYPE_ROTATION || SURFACE_TYPE == SURFACE_TYPE_LAMP || SURFACE_TYPE == SURFACE_TYPE_BOTTLE) {
            float heightDiff = curveEnd.y - curveStart.y;
            
            if (heightDiff > 0.0001) {
                point_row1_a.y = (point_row1_a.y - curveStart.y) / heightDiff;
                point_row1_b.y = (point_row1_b.y - curveStart.y) / heightDiff;
                point_row2_a.y = (point_row2_a.y - curveStart.y) / heightDiff;
                point_row2_b.y = (point_row2_b.y - curveStart.y) / heightDiff;
            }
            
            // Apply rotation
            float angle1 = max_rotate * float(instance[0]) / float(instante - 1);
            float angle2 = max_rotate * float(instance[0] + 1) / float(instante - 1);
            
            point_row1_a = rotateY(point_row1_a, angle1);
            point_row1_b = rotateY(point_row1_b, angle1);
            point_row2_a = rotateY(point_row2_a, angle2);
            point_row2_b = rotateY(point_row2_b, angle2);
        }

        // Emit vertices transformed to light space
        gl_Position = LightSpaceMatrix * Model * vec4(point_row2_a, 1.0);
        EmitVertex();
        gl_Position = LightSpaceMatrix * Model * vec4(point_row1_a, 1.0);
        EmitVertex();
        gl_Position = LightSpaceMatrix * Model * vec4(point_row2_b, 1.0);
        EmitVertex();
        gl_Position = LightSpaceMatrix * Model * vec4(point_row1_b, 1.0);
        EmitVertex();
        
        EndPrimitive();
    }
}