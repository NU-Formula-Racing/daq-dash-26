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

float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

void main()
{
    float t = u_timeMs * 0.001f;

    vec2 scrollDir = normalize(vec2(-1.0, -1.0));
    float scrollSpeed = 0.05f; // UV units per second

    vec2 sampleUV = v_uv + scrollDir * t * scrollSpeed;
    vec4 texColor = texture(u_albedo, sampleUV);
    vec3 baseColor = v_color * texColor.rgb;

    vec4 bgA = vec4(0.2941f, 0.2706f, 0.2667f, 1.0f) * 0.9f;
    vec4 bgB = vec4(0.1725f, 0.1569f, 0.1529f, 1.0f) * 0.95f;

    vec2 dir = normalize(vec2(1.0, -1.0));
    float s = dot(v_uv - vec2(0.0, 1.0), dir)
            / dot(vec2(1.0, -1.0), dir);

    vec4 bgColor = mix(bgA, bgB, s);

    // screen-space dithering
    float noise = hash(gl_FragCoord.xy) - 0.5;
    bgColor.rgb += noise / 255.0;

    FragColor = mix(bgColor, texColor, texColor.a);
    // FragColor = vec4(sampleUV.x, sampleUV.y, 0.0f, 1.0f);
}
