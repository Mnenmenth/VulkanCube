/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_FILEIO_H
#define VULKANCUBE_FILEIO_H

#include <string>
#include <vector>

namespace vkc::FileIO
{
    auto ReadFile(const std::string& fileName, std::vector<char>& buffer) -> void;
}

#endif //VULKANCUBE_FILEIO_H
