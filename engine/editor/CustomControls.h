#pragma once

namespace ImGui
{
    bool GCheckbox(const std::string& label, bool* value);
    bool GColorEdit3(const std::string& label, glm::vec3& value);
    bool GColorEdit4(const std::string& label, glm::vec4& value);
    bool GDragFloat(const std::string& label, float* value, float step = 0.01f, float min = 0.0f, float max = 0.0f);
    bool GDragInt(const std::string& label, int* value, int step = 1, int min = 0, int max = 0);
    bool GDragFloatVec3(const std::string& label, glm::vec3& values, float step = 0.01f, float min = 0.0f, float max = 0.0f);
    bool GDragTransform(glm::vec3& pos, glm::vec3& rot, glm::vec3& scale, float step = 0.01f, float min = 0.0f, float max = 0.0f);
    bool GAssetHolder(const std::string& name, const std::string& source);
    bool GTextInput(const std::string& label, std::string* source, bool enabled = true);

    bool FileOpen(const char* label, const char* btn, char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns);
    bool FileSave(const char* label, const char* btn, char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns);
}