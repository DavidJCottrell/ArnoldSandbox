#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;   // kept in layout; unused in fragment

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_WorldPos;
out vec3 v_Normal;

void main()
{
    vec4 worldPos = u_Transform * vec4(a_Position, 1.0);
    v_WorldPos    = worldPos.xyz;
    v_Normal      = a_Normal;   // mat3(translation) == identity, so pass through
    gl_Position   = u_ViewProjection * worldPos;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_WorldPos;
in vec3 v_Normal;

uniform sampler2D u_Atlas;   // slot 0: [Grass | Dirt | Stone] horizontal strip
uniform vec3      u_LightDir;

const float UV_DETAIL   = 0.18;  // detail tile frequency (~5-6 world units per tile)
const float UV_MACRO    = 0.035; // macro variation frequency (~28 world units per blob)
const float HEIGHT_HIGH = 9.0;   // world Y where terrain is fully grass-eligible

// Sample one atlas tile triplanarly at a given UV scale.
// Atlas layout: horizontal strip, tileIndex 0=Grass 1=Dirt 2=Stone
// Each tile occupies U range [i/3, (i+1)/3]; V spans full height.
vec3 sampleTile(int tileIndex, vec3 wpos, vec3 bw, float scale)
{
    float tw  = 1.0 / 3.0;
    float off = float(tileIndex) * tw;

    vec2 uvX = wpos.yz;
    vec2 uvY = wpos.xz;
    vec2 uvZ = wpos.xy;

    // fract() keeps U within the tile column; V tiles freely via GL_REPEAT
    vec2 axX = vec2(fract(uvX.x * scale) * tw + off, uvX.y * scale);
    vec2 axY = vec2(fract(uvY.x * scale) * tw + off, uvY.y * scale);
    vec2 axZ = vec2(fract(uvZ.x * scale) * tw + off, uvZ.y * scale);

    return texture(u_Atlas, axX).rgb * bw.x
         + texture(u_Atlas, axY).rgb * bw.y
         + texture(u_Atlas, axZ).rgb * bw.z;
}

void main()
{
    vec3 n = normalize(v_Normal);

    // Triplanar blend weights — power of 4 sharpens seams without popping
    vec3 bw = pow(abs(n), vec3(4.0));
    bw /= (bw.x + bw.y + bw.z + 1e-5);

    // Slope / height blend factors (mirrors existing CPU VertexColor logic)
    float slopeBlend  = smoothstep(0.5, 1.0, abs(n.y)); // 0=steep/stone → 1=flat
    float heightBlend = clamp(v_WorldPos.y / HEIGHT_HIGH, 0.0, 1.0);
    float grassWeight = slopeBlend * heightBlend;
    float dirtWeight  = slopeBlend * (1.0 - heightBlend * 0.5);

    // Sample detail + macro for each tile; multiply to break tiling
    vec3 macro;

    macro      = sampleTile(0, v_WorldPos, bw, UV_MACRO) * 0.5 + 0.75;
    vec3 grass = sampleTile(0, v_WorldPos, bw, UV_DETAIL) * macro;

    macro     = sampleTile(1, v_WorldPos, bw, UV_MACRO) * 0.5 + 0.75;
    vec3 dirt = sampleTile(1, v_WorldPos, bw, UV_DETAIL) * macro;

    macro      = sampleTile(2, v_WorldPos, bw, UV_MACRO) * 0.5 + 0.75;
    vec3 stone = sampleTile(2, v_WorldPos, bw, UV_DETAIL) * macro;

    // Blend: stone base → mix in dirt on flatter surfaces → mix in grass on high+flat
    vec3 texColor = mix(stone, dirt,     dirtWeight);
         texColor = mix(texColor, grass, grassWeight);

    // Lighting
    float ambient = 0.15;
    float diffuse = max(dot(n, normalize(u_LightDir)), 0.0);
    float light   = ambient + (1.0 - ambient) * diffuse;

    color = vec4(texColor * light, 1.0);
}
