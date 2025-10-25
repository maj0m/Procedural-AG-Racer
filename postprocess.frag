#version 450 core
precision highp float;

uniform sampler2D u_sceneColor;
uniform sampler2D u_sceneDepth;
uniform float u_near;
uniform float u_far;
uniform float u_focusDist;
uniform float u_focusRange;
uniform float u_bloomThreshold;
uniform float u_bloomSoftKnee;
uniform float u_bloomIntensity;
uniform float u_saturation;
uniform float u_vibrance;

in vec2 uv;

out vec4 fragmentColor;

const int   DOF_KERNEL_SIZE = 1;
const float MAX_COC = 2.0;

// ---------- Utility ----------
float linearizeDepth(float depthNonLinear) {
    float ndcDepth = depthNonLinear * 2.0 - 1.0;
    return (2.0 * u_near * u_far) /
           (u_far + u_near - ndcDepth * (u_far - u_near));
}

float computeLuminance(vec3 color) {
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

vec3 applySaturation(vec3 color, float saturationAmount) {
    float luminance = computeLuminance(color);
    return mix(vec3(luminance), color, saturationAmount);
}

vec3 applyVibrance(vec3 color, float vibranceAmount) {
    float minComponent = min(min(color.r, color.g), color.b);
    float maxComponent = max(max(color.r, color.g), color.b);
    float currentSaturation = maxComponent - minComponent;
    float mutedWeight = 1.0 - clamp(currentSaturation, 0.0, 1.0);
    float adjustedSaturation = 1.0 + vibranceAmount * mutedWeight;
    return applySaturation(color, adjustedSaturation);
}

float computeBloomMask(float brightness, float threshold, float knee) {
    float kneeValue = threshold * knee;
    float diff = max(brightness - threshold + kneeValue, 0.0);
    return clamp((diff * diff) / (kneeValue * kneeValue), 0.0, 1.0);
}

// ---------- Bloom ----------
vec3 sampleBloom(vec2 uvCoords) {
    // Base scene color and bright-pass mask
    vec3 baseColor = textureLod(u_sceneColor, uvCoords, 0.0).rgb;
    float bloomMask = computeBloomMask(computeLuminance(baseColor), u_bloomThreshold, u_bloomSoftKnee);

    vec3 accumulatedBloom = vec3(0.0);
    float totalWeight = 0.0;

    // Sample lower mips for blur contribution
    for (int mipLevel = 1; mipLevel <= 10; ++mipLevel) {
        float mipWeight = pow(1.8, float(mipLevel - 1));
        vec3 mipColor = textureLod(u_sceneColor, uvCoords, float(mipLevel)).rgb;
        accumulatedBloom += mipColor * mipWeight;
        totalWeight += mipWeight;
    }

    vec3 averagedBloom = (totalWeight > 0.0)
        ? (accumulatedBloom / totalWeight)
        : vec3(0.0);

    return averagedBloom * bloomMask;
}

// ---------- Main ----------
void main() {
    // Depth of Field
    vec2 texelSize = 1.0 / vec2(textureSize(u_sceneColor, 0));

    float depthSample = texture(u_sceneDepth, uv).r;
    float linearDepth = linearizeDepth(depthSample);

    float circleOfConfusion = (abs(linearDepth - u_focusDist) - u_focusRange) / u_focusRange;
    circleOfConfusion = clamp(circleOfConfusion, 0.0, 1.0);
    float blurRadius = circleOfConfusion * MAX_COC;

    vec4 dofColorAccumulator = vec4(0.0);
    for (int x = -DOF_KERNEL_SIZE; x <= DOF_KERNEL_SIZE; ++x) {
        for (int y = -DOF_KERNEL_SIZE; y <= DOF_KERNEL_SIZE; ++y) {
            vec2 offset = vec2(x, y) * texelSize * blurRadius;
            dofColorAccumulator += texture(u_sceneColor, uv + offset);
        }
    }

    float kernelArea = float((DOF_KERNEL_SIZE * 2 + 1) * (DOF_KERNEL_SIZE * 2 + 1));
    dofColorAccumulator /= kernelArea;

    // Bloom
    vec3 bloomColor = sampleBloom(uv) * u_bloomIntensity;

    // Combine
    vec3 combinedColor = dofColorAccumulator.rgb + bloomColor;

    // Color Grading
    combinedColor = applyVibrance(combinedColor, u_vibrance);
    combinedColor = applySaturation(combinedColor, u_saturation);

    fragmentColor = vec4(combinedColor, 1.0);
}
