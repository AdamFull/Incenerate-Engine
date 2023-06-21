#pragma once

namespace ImGui
{
    constexpr const float widthScale = 0.2f;

    void GText(const char* label, const char* fmt, ...);
    bool GTextInput(const char* label, std::string* value, bool enabled = true);
    bool GCheckbox(const char* label, bool* value);
    bool GColorEdit3(const char* label, glm::vec3& value);
    bool GColorEdit4(const char* label, glm::vec4& value);

    // Drag scalar
    bool GDragInt(const char* label, int* value, int step = 1, int min = 0, int max = 0);
    bool GDragFloat(const char* label, float* value, float step = 0.01f, float min = 0.0f, float max = 0.0f);
    bool GDragDouble(const char* label, double* value, double step = 0.01, double min = 0.0, double max = 0.0);
    
    // Drag vec3
    bool GDragIntVec3(const char* label, int value[3], int step = 1, int min = 0, int max = 0);
    bool GDragFloatVec3(const char* label, float value[3], float step = 0.01f, float min = 0.0f, float max = 0.0f);
    bool GDragDoubleVec3(const char* label, double value[3], double step = 0.01, double min = 0.0, double max = 0.0);

    // Drag vec4
    bool GDragIntVec4(const char* label, int value[4], int step = 1, int min = 0, int max = 0);
    bool GDragFloatVec4(const char* label, float value[4], float step = 0.01f, float min = 0.0f, float max = 0.0f);
    bool GDragDoubleVec4(const char* label, double value[4], double step = 0.01, double min = 0.0, double max = 0.0);

    bool GDragTransform(glm::vec3& pos, glm::quat& rot, glm::vec3& scale, float step = 0.01f, float min = 0.0f, float max = 0.0f);
    bool GAssetHolder(const char* label, const std::string& source);
    bool GCombo(const char* label, int* current_item, const char* const items[], int item_count, int height_in_items = -1);
    bool GSliderInt(const char* label, int* value, int min, int max);

    bool FileOpen(char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns);
    bool FileSave(char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns);
}