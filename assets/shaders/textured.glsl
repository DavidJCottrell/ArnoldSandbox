#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec3 v_Normal;

void main()
{
    v_TexCoord  = a_TexCoord;
    // mat3(u_Transform) handles rotations and uniform scaling; sufficient for block geometry
    v_Normal    = mat3(u_Transform) * a_Normal;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Normal;

uniform sampler2D u_Texture;
uniform vec3      u_LightDir;  // world-space direction toward the light, normalised

void main()
{
    float ambient = 0.15;
    float diffuse = max(dot(normalize(v_Normal), u_LightDir), 0.0);
    float light   = ambient + (1.0 - ambient) * diffuse;

    color = texture(u_Texture, v_TexCoord) * vec4(vec3(light), 1.0);
}
