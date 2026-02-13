#version 330

// Input and output topologies
layout(triangles) in;
layout(line_strip, max_vertices = 170) out;

// Uniform properties
uniform mat4 View;
uniform mat4 Projection;
uniform float normalLength; 

out vec3 color;

void main()
{
    vec3 p1 = gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[1].gl_Position.xyz;
    vec3 p3 = gl_in[2].gl_Position.xyz;
    
    vec3 center = (p1 + p2 + p3) / 3.0;
    
    vec3 edge1 = p2 - p1;
    vec3 edge2 = p3 - p1;
    vec3 normal = normalize(cross(edge1, edge2));
    
    color = vec3(0.0, 0.0, 1.0); 
    
    gl_Position = Projection * View * vec4(center, 1.0);
    EmitVertex();
    
    gl_Position = Projection * View * vec4(center + normal * normalLength, 1.0);
    EmitVertex();
    
    EndPrimitive();
}