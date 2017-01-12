#ifndef __REGISTRATION_OPTIMIZER_H__
#define __REGISTRATION_OPTIMIZER_H__

#include <Core/Image/Image.h>

class Optimizer
{
public:
    virtual void execute(
        const Image* fixed, 
        const Image* moving, 
        int pair_count,
        const ImageUInt8& constraint_mask,
        const ImageVec3d& constraint_values,
        ImageVec3d& def) = 0;
};

#endif // __REGISTRATION_OPTIMIZER_H__
