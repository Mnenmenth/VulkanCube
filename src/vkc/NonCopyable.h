/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_NONCOPYABLE_H
#define VULKANCUBE_NONCOPYABLE_H
class NonCopyable
{
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    auto operator=(const NonCopyable&) -> NonCopyable& = delete;
};
#endif //VULKANCUBE_NONCOPYABLE_H
