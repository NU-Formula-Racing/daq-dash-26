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

uniform float u_timeMs;
uniform float u_speedLineRadius;
uniform float u_speedLineScale;
uniform float u_speedLineSpeed;
uniform float u_speedLineEdge;
uniform float u_speedLineThickness;
uniform float u_speedLineInnerRadius;

uniform vec4 u_vignetteInnerColor;
uniform vec4 u_vignetteOuterColor;
uniform float u_bgAlpha;
uniform float u_lineAlpha;
uniform float u_vignettePower;

vec3 random3(vec3 c) {
    float j = 4096.0 * sin(dot(c, vec3(17.0, 59.4, 15.0)));

    vec3 r;
    r.z = fract(512.0 * j);
    j *= 0.125;
    r.x = fract(512.0 * j);
    j *= 0.125;
    r.y = fract(512.0 * j);

    return r - 0.5;
}

float simplex3d(vec3 p) {
    vec3 s = floor(p + dot(p, vec3(0.3333333)));
    vec3 x = p - s + dot(s, vec3(0.1666667));

    vec3 e = step(vec3(0.0), x - x.yzx);
    vec3 i1 = e * (1.0 - e.zxy);
    vec3 i2 = 1.0 - e.zxy * (1.0 - e);

    vec3 x1 = x - i1 + 0.1666667;
    vec3 x2 = x - i2 + 2.0 * 0.1666667;
    vec3 x3 = x - 1.0 + 3.0 * 0.1666667;

    vec4 w;
    vec4 d;

    w.x = dot(x, x);
    w.y = dot(x1, x1);
    w.z = dot(x2, x2);
    w.w = dot(x3, x3);

    w = max(0.6 - w, 0.0);

    d.x = dot(random3(s), x);
    d.y = dot(random3(s + i1), x1);
    d.z = dot(random3(s + i2), x2);
    d.w = dot(random3(s + 1.0), x3);

    w *= w;
    w *= w;
    d *= w;

    return dot(d, vec4(52.0));
}

float speedLineMask(vec2 uv) {
    vec2 centered = uv - vec2(0.675, 0.5);

    float len = length(centered);
    if (len <= 0.00001) {
        return 0.0;
    }

    float innerRadius = max(u_speedLineInnerRadius, 0.0);
    float innerFade = smoothstep(innerRadius, innerRadius + 0.08, len);

    float time = (u_timeMs * 0.001) * u_speedLineSpeed;
    float radius = max(u_speedLineRadius, 0.0001);
    float scale = max(u_speedLineScale, 0.0001);
    float edge = u_speedLineEdge;
    float thickness = max(u_speedLineThickness, 0.0001);

    vec2 dir = centered / len;
    vec2 p = vec2(0.5) + dir * min(len, radius);

    vec3 p3 = scale * 0.25 * vec3(p.xy, 0.0) + vec3(0.0, 0.0, time * 0.025);

    float noise = simplex3d(p3 * 32.0) * 0.5 + 0.5;
    float radialFadeRadius = 8.0;
    float dist = abs(clamp(len / radialFadeRadius, 0.0, 1.0) * noise * 2.0 - 1.0);

    float stepped = smoothstep(
            edge - 0.5,
            edge + 0.5,
            noise * (1.0 - pow(dist, 4.0))
        );

    float finalMask = smoothstep(
            edge - 0.05,
            edge + 0.05,
            noise * stepped * thickness
        );

    return finalMask * innerFade;
}

float vignetteAmount(vec2 uv) {
    vec2 centered = uv - vec2(0.675, 0.5);
    float r = clamp(length(centered) / 0.70710678, 0.0, 1.0);
    return pow(r, max(u_vignettePower, 0.0001));
}

vec4 vignetteColor(vec2 uv) {
    float t = vignetteAmount(uv);
    return mix(u_vignetteInnerColor, u_vignetteOuterColor, t);
}

void main() {
    vec4 texColor = texture(u_albedo, v_uv);

    // hack to make vignette go to circle
    vec2 uv = v_uv * vec2(800.0f / 480.0f, 1);
    float lines = speedLineMask(uv);
    float vignette = vignetteAmount(uv);
    vec4 vig = vignetteColor(uv);

    float bgAlpha = u_bgAlpha * vignette;
    float lineAlpha = u_lineAlpha * vignette;
    float alpha = mix(bgAlpha, lineAlpha, lines);

    vec4 color;
    color.rgb = vig.rgb * v_color.rgb * texColor.rgb;
    color.a = alpha * v_color.a * texColor.a * vig.a;

    if (color.a <= 0.001) {
        discard;
    }

    FragColor = color;
}
