// 文件: Utility.cpp
// 功能: 实现常用工具函数
#include "pl0/Utility.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace pl0 {

// 函数: 读取 UTF-8 文件内容
std::string read_file_utf8(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + path.string());
  }
  std::ostringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

// 函数: 按换行符切分文本
std::vector<std::string_view> split_lines(std::string_view text) {
  std::vector<std::string_view> lines;
  std::size_t start = 0;
  while (start <= text.size()) {
    std::size_t end = text.find('\n', start);
    if (end == std::string_view::npos) {
      lines.emplace_back(text.substr(start));
      break;
    }
    lines.emplace_back(text.substr(start, end - start));
    start = end + 1;
  }
  return lines;
}

// 函数: 去除行尾回车符
void trim_trailing_cr(std::string& line) {
  if (!line.empty() && line.back() == '\r') {
    line.pop_back();
  }
}

}  // namespace pl0
