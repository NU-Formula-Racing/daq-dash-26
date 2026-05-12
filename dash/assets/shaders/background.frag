#version 300 es
precision highp float;

out vec4 FragColor;

in vec3 v_color;
in vec3 v_normal;
in vec3 v_position;
in vec2 v_uv;
in vec3 v_cameraPosition;
in vec3 v_cameraDirection;

uniform sampler2D u_albedo; // optional, can be ignored if not used
uniform float u_timeMs;

void main()
{
    vec2 offset = vec2(-1.0, -1.0) * u_timeMs;
    vec4 texture = texture(u_albedo, v_uv + offset).rgba;
    vec3 baseColor = v_color * texture.rgb;
    FragColor = vec4(baseColor, texture.a);
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
