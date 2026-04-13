#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

out vec3 v_Direction;

uniform mat4 u_ViewProjection;

void main()
{
    v_Direction = a_Position;
    vec4 pos    = u_ViewProjection * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;  // force depth to far plane (z/w = 1.0)
}

#type fragment
#version 330 core

in  vec3 v_Direction;
out vec4 FragColor;

uniform vec3  u_ZenithColor;
uniform vec3  u_HorizonColor;
uniform vec3  u_GroundColor;
uniform vec3  u_SunDirection;  // pre-normalised
uniform vec3  u_SunColor;
uniform float u_SunSize;       // larger value = wider sun disc

void main()
{
    vec3  dir = normalize(v_Direction);
    float y   = dir.y;

    // Gradient: zenith -> horizon -> ground
    vec3 skyColor;
    if (y >= 0.0)
        skyColor = mix(u_HorizonColor, u_ZenithColor, pow(y, 0.5));
    else
        skyColor = mix(u_HorizonColor, u_GroundColor, clamp(-y * 4.0, 0.0, 1.0));

    // Sun disc — smoothstep over cosine-of-angle threshold
    float cosAngle = dot(dir, u_SunDirection);
    float sunDisc  = smoothstep(1.0 - u_SunSize, 1.0 - u_SunSize * 0.3, cosAngle);
    skyColor = mix(skyColor, u_SunColor, sunDisc);

    FragColor = vec4(skyColor, 1.0);
}
