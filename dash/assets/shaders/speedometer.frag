#version 300 es
precision highp float;

out vec4 FragColor;

in vec4 v_color;
in vec3 v_normal;
in vec3 v_position;
in vec2 v_uv;
in vec3 v_cameraPosition;
in vec3 v_cameraDirection;

uniform sampler2D u_albedo;
uniform float u_angle;
uniform float u_trailRads;

uniform sampler2D u_clipMask;

vec4 computeRadialColor() {
    vec2 angleVec = vec2(
        cos(u_angle),
        sin(u_angle)
    );
    vec2 ourVec = normalize(v_uv - vec2(0.5f, 0.5f));

    vec4 targetColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
    vec4 clear = vec4(targetColor.rgb, 0.0f);
    float t = dot(angleVec, ourVec);

    return mix(targetColor, clear, t);
}

void main() {
    // Anti-alias width in screen space.
    float aa = max(fwidth(dist), 0.00001f);

    vec4 texColor = texture(u_albedo, v_uv);
    vec4 bgColor = computeRadialColor();

    vec4 color = mix(texColor, bgColor, texColor.a);
    color.a *= texture(u_clipMask, v_uv).r;

    FragColor = color;
}
