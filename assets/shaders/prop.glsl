#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec3 v_WorldPos;

void main()
{
    vec4 worldPos = u_Transform * vec4(a_Position, 1.0);
    v_WorldPos  = worldPos.xyz;
    v_TexCoord  = a_TexCoord;
    gl_Position = u_ViewProjection * worldPos;
}

#type fragment
#version 330 core

in  vec2 v_TexCoord;
in  vec3 v_WorldPos;
out vec4 fragColor;

uniform sampler2D u_Texture;
uniform vec3      u_LightDir;

void main()
{
    // Reconstruct face normal from screen-space derivatives — no normal attribute needed
    vec3 dx     = dFdx(v_WorldPos);
    vec3 dy     = dFdy(v_WorldPos);
    vec3 normal = normalize(cross(dx, dy));

    float diffuse = max(dot(normal, normalize(u_LightDir)), 0.0);
    float light   = 0.4 + 0.6 * diffuse;

    vec4 tex  = texture(u_Texture, v_TexCoord * 2.0);
    fragColor = vec4(tex.rgb * light, 1.0);
}
