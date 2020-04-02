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
    NonCopyable& operator=(const NonCopyable&) = delete;
};
#endif //VULKANCUBE_NONCOPYABLE_H
