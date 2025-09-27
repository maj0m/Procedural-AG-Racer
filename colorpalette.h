#pragma once
#include "framework.h"
#include <imgui.h>

struct ColorPaletteUBO {
    vec4 terrainColors[5];
    vec4 grassColor;
    vec4 waterColor;
    vec4 fogColor;
    vec4 angleThresholds; // degrees
    float fogDensity;
    float _pad[3];
};

class ColorPalette {
private:
    ColorPaletteUBO palette{};
    GLuint colorUBO = 0; // binding = 7

    void updatePaletteUBO() {
        glBindBuffer(GL_UNIFORM_BUFFER, colorUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ColorPaletteUBO), &palette);
    }

    // Enforce monotonic thresholds (t1 <= t2 <= t3 <= t4)
    static void sortThresholds(vec4& t) {
        if (t.y < t.x) t.y = t.x;
        if (t.z < t.y) t.z = t.y;
        if (t.w < t.z) t.w = t.z;
    }

public:
    ColorPalette() {
        glGenBuffers(1, &colorUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, colorUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ColorPaletteUBO), nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 7, colorUBO);
        SetDefaults(); // fill palette
        updatePaletteUBO();
    }

    // Defaults
    void SetDefaults() {
        palette.terrainColors[0] = vec4(0.30f, 0.20f, 0.10f, 1.0f);
        palette.terrainColors[1] = vec4(0.45f, 0.35f, 0.20f, 1.0f);
        palette.terrainColors[2] = vec4(0.60f, 0.55f, 0.40f, 1.0f);
        palette.terrainColors[3] = vec4(0.75f, 0.75f, 0.65f, 1.0f);
        palette.terrainColors[4] = vec4(1.00f, 0.98f, 0.90f, 1.0f);
        palette.grassColor = vec4(0.18, 0.45, 0.16, 1.0);
        palette.waterColor = vec4(0.30f, 0.64f, 0.69f, 1.0f);
        palette.fogColor = CLEAR_COLOR;
        palette.angleThresholds = vec4(15.0f, 40.0f, 120.0f, 240.0f);
        palette.fogDensity = 0.000001f;
    }

    // ImGui editor
    bool DrawImGui(const char* title = "Colors") {
        bool changed = false;
        if (ImGui::CollapsingHeader(title)) {
            // Terrain colors
            for (int i = 0; i < 5; ++i) {
                float col[3] = { palette.terrainColors[i].x, palette.terrainColors[i].y, palette.terrainColors[i].z };
                char label[16];
                snprintf(label, sizeof(label), "Terrain %d", i);
                if (ImGui::ColorEdit3(label, col, ImGuiColorEditFlags_Float)) {
                    palette.terrainColors[i] = vec4(col[0], col[1], col[2], 1.0);
                    changed = true;
                }
            }

            // Grass / Water / Fog
            {
                float col[3];

                col[0] = palette.grassColor.x; col[1] = palette.grassColor.y; col[2] = palette.grassColor.z;
                if (ImGui::ColorEdit3("Grass", col)) { palette.grassColor = vec4(col[0], col[1], col[2], 1.0); changed = true; }

                col[0] = palette.waterColor.x; col[1] = palette.waterColor.y; col[2] = palette.waterColor.z;
                if (ImGui::ColorEdit3("Water", col)) { palette.waterColor = vec4(col[0], col[1], col[2], 1.0); changed = true; }

                col[0] = palette.fogColor.x; col[1] = palette.fogColor.y; col[2] = palette.fogColor.z;
                if (ImGui::ColorEdit3("Fog", col)) { palette.fogColor = vec4(col[0], col[1], col[2], 1.0); changed = true; }
            }

            ImGui::SeparatorText("Slope thresholds (deg)");
            {
                float treshold[4] = { palette.angleThresholds.x, palette.angleThresholds.y, palette.angleThresholds.z, palette.angleThresholds.w };
                bool tresholdChanged = false;
                tresholdChanged |= ImGui::DragFloat("t1", &treshold[0], 0.5f, 0.0f, 360.0f);
                tresholdChanged |= ImGui::DragFloat("t2", &treshold[1], 0.5f, 0.0f, 360.0f);
                tresholdChanged |= ImGui::DragFloat("t3", &treshold[2], 0.5f, 0.0f, 360.0f);
                tresholdChanged |= ImGui::DragFloat("t4", &treshold[3], 0.5f, 0.0f, 360.0f);
                if (tresholdChanged) {
                    palette.angleThresholds = vec4(treshold[0], treshold[1], treshold[2], treshold[3]);
                    sortThresholds(palette.angleThresholds);
                    changed = true;
                }
            }

            ImGui::SeparatorText("Fog");
            {
                float d = palette.fogDensity;
                if (ImGui::DragFloat("Fog Density", &d, 1e-6f, 0.0f, 1e-2f, "%.8f", ImGuiSliderFlags_Logarithmic)) {
                    palette.fogDensity = d;
                    changed = true;
                }
            }

            // Presets
            if (ImGui::Button("Preset: Desert")) {
                palette.terrainColors[0] = vec4(0.55f, 0.45f, 0.30f, 1.0f);
                palette.terrainColors[1] = vec4(0.70f, 0.55f, 0.35f, 1.0f);
                palette.terrainColors[2] = vec4(0.80f, 0.65f, 0.45f, 1.0f);
                palette.terrainColors[3] = vec4(0.90f, 0.78f, 0.60f, 1.0f);
                palette.terrainColors[4] = vec4(1.00f, 0.92f, 0.75f, 1.0f);
                palette.grassColor = vec4(0.55f, 0.60f, 0.30f, 1.0f);
                palette.waterColor = vec4(0.25f, 0.55f, 0.65f, 1.0f);
                changed = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Preset: Alpine")) {
                palette.terrainColors[0] = vec4(0.20f, 0.22f, 0.20f, 1.0f);
                palette.terrainColors[1] = vec4(0.30f, 0.34f, 0.30f, 1.0f);
                palette.terrainColors[2] = vec4(0.55f, 0.60f, 0.55f, 1.0f);
                palette.terrainColors[3] = vec4(0.80f, 0.85f, 0.82f, 1.0f);
                palette.terrainColors[4] = vec4(0.95f, 0.97f, 0.98f, 1.0f);
                palette.grassColor = vec4(0.20f, 0.55f, 0.25f, 1.0f);
                palette.waterColor = vec4(0.12f, 0.28f, 0.65f, 1.0f);
                changed = true;
            }

            if (ImGui::Button("Preset: Dusk")) {
                palette.terrainColors[0] = vec4(243, 233, 210, 255) / 255.0;
                palette.terrainColors[1] = vec4(233, 196, 106, 255) / 255.0;
                palette.terrainColors[2] = vec4(195, 122, 84, 255) / 255.0;
                palette.terrainColors[3] = vec4(107, 91, 149, 255) / 255.0;
                palette.terrainColors[4] = vec4(47, 42, 68, 255) / 255.0;
                palette.grassColor = vec4(210, 230, 120, 255) / 255.0;
                palette.waterColor = vec4(110, 170, 160, 255) / 255.0;
                changed = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Preset: Marill")) {
                palette.terrainColors[0] = HexRGB(0xf7d9a1);
                palette.terrainColors[1] = HexRGB(0xf2a78c);
                palette.terrainColors[2] = HexRGB(0xd78b88);
                palette.terrainColors[3] = HexRGB(0xb86b8a);
                palette.terrainColors[4] = HexRGB(0x7f4b8b);
                palette.grassColor = HexRGB(0x7f4b8b);
                palette.waterColor = HexRGB(0x7f4b8b);
                changed = true;
            }

            if (ImGui::Button("Preset: Celest")) {
                palette.terrainColors[0] = HexRGB(0x5aa8d8);
                palette.terrainColors[1] = HexRGB(0xa4d8e5);
                palette.terrainColors[2] = HexRGB(0xf9d4b4);
                palette.terrainColors[3] = HexRGB(0xf1a76a);
                palette.terrainColors[4] = HexRGB(0xd86f6f);
                palette.grassColor = HexRGB(0x7f4b8b);
                palette.waterColor = HexRGB(0x7f4b8b);
                changed = true;
            }

            if (ImGui::Button("Reset to Defaults")) {
                SetDefaults();
                changed = true;
            }
        }

        if (changed) updatePaletteUBO();
        return changed;
    }
};
