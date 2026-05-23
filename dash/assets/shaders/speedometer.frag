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

// Dial shape config
const float OUTER_RADIUS = 0.5;
const float INNER_CIRCLE_RADIUS = 0.1;
const float BLEND_RADIUS = 0.1;

// Needle config
const float NEEDLE_HALF_WIDTH = 0.018;

float wrapAngle(float a) {
    return mod(a + TAU, TAU);
}

// Distance from fragAngle to endAngle going backwards around the dial
// 0 means fragment is exactly at the current needle angle
// u_trailRads means fragment is at the end of the trail
float backwardAngularDistance(float fragAngle, float endAngle) {
    return mod(endAngle - fragAngle + TAU, TAU);
}

// Smallest absolute angular distance between two angles
float angularDistance(float a, float b) {
    float d = abs(wrapAngle(a) - wrapAngle(b));
    return min(d, TAU - d);
}

// Alpha-over compositing assuming dst is already the element below
vec4 alphaOver(vec4 dst, vec4 src) {
    return vec4(
        mix(dst.rgb, src.rgb, src.a),
        dst.a + src.a * (1.0 - dst.a)
    );
}

float computeRadialAlpha(float len) {
    if (len > OUTER_RADIUS) {
        return 0.0;
    }

    return smoothstep(
        INNER_CIRCLE_RADIUS,
        INNER_CIRCLE_RADIUS + BLEND_RADIUS,
        len
    );
}

float computeTrailAlpha(float len, float fragAngle, float endAngle) {
    float radialAlpha = computeRadialAlpha(len);

    if (radialAlpha <= 0.0) {
        return 0.0;
    }

    float dist = backwardAngularDistance(fragAngle, endAngle);

    if (dist > u_trailRads) {
        return 0.0;
    }

    // Fade alpha backward along the trail, then fade radially toward center
    float angularAlpha = 1.0 - smoothstep(0.0, u_trailRads, dist);

    return angularAlpha * radialAlpha;
}

float computeNeedleAlpha(float len, float fragAngle, float endAngle) {
    float radialAlpha = computeRadialAlpha(len);

    if (radialAlpha <= 0.0) {
        return 0.0;
    }

    float dist = angularDistance(fragAngle, endAngle);

    float angularAlpha = 1.0 - smoothstep(
                NEEDLE_HALF_WIDTH * 0.6,
                NEEDLE_HALF_WIDTH,
                dist
            );

    return angularAlpha * radialAlpha;
}

vec4 computeNeedleColor(float len, float fragAngle, float endAngle) {
    float needleAlpha = computeNeedleAlpha(len, fragAngle, endAngle);

    return vec4(u_needleColor.rgb, u_needleColor.a * needleAlpha);
}

void main() {
    vec2 p = v_uv - vec2(0.5);
    float len = length(p);

    float fragAngle = 0.0;
    if (len >= 0.0001) {
        fragAngle = wrapAngle(atan(p.y, p.x));
    }

    float endAngle = wrapAngle(u_angle);

    vec4 texColor = texture(u_albedo, v_uv) * v_color;

    vec4 color = u_bgColor;

    if (len > 0.5f) {
        color = vec4(0.0f);
    }

    float trailAlpha = computeTrailAlpha(len, fragAngle, endAngle);
    vec4 trailColor = vec4(u_trailColor.rgb, u_trailColor.a * trailAlpha);
    color = alphaOver(color, trailColor);

    vec4 needleColor = computeNeedleColor(len, fragAngle, endAngle);
    color = alphaOver(color, needleColor);

    color = alphaOver(color, texColor);

    FragColor = color;
}
