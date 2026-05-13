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
    float t = u_timeMs * 0.001f;

    vec2 scrollDir = normalize(vec2(-1.0, -1.0));
    float scrollSpeed = 0.1f; // UV units per second

    vec2 sampleUV = v_uv + scrollDir * t * scrollSpeed;
    vec4 texColor = texture(u_albedo, sampleUV);
    vec3 baseColor = v_color * texColor.rgb;

    FragColor = vec4(baseColor, texColor.a);
    // FragColor = vec4(sampleUV.x, sampleUV.y, 0.0f, 1.0f);
}
