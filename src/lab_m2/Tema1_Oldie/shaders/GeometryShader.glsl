#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 256) out;

uniform mat4 Model, View, Projection, LightSpaceMatrix;
uniform mat4 viewMatrices[6];
uniform vec3 control_p0, control_p1, control_p2, control_p3;
uniform vec3 control_points[20];
uniform vec2 uvScale;
uniform int num_control_points, instante, puncte_generate;
uniform int render_to_cubemap, cubemap_face, SURFACE_TYPE;
uniform float max_translate, max_rotate;

out vec4 frag_pos_light_space;
out vec3 gColor, frag_pos, gNormal;
out vec2 gUV;
flat out int is_tv;

in int instance[2];

const int TYPE_ROTATION = 0;
const int TYPE_TRANSLATION = 1;
const int TYPE_PLANE = 2;
const int TYPE_VASE = 4;
const int TYPE_LAMP = 5;
const int TYPE_SCREEN = 6;
const int TYPE_BOTTLE = 7;
const float EPSILON = 0.0001;

vec3 bezierCubic(float t) { 
    float mt = 1.0 - t;
    float t2 = t * t, mt2 = mt * mt;
    return control_p0 * (mt2 * mt) + 
           control_p1 * (3.0 * t * mt2) + 
           control_p2 * (3.0 * t2 * mt) + 
           control_p3 * (t2 * t);
}

vec3 bsplineInterpolate(float t) {
    int n = num_control_points;
    if (n < 4) return control_points[0];
    
    float totalT = clamp(t, 0.0, 0.999999) * float(n - 3);
    int i = int(totalT);
    float u = fract(totalT);
    float u2 = u * u, u3 = u2 * u;
    
    vec3 p[4];
    for (int j = 0; j < 4; j++)
        p[j] = control_points[clamp(i + j, 0, n - 1)];
    
    float b[4] = float[](
        (1.0 - 3.0*u + 3.0*u2 - u3) / 6.0,
        (4.0 - 6.0*u2 + 3.0*u3) / 6.0,
        (1.0 + 3.0*u + 3.0*u2 - 3.0*u3) / 6.0,
        u3 / 6.0
    );
    
    return p[0]*b[0] + p[1]*b[1] + p[2]*b[2] + p[3]*b[3];
}

vec3 evaluateCurve(float t) {
    return (num_control_points > 0) ? bsplineInterpolate(t) : bezierCubic(t);
}

vec3 rotY(vec3 p, float a) {
    return vec3(p.x * cos(a) - p.z * sin(a), p.y, p.x * sin(a) + p.z * cos(a));
}

vec3 shiftX(vec3 p, float d) { return p + vec3(d, 0, 0); }

void emitVertex(vec3 pos, vec3 col, vec2 uv, vec3 norm, int layer) {
    vec4 wp = Model * vec4(pos, 1.0);
    gColor = col;
    gUV = uv;
    gNormal = mat3(transpose(inverse(Model))) * norm;
    frag_pos = wp.xyz;
    frag_pos_light_space = LightSpaceMatrix * wp;
    
    gl_Layer = layer;
    gl_Position = (render_to_cubemap == 1) ? 
        Projection * viewMatrices[layer] * wp : 
        Projection * View * wp;
    
    EmitVertex();
}

void generatePlane() {
    float u1 = float(instance[0]) / float(instante - 1);
    float u2 = float(instance[0] + 1) / float(instante - 1);
    int layer = (render_to_cubemap == 1) ? cubemap_face : 0;
    is_tv = 0;
    
    for (int i = 0; i < puncte_generate - 1; i++) {
        float t1 = float(i) / float(puncte_generate - 1);
        float t2 = float(i + 1) / float(puncte_generate - 1);
        
        vec3 p[4] = vec3[](
            vec3(t1, 0, u1), vec3(t2, 0, u1),
            vec3(t1, 0, u2), vec3(t2, 0, u2)
        );
        
        vec3 norm = normalize(cross(p[2] - p[0], p[1] - p[0]));
        vec3 col = vec3(0);
        vec2 uvs[4] = vec2[](
            vec2(t1, u1) * uvScale, vec2(t2, u1) * uvScale,
            vec2(t1, u2) * uvScale, vec2(t2, u2) * uvScale
        );
        
        emitVertex(p[0], col, uvs[0], norm, layer);
        emitVertex(p[2], col, uvs[2], norm, layer);
        emitVertex(p[1], col, uvs[1], norm, layer);
        emitVertex(p[3], col, uvs[3], norm, layer);
        EndPrimitive();
    }
}

void normalizePoints(inout vec3 p[4], vec3 start, vec3 end) {
    if (SURFACE_TYPE == TYPE_TRANSLATION) {
        float len = length(end - start);
        if (len > EPSILON) {
            for (int i = 0; i < 4; i++)
                p[i] = (p[i] - start) / len;
        }
    } 
    else if (SURFACE_TYPE == TYPE_ROTATION) {
        float h = end.y - start.y;
        if (h > EPSILON) {
            for (int i = 0; i < 4; i++)
                p[i].y = (p[i].y - start.y) / h;
        }
    } 
    else if (SURFACE_TYPE == TYPE_LAMP) {
        vec3 minB = min(min(start, end), min(min(control_p0, control_p1), min(control_p2, control_p3)));
        vec3 maxB = max(max(start, end), max(max(control_p0, control_p1), max(control_p2, control_p3)));
        vec3 sz = maxB - minB;
        
        for (int i = 0; i < 4; i++) {
            if (sz.x > EPSILON) p[i].x = (p[i].x - minB.x) / (2.0 * sz.x);
            if (sz.y > EPSILON) p[i].y = (p[i].y - minB.y) / sz.y;
            if (sz.z > EPSILON) p[i].z = (p[i].z - minB.z) / (2.0 * sz.z);
        }
    }
}

void computeRotNormals(vec3 p[4], out vec3 n[4]) {
    vec3 rad[4];
    for (int i = 0; i < 4; i++)
        rad[i] = normalize(vec3(p[i].x, 0, p[i].z));
    
    float slope = normalize(p[1] - p[0]).y;
    for (int i = 0; i < 4; i++)
        n[i] = normalize(rad[i] - vec3(0, slope * 0.5, 0));
}

void transformSurface(inout vec3 p[4], out vec3 n[4]) {
    bool isRot = (SURFACE_TYPE == TYPE_ROTATION || SURFACE_TYPE == TYPE_LAMP || 
                  SURFACE_TYPE == TYPE_BOTTLE || SURFACE_TYPE == TYPE_VASE);
    
    is_tv = (SURFACE_TYPE == TYPE_BOTTLE || SURFACE_TYPE == TYPE_VASE || SURFACE_TYPE == TYPE_SCREEN) ? 1 : 0;
    
    if (isRot) {
        float a[2] = float[](
            max_rotate * instance[0] / (instante - 1),
            max_rotate * (instance[0] + 1) / (instante - 1)
        );
        p[0] = rotY(p[0], a[0]); p[1] = rotY(p[1], a[0]);
        p[2] = rotY(p[2], a[1]); p[3] = rotY(p[3], a[1]);
        computeRotNormals(p, n);
    }
    else if (SURFACE_TYPE == TYPE_TRANSLATION) {
        float off[2] = float[](
            float(instance[0]) / float(instante - 1),
            float(instance[0] + 1) / float(instante - 1)
        );
        p[0] = shiftX(p[0], off[0]); p[1] = shiftX(p[1], off[0]);
        p[2] = shiftX(p[2], off[1]); p[3] = shiftX(p[3], off[1]);
        
        vec3 norm = normalize(cross(p[1] - p[0], p[2] - p[0]));
        for (int i = 0; i < 4; i++) n[i] = norm;
    }
    else if (SURFACE_TYPE == TYPE_SCREEN) {
        float a[2] = float[](
            max_rotate * float(instance[0]) / float(instante - 1),
            max_rotate * float(instance[0] + 1) / float(instante - 1)
        );
        
        vec3 rx[4];
        for (int i = 0; i < 2; i++) {
            rx[i] = vec3(p[i].x, p[i].y * cos(a[0]), p[i].y * sin(a[0]));
            rx[i+2] = vec3(p[i+2].x, p[i+2].y * cos(a[1]), p[i+2].y * sin(a[1]));
        }
        
        for (int i = 0; i < 4; i++)
            p[i] = vec3(rx[i].x, rx[i].y, rx[i].z * 0.2);
        
        vec3 normU = normalize(cross(rx[1] - rx[0], rx[2] - rx[0]));
        vec3 normS = normalize(vec3(normU.xy, normU.z * 5.0));
        for (int i = 0; i < 4; i++) n[i] = normS;
    }
}

void emitQuad(vec3 p[4], vec3 n[4], vec3 col, float t1, float t2, float v1, float v2, int layer) {
    vec2 uv[4];
    
    if (SURFACE_TYPE == TYPE_LAMP) {
        uv[0] = vec2(t1, v2); uv[1] = vec2(t2, v2);
        uv[2] = vec2(t1, v1); uv[3] = vec2(t2, v1);
        emitVertex(p[0], col, uv[0] * uvScale, n[0], layer);
        emitVertex(p[2], col, uv[2] * uvScale, n[2], layer);
        emitVertex(p[1], col, uv[1] * uvScale, n[1], layer);
        emitVertex(p[3], col, uv[3] * uvScale, n[3], layer);
    } else if (SURFACE_TYPE == TYPE_SCREEN) {
        uv[0] = vec2(t1, v1); uv[1] = vec2(t2, v1);
        uv[2] = vec2(t1, v2); uv[3] = vec2(t2, v2);
        emitVertex(p[0], col, uv[0] * uvScale, n[0], layer);
        emitVertex(p[2], col, uv[2] * uvScale, n[2], layer);
        emitVertex(p[1], col, uv[1] * uvScale, n[1], layer);
        emitVertex(p[3], col, uv[3] * uvScale, n[3], layer);
    } else {
        uv[0] = vec2(t1, v1); uv[1] = vec2(t2, v1);
        uv[2] = vec2(t1, v2); uv[3] = vec2(t2, v2);
        emitVertex(p[2], col, uv[2] * uvScale, n[2], layer);
        emitVertex(p[0], col, uv[0] * uvScale, n[0], layer);
        emitVertex(p[3], col, uv[3] * uvScale, n[3], layer);
        emitVertex(p[1], col, uv[1] * uvScale, n[1], layer);
    }
    EndPrimitive();
}

void generateSurface() {
    vec3 start = evaluateCurve(0.0), end = evaluateCurve(1.0);
    int layer = (render_to_cubemap == 1) ? cubemap_face : 0;

    for (int i = 0; i < puncte_generate - 1; i++) {
        float t[2] = float[](float(i) / float(puncte_generate - 1), float(i + 1) / float(puncte_generate - 1));
        float v[2] = float[](float(instance[0]) / float(instante - 1), float(instance[0] + 1) / float(instante - 1));

        vec3 p[4] = vec3[](evaluateCurve(t[0]), evaluateCurve(t[1]), evaluateCurve(t[0]), evaluateCurve(t[1]));
        normalizePoints(p, start, end);
        
        vec3 n[4];
        transformSurface(p, n);
        
        emitQuad(p, n, vec3(0), t[0], t[1], v[0], v[1], layer);
    }
}

void main() {
    if (instance[0] >= instante - 1) return;
    
    (SURFACE_TYPE == TYPE_PLANE) ? generatePlane() : generateSurface();
}