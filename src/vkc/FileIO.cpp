/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include <fstream>
#include "FileIO.h"
#include "Types.h"

auto vkc::FileIO::ReadFile(const std::string &fileName, std::vector<char> &buffer) -> void
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    type::size fileSize = static_cast<type::size>(file.tellg());

    buffer.resize(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
}