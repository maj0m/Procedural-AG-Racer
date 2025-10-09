#pragma once
#include "framework.h"
#include <imgui.h>

struct ColorPaletteUBO {
    vec4 terrainColors[5];
    vec4 angleThresholds; // degrees
    vec4 grassColor;
    vec4 waterColor;
    vec4 skyColor;
    vec4 atmosphereColor;
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
        palette.terrainColors[0] = HexRGB(0xF3E9D2);
        palette.terrainColors[1] = HexRGB(0xE9C46A);
        palette.terrainColors[2] = HexRGB(0xC37A54);
        palette.terrainColors[3] = HexRGB(0x6B5B95);
        palette.terrainColors[4] = HexRGB(0x2F2A44);
        palette.angleThresholds = vec4(15.0f, 40.0f, 120.0f, 240.0f);
        palette.grassColor = HexRGB(0xD2E678);
        palette.waterColor = HexRGB(0x6EAAA0);
        palette.skyColor = vec4(0.06f, 0.22f, 0.60f, 1.0f);
        palette.atmosphereColor = vec4(0.65f, 0.78f, 0.90f, 1.0f);
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

                col[0] = palette.skyColor.x; col[1] = palette.skyColor.y; col[2] = palette.skyColor.z;
                if (ImGui::ColorEdit3("Fog", col)) { palette.skyColor = vec4(col[0], col[1], col[2], 1.0); changed = true; }
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
            if (ImGui::Button("Preset: Dusk")) {
                palette.terrainColors[0] = HexRGB(0xF3E9D2);
                palette.terrainColors[1] = HexRGB(0xE9C46A);
                palette.terrainColors[2] = HexRGB(0xC37A54);
                palette.terrainColors[3] = HexRGB(0x6B5B95);
                palette.terrainColors[4] = HexRGB(0x2F2A44);
                palette.angleThresholds = vec4(15.0f, 40.0f, 120.0f, 240.0f);
                palette.grassColor = HexRGB(0xD2E678);
                palette.waterColor = HexRGB(0x6EAAA0);
                palette.skyColor = HexRGB(0x0F3899);
                palette.fogDensity = 0.000001f;
                changed = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Preset: Mint")) {
                palette.terrainColors[0] = HexRGB(0xA8E6CF);
                palette.terrainColors[1] = HexRGB(0xDDEDC4);
                palette.terrainColors[2] = HexRGB(0xFFD4B8);
                palette.terrainColors[3] = HexRGB(0xFFADAD);
                palette.terrainColors[4] = HexRGB(0xFF667D);
                palette.angleThresholds = vec4(20.0f, 50.0f, 70.0f, 130.0f);
                palette.grassColor = HexRGB(0xA8E6CF);
                palette.waterColor = HexRGB(0x72E1DA);
                palette.skyColor = HexRGB(0xA6EDFD);
                palette.fogDensity = 0.0000007f;
                changed = true;
            }
            
            if (ImGui::Button("Preset: Cherry")) {
                palette.terrainColors[0] = HexRGB(0xF6E48E);
                palette.terrainColors[1] = HexRGB(0xF8C391);
                palette.terrainColors[2] = HexRGB(0xF67481);
                palette.terrainColors[3] = HexRGB(0xC06D84);
                palette.terrainColors[4] = HexRGB(0x6C5B80);
                palette.angleThresholds = vec4(20.0f, 60.0f, 90.0f, 120.0f);
                palette.grassColor = HexRGB(0xF67481);
                palette.waterColor = HexRGB(0x91F1E0);
                palette.skyColor = HexRGB(0x46E0B7);
                palette.fogDensity = 0.000001f;
                changed = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Preset: Volcano")) {
                palette.terrainColors[0] = HexRGB(0x43474C);
                palette.terrainColors[1] = HexRGB(0x151618);
                palette.terrainColors[2] = HexRGB(0x522A27);
                palette.terrainColors[3] = HexRGB(0xC73E1D);
                palette.terrainColors[4] = HexRGB(0xE0BE36);
                palette.angleThresholds = vec4(20.0f, 60.0f, 90.0f, 130.0f);
                palette.grassColor = HexRGB(0x3A4936);
                palette.waterColor = HexRGB(0x3E6562);
                palette.skyColor = HexRGB(0xFF7100);
                palette.fogDensity = 0.0000016f;
                changed = true;
            }

            if (ImGui::Button("Preset: Alien")) {
                palette.terrainColors[0] = HexRGB(0x4D6AFF);
                palette.terrainColors[1] = HexRGB(0x9A5CFF);
                palette.terrainColors[2] = HexRGB(0xFF57B9);
                palette.terrainColors[3] = HexRGB(0xFFEC70);
                palette.terrainColors[4] = HexRGB(0x00FF9D);
                palette.angleThresholds = vec4(20.0f, 50.0f, 100.0f, 150.0f);
                palette.grassColor = HexRGB(0x4D6AFF);
                palette.waterColor = HexRGB(0x41B0EC);
                palette.skyColor = HexRGB(0xB977E1);
                palette.fogDensity = 0.000001f;
                changed = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Preset: Anime")) {
                palette.terrainColors[0] = HexRGB(0xFF6E61);
                palette.terrainColors[1] = HexRGB(0xFFB84D);
                palette.terrainColors[2] = HexRGB(0x6D9DC5);
                palette.terrainColors[3] = HexRGB(0x5E4B8B);
                palette.terrainColors[4] = HexRGB(0xDA1B61);
                palette.angleThresholds = vec4(20.0f, 50.0f, 70.0f, 130.0f);
                palette.grassColor = HexRGB(0x7F9459);
                palette.waterColor = HexRGB(0x3CD4E2);
                palette.skyColor = HexRGB(0xFFFA4D);
                palette.fogDensity = 0.00000075f;
                changed = true;
            }

            if (ImGui::Button("Preset: Neon")) {
                palette.terrainColors[0] = HexRGB(0x04E762);
                palette.terrainColors[1] = HexRGB(0xF5B700);
                palette.terrainColors[2] = HexRGB(0xDC0073);
                palette.terrainColors[3] = HexRGB(0x008BF8);
                palette.terrainColors[4] = HexRGB(0x89FC00);
                palette.angleThresholds = vec4(20.0f, 50.0f, 100.0f, 140.0f);
                palette.grassColor = HexRGB(0x22E594);
                palette.waterColor = HexRGB(0x4AC6A5);
                palette.skyColor = HexRGB(0x2DE5EB);
                palette.fogDensity = 0.0000005f;
                changed = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Preset: Harmony")) {
                palette.terrainColors[0] = HexRGB(0xF4F1DE);
                palette.terrainColors[1] = HexRGB(0xE07A5F);
                palette.terrainColors[2] = HexRGB(0x3D405B);
                palette.terrainColors[3] = HexRGB(0x81B29A);
                palette.terrainColors[4] = HexRGB(0xF2CC8F);
                palette.angleThresholds = vec4(30.0f, 60.0f, 100.0f, 140.0f);
                palette.grassColor = HexRGB(0x81B29A);
                palette.waterColor = HexRGB(0x4AC6A5);
                palette.skyColor = HexRGB(0x7FE9CC);
                palette.fogDensity = 0.0000012f;
                changed = true;
            }

            if (ImGui::Button("Reset to Defaults")) {
                SetDefaults();
            }
        }

        if (changed) updatePaletteUBO();
        return changed;
    }
};
