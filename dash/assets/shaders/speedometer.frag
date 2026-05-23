#version 300 es
precision highp float;

out vec4 FragColor;

in vec4 v_color;
in vec2 v_uv;

uniform sampler2D u_albedo;
uniform float u_angle;
uniform float u_trailRads;
uniform vec4 u_trailColor;
uniform vec4 u_needleColor;
uniform vec4 u_bgColor;

const float TAU = 6.28318530718;

float wrapAngle(float a) {
    return mod(a + TAU, TAU);
}

// Distance from fragAngle to endAngle going backwards around the dial.
// 0 means fragment is exactly at the current needle angle.
// u_trailRads means fragment is at the end of the trail.
float backwardAngularDistance(float fragAngle, float endAngle) {
    return mod(endAngle - fragAngle + TAU, TAU);
}

vec4 computeTrailColor() {
    vec2 p = v_uv - vec2(0.5);
    float len = length(p);

    // Outside dial radius.
    if (len > 0.5) {
        return vec4(0.0, 0.0, 0.0, 0.0);
    }

    float fragAngle = wrapAngle(atan(p.y, p.x));
    float endAngle = wrapAngle(u_angle);
    float dist = backwardAngularDistance(fragAngle, endAngle);

    // Outside angular trail region: just background.
    if (dist > u_trailRads) {
        return u_bgColor;
    }

    // Trail is strongest near needle, fades backward.
    float angularFade = 1.0 - smoothstep(0.0, u_trailRads, dist);

    vec4 trailColor = vec4(
            u_trailColor.rgb,
            u_trailColor.a * angularFade
        );

    // Inner circle stays bgColor, then smoothly blends into the trail.
    const float innerRadius = 0.25;
    const float blendWidth = 0.08;

    float radialFade = smoothstep(
            innerRadius,
            innerRadius + blendWidth,
            len
        );

    return mix(u_bgColor, trailColor, radialFade);
}

void main() {
    vec4 texColor = texture(u_albedo, v_uv) * v_color;
    vec4 trailColor = computeTrailColor();

    FragColor = mix(trailColor, texColor, texColor.a);
}
