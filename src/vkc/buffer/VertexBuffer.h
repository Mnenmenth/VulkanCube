/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_VERTEXBUFFER_H
#define VULKANCUBE_VERTEXBUFFER_H


#include "../NonCopyable.h"

namespace vkc
{
    class Device;
    class Buffer;
    class VertexBuffer : public NonCopyable
    {
    public:
        //! Buffer must already be appropriately sized
        VertexBuffer();
        ~VertexBuffer();
    };
}


#endif //VULKANCUBE_VERTEXBUFFER_H
