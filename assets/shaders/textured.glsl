#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_Normal;
out vec4 v_Color;

void main()
{
    v_Normal    = mat3(u_Transform) * a_Normal;
    v_Color     = a_Color;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec4 v_Color;

uniform vec3 u_LightDir;

void main()
{
    float ambient = 0.15;
    float diffuse = max(dot(normalize(v_Normal), u_LightDir), 0.0);
    float light   = ambient + (1.0 - ambient) * diffuse;

    color = v_Color * vec4(vec3(light), 1.0);
}
