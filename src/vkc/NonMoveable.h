/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_NONMOVEABLE_H
#define VULKANCUBE_NONMOVEABLE_H

class NonMoveable
{
public:
    NonMoveable() = default;
    NonMoveable(NonMoveable&&) = delete;
    auto operator=(NonMoveable&&) -> NonMoveable& = delete;
};

#endif //VULKANCUBE_NONMOVEABLE_H
