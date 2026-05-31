#version 300 es
precision highp float;

out vec4 FragColor;

in vec4 v_color;
in vec2 v_uv;

uniform sampler2D u_albedo;

uniform float u_percent;
uniform vec4 u_barColor;
uniform vec4 u_bgColor;

// Alpha-over compositing assuming dst is already the element below
vec4 alphaOver(vec4 dst, vec4 src) {
    return vec4(
        mix(dst.rgb, src.rgb, src.a),
        dst.a + src.a * (1.0 - dst.a)
    );
}

void main() {
    vec4 texColor = texture(u_albedo, v_uv) * v_color;
    float whiteMask = texColor.r * texColor.g * texColor.b;

    float percent = clamp(u_percent, 0.0, 1.0);
    float filled = step(v_uv.x, percent);

    vec4 barRegionColor = mix(u_bgColor, u_barColor, filled);

    vec4 maskedBarColor = vec4(
            barRegionColor.rgb * texColor.rgb,
            texColor.a
        );

    FragColor = maskedBarColor;
}
