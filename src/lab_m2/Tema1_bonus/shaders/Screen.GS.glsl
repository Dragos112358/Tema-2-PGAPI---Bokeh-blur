#version 330

// Input and output topologies
// TODO(student): First, generate a curve (via line strip),
// then a rotation/translation surface (via triangle strip)
layout(lines) in;
//layout(line_strip) in;

layout(triangle_strip, max_vertices = 800) out;
uniform mat4 Model;
// Uniform properties
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p0, control_p1, control_p2, control_p3;
uniform int no_of_instances;
// TODO(student): Declare any other uniforms here
uniform int no_of_generated_points;
uniform float max_translate;
uniform float max_rotate;
uniform int SURFACE_TYPE;
out vec3 gColor;
out vec3 world_position;
out vec3 world_normal;
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
    return vec3(point.x, point.y+t, point.z);
}
vec3 translateZ(vec3 point, float t)
{
    return vec3(point.x, point.y, point.z+t);
}

// This function computes B(t) with 4 control points. For a visual example,
// see [1]. For an interactive Javascript example with the exact points in
// this code, see [2].
//
// [1] https://www.desmos.com/calculator/cahqdxeshd
// [2] https://jsfiddle.net/6yuv9htf/
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

vec3 bezier3(float t) 
{ 
    vec3 base = control_p0 * pow(1.0 - t, 3) + 
                control_p1 * 3.0 * t * pow(1.0 - t, 2) + 
                control_p2 * 3.0 * pow(t, 2) * (1.0 - t) + 
                control_p3 * pow(t, 3); 
    return base; 
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

// TODO(student): If you want to take things a step further, try drawing a
// Hermite spline. Hint: you can repurpose two of the control points. For a
// visual example, see [1]. For an interactive Javascript example with the
// exact points in this code, see [2].
// 
// Unlike the Javascript function, you MUST NOT call the Bezier function.
// There is another way to draw a Hermite spline, all you need is to find
// the formula.
//
// [1] https://www.desmos.com/calculator/5knm5tkr8m
// [2] https://jsfiddle.net/6yuv9htf/

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
    const int TV = 3;
    //const int SURFACE_TYPE = SURFACE_TYPE_ROTATION;
    vec3 color;
    int instance_id =instance[0];
    if (instance_id % 3 == 0)
        color = vec3(1.0, 0.0, 0.0);
    else if (instance_id % 3 == 1)
        color = vec3(0.0, 1.0, 0.0);  
    else
        color = vec3(0.0, 0.0, 1.0);
     vec3 baseColor;
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


            vec3 point_row1_a = bezier(t1);
            vec3 point_row1_b = bezier(t2);

            vec3 point_row2_a = bezier(t1);
            vec3 point_row2_b = bezier(t2);

            if (SURFACE_TYPE == SURFACE_TYPE_ROTATION)
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
                point_row1_a = translateX(point_row1_a, max_translate * instance[0] / (no_of_instances - 1));
                point_row1_b = translateX(point_row1_b, max_translate * instance[0] / (no_of_instances - 1));
                point_row2_a = translateX(point_row2_a, max_translate * (instance[0] + 1) / (no_of_instances - 1));
                point_row2_b = translateX(point_row2_b, max_translate * (instance[0] + 1) / (no_of_instances - 1));
            }
            else if(SURFACE_TYPE == SURFACE_TYPE_PLANE) { 
                 is_tv = 0;
                point_row1_a = translateZ(point_row1_a, max_translate * instance[0] / (no_of_instances - 1)); 
                point_row1_b = translateZ(point_row1_b, max_translate * instance[0] / (no_of_instances - 1)); 
                point_row2_a = translateZ(point_row2_a, max_translate * (instance[0] + 1) / (no_of_instances - 1)); 
                point_row2_b = translateZ(point_row2_b, max_translate * (instance[0] + 1) / (no_of_instances - 1)); 
            }
            else if(SURFACE_TYPE == SURFACE_TYPE_VASE) { 
                is_tv = 1;
                point_row1_a = bezier2(t1);
                point_row1_b = bezier2(t2);
                point_row2_a = bezier2(t1);
                point_row2_b = bezier2(t2);
                point_row1_a = rotateY(point_row1_a, max_rotate * instance[0] / (no_of_instances - 1)); 
                point_row1_b = rotateY(point_row1_b, max_rotate * instance[0] / (no_of_instances - 1)); 
                point_row2_a = rotateY(point_row2_a, max_rotate * (instance[0] + 1) / (no_of_instances - 1)); 
                point_row2_b = rotateY(point_row2_b, max_rotate * (instance[0] + 1) / (no_of_instances - 1)); 
            }
            else if(SURFACE_TYPE == SURFACE_TYPE_LAMP)
            {
                point_row1_a = bezier2(t1);
                point_row1_b = bezier2(t2);
                point_row2_a = bezier2(t1);
                point_row2_b = bezier2(t2);
                point_row1_a = rotateY(point_row1_a, max_rotate * instance[0] / (no_of_instances - 1));
                point_row1_b = rotateY(point_row1_b, max_rotate * instance[0] / (no_of_instances - 1));
                point_row2_a = rotateY(point_row2_a, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
                point_row2_b = rotateY(point_row2_b, max_rotate * (instance[0] + 1) / (no_of_instances - 1));   
            }
             else if(SURFACE_TYPE == SURFACE_TYPE_SCREEN)
            {
                point_row1_a = bezier2(t1);
                point_row1_b = bezier2(t2);
                point_row2_a = bezier2(t1);
                point_row2_b = bezier2(t2);
                point_row1_a = rotateY(point_row1_a, max_rotate * instance[0] / (no_of_instances - 1));
                point_row1_b = rotateY(point_row1_b, max_rotate * instance[0] / (no_of_instances - 1));
                point_row2_a = rotateY(point_row2_a, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
                point_row2_b = rotateY(point_row2_b, max_rotate * (instance[0] + 1) / (no_of_instances - 1));   
            }

            //calcul normale
            vec3 dT = point_row1_b - point_row1_a;
            vec3 dU = point_row2_a - point_row1_a;
            vec3 normal = normalize(cross(dU, dT));
            mat3 normalMatrix = mat3(transpose(inverse(Model)));
            world_normal = normalize(normalMatrix * normal);

            float angleDeg = degrees(max_rotate) * float(instance_id) / float(no_of_instances - 1);
            baseColor = getColorForAngle(angleDeg);
            brightness1 = mix(0.0, 1.0, t1);
            brightness2 = mix(0.0, 1.0, t2);

            gColor = baseColor*brightness1;
            gUV = vec2(u1, v1);
            world_normal = normal;
            world_position = vec3(Model * vec4(point_row1_a, 1.0));
            gl_Position = Projection * View *Model* vec4(point_row1_a, 1);
            EmitVertex();

            gColor = baseColor*brightness1;
            gUV = vec2(u1, v2);
            world_normal = normal;
            world_position = vec3(Model * vec4(point_row2_a, 1.0));
            gl_Position = Projection * View *Model* vec4(point_row2_a, 1);
            EmitVertex();

            gColor = baseColor*brightness2;
            gUV = vec2(u2, v1);
            world_normal = normal;
            world_position = vec3(Model * vec4(point_row1_b, 1.0));

            gl_Position = Projection * View *Model* vec4(point_row1_b, 1);
            EmitVertex();

            gColor = baseColor*brightness2;
            gUV = vec2(u2, v2);
            world_normal = normal;
            world_position = vec3(Model * vec4(point_row2_b, 1.0));
            gl_Position = Projection * View *Model* vec4(point_row2_b, 1);
            EmitVertex();
            EndPrimitive();
        }
    }
}