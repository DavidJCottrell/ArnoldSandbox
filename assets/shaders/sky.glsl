#type vertex
#version 330 core

layout(location = 0) in vec2 a_NDCPos;

out vec3 v_Direction;

uniform mat4 u_InvViewProjection;  // inverse of (proj * rotation-only-view)

void main()
{
    gl_Position = vec4(a_NDCPos, 0.9999, 1.0);  // full-screen quad at far plane, w always 1

    // Unproject NDC position back to world direction
    vec4 worldPos = u_InvViewProjection * vec4(a_NDCPos, 1.0, 1.0);
    v_Direction   = normalize(worldPos.xyz / worldPos.w);
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
