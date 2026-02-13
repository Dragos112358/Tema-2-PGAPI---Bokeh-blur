#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 170) out;

uniform mat4 LightSpaceMatrix;
uniform mat4 Model;
uniform vec3 control_p0, control_p1, control_p2, control_p3;
uniform int no_of_instances;
uniform int no_of_generated_points;
uniform float max_translate;
uniform float max_rotate;
uniform int SURFACE_TYPE;

in int instance[2];

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

void main()
{
    const int SURFACE_TYPE_ROTATION = 0;
    const int SURFACE_TYPE_TRANSLATION = 1;
    const int SURFACE_TYPE_PLANE = 2;
    const int SURFACE_TYPE_LAMP = 5;

    vec3 curveStart, curveEnd;
    
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

            vec3 point_row1_a, point_row1_b, point_row2_a, point_row2_b;

            if (SURFACE_TYPE == SURFACE_TYPE_PLANE) {
                point_row1_a = bezier(t1);
                point_row1_b = bezier(t2);
                point_row2_a = bezier(t1);
                point_row2_b = bezier(t2);
            } else {
                point_row1_a = bezier2(t1);
                point_row1_b = bezier2(t2);
                point_row2_a = bezier2(t1);
                point_row2_b = bezier2(t2);
            }

            // Normalize points
            if (SURFACE_TYPE == SURFACE_TYPE_TRANSLATION || SURFACE_TYPE == SURFACE_TYPE_PLANE) {
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
            } else if (SURFACE_TYPE == SURFACE_TYPE_ROTATION) {
                float heightDiff = curveEnd.y - curveStart.y;
                if (heightDiff > 0.0001) {
                    point_row1_a.y = (point_row1_a.y - curveStart.y) / heightDiff;
                    point_row1_b.y = (point_row1_b.y - curveStart.y) / heightDiff;
                    point_row2_a.y = (point_row2_a.y - curveStart.y) / heightDiff;
                    point_row2_b.y = (point_row2_b.y - curveStart.y) / heightDiff;
                }
            }

            // Apply surface transformation
            if (SURFACE_TYPE == SURFACE_TYPE_ROTATION || SURFACE_TYPE == SURFACE_TYPE_LAMP) {
                point_row1_a = rotateY(point_row1_a, max_rotate * instance[0] / (no_of_instances - 1));
                point_row1_b = rotateY(point_row1_b, max_rotate * instance[0] / (no_of_instances - 1));
                point_row2_a = rotateY(point_row2_a, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
                point_row2_b = rotateY(point_row2_b, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
            } else {
                point_row1_a = translateX(point_row1_a, float(instance[0]) / float(no_of_instances - 1));
                point_row1_b = translateX(point_row1_b, float(instance[0]) / float(no_of_instances - 1));
                point_row2_a = translateX(point_row2_a, float(instance[0] + 1) / float(no_of_instances - 1));
                point_row2_b = translateX(point_row2_b, float(instance[0] + 1) / float(no_of_instances - 1));
            }

            // Emit vertices - transform to light space
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
    }
}