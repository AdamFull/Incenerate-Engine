#pragma once

namespace ImGui
{
    bool GCheckbox(const char* label, bool* value);
    bool GColorEdit3(const char* label, glm::vec3& value);
    bool GColorEdit4(const char* label, glm::vec4& value);
    bool GDragFloat(const char* label, float* value, float step = 0.01f, float min = 0.0f, float max = 0.0f);
    bool GDragInt(const char* label, int* value, int step = 1, int min = 0, int max = 0);
    bool GDragFloatVec3(const char* label, glm::vec3& values, float step = 0.01f, float min = 0.0f, float max = 0.0f);
    bool GDragTransform(glm::vec3& pos, glm::vec3& rot, glm::vec3& scale, float step = 0.01f, float min = 0.0f, float max = 0.0f);
    bool GAssetHolder(const char* label, const std::string& source);
    bool GTextInput(const char* label, std::string* source, bool enabled = true);
    bool GCombo(const char* label, int* current_item, const char* const items[], int item_count, int height_in_items = -1);

    bool FileOpen(const char* label, const char* btn, char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns);
    bool FileSave(const char* label, const char* btn, char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns);
}