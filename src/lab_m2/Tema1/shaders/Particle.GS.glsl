#version 430
// Input and output topologies
layout(points) in;
layout(triangle_strip, max_vertices = 24) out;  // 4 vertices * 6 faces = 24
// Uniform properties
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 viewMatrices[6];
uniform vec3 eye_position;
uniform float offset;
in float vert_lifetime[1];
in float vert_iLifetime[1];
// Output
layout(location = 0) out vec2 texture_coord;
layout(location = 1) out float geom_lifetime;
layout(location = 2) out float geom_iLifetime;
// Local variables
vec3 vpos = gl_in[0].gl_Position.xyz;
//in vec3 worldPos;
//vec3 vpos = worldPos[0];
void EmitPoint(vec2 offset_coord, vec3 right, vec3 up, int layer)
{
    vec3 pos = right * offset_coord.x + up * offset_coord.y + vpos;
    gl_Position = Projection * viewMatrices[layer] * vec4(pos, 1.0);
    EmitVertex();
}

void main()
{
    float ds = offset;
    
    for (int layer = 0; layer < 6; layer++)
    {
        gl_Layer = layer;
        
        vec3 forward;
        vec3 worldUp = vec3(0, 1, 0);
        
        if (layer == 0) {
            // +X face
            forward = vec3(1.0, 0.0, 0.0);
            worldUp = vec3(0.0, -1.0, 0.0);
        }
        else if (layer == 1) {
            // -X face
            forward = vec3(-1.0, 0.0, 0.0);
            worldUp = vec3(0.0, -1.0, 0.0);
        }
        else if (layer == 2) {
            // +Y face
            forward = vec3(0.0, 1.0, 0.0);
            worldUp = vec3(0.0, 0.0, 1.0);
        }
        else if (layer == 3) {
            // -Y face
            forward = vec3(0.0, -1.0, 0.0);
            worldUp = vec3(0.0, 0.0, -1.0);
        }
        else if (layer == 4) {
            // +Z face
            forward = vec3(0.0, 0.0, 1.0);
            worldUp = vec3(0.0, -1.0, 0.0);
        }
        else {
            // -Z face
            forward = vec3(0.0, 0.0, -1.0);
            worldUp = vec3(0.0, -1.0, 0.0);
        }
        
        vec3 right = normalize(cross(forward, worldUp));
        vec3 up = normalize(cross(forward, right));
        
        // Vertex 0: bottom-left
        texture_coord = vec2(0.0, 0.0);
        geom_lifetime = vert_lifetime[0];
        geom_iLifetime = vert_iLifetime[0];
        EmitPoint(vec2(-ds, -ds), right, up, layer);
        
        // Vertex 1: bottom-right
        texture_coord = vec2(1.0, 0.0);
        geom_lifetime = vert_lifetime[0];
        geom_iLifetime = vert_iLifetime[0];
        EmitPoint(vec2(ds, -ds), right, up, layer);
        
        // Vertex 3: top-left
        texture_coord = vec2(0.0, 1.0);
        geom_lifetime = vert_lifetime[0];
        geom_iLifetime = vert_iLifetime[0];
        EmitPoint(vec2(-ds, ds), right, up, layer);
        
        // Vertex 2: top-right
        texture_coord = vec2(1.0, 1.0);
        geom_lifetime = vert_lifetime[0];
        geom_iLifetime = vert_iLifetime[0];
        EmitPoint(vec2(ds, ds), right, up, layer);
        
        EndPrimitive();
    }
}