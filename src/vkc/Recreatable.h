/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_RECREATABLE_H
#define VULKANCUBE_RECREATABLE_H

namespace vkc
{
    class Recreatable
    {
    public:
        // Creates the item
            // If an instance is already created, the current instance is passed
            // as pNext
        virtual auto recreate() -> void = 0;
        // Cleanup old handles from recreation
        virtual auto cleanup() -> void = 0;
    };
}

#endif //VULKANCUBE_RECREATABLE_H
