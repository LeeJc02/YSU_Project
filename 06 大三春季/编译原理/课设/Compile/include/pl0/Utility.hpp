// 文件: Utility.hpp
// 功能: 声明通用工具函数
#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace pl0 {

// 函数: 读取 UTF-8 文件内容
[[nodiscard]] std::string read_file_utf8(const std::filesystem::path& path);

// 函数: 将文本按行切分
[[nodiscard]] std::vector<std::string_view> split_lines(std::string_view text);

// 函数: 去除行尾回车
void trim_trailing_cr(std::string& line);

}  // namespace pl0
