#include <Core/Common.h>

#include <Flow/FlowContext.h>
#include <Core/Image/ImageObject.h>
#include <Core/Image/Image.h>
#include <Core/Python/PythonCommon.h>

#include "BlockedGraphCutOptimizer.h"
#include "RegistrationEngine.h"
#include "Registration.h"

void registration::registration_node(FlowContext* ctx)
{
    RegistrationEngine::Params params;
    int pixel_type = image::PixelType_Unknown;

    for (int i = 0; i < 3; ++i)
    {
        std::stringstream ss; 
        
        ss << "Fixed" << i;
        ImageObject* fixed = ctx->read_pin<ImageObject*>(ss.str().c_str());
        ss.str("");

        ss << "Moving" << i;
        ImageObject* moving = ctx->read_pin<ImageObject*>(ss.str().c_str());

        if (!fixed || !moving)
            continue;

        Image fixed_img = fixed->image();
        Image moving_img = moving->image();

        if (!fixed_img || !moving_img)
            PYTHON_ERROR(ValueError, "Not valid images!");

        if (fixed_img.pixel_type() != moving_img.pixel_type() ||
            fixed_img.size() != moving_img.size())
            PYTHON_ERROR(ValueError, "Both images needs to be of same type and size.");

        switch (fixed_img.pixel_type())
        {
        case image::PixelType_UInt8:
        case image::PixelType_UInt16:
        case image::PixelType_UInt32:
        case image::PixelType_Float32:
            if (pixel_type == image::PixelType_Unknown)
                pixel_type = image::PixelType_Float32;

            // Convert if necessary
            params.fixed.push_back(ImageFloat32(fixed_img)); 
            params.moving.push_back(ImageFloat32(moving_img));

            break;
        case image::PixelType_Float64:
            if (pixel_type == image::PixelType_Unknown)
                pixel_type = image::PixelType_Float64;

            // Convert if necessary
            params.fixed.push_back(ImageFloat64(fixed_img));
            params.moving.push_back(ImageFloat64(moving_img));

            break;
        case image::PixelType_Vec4u8:
        case image::PixelType_Vec4f:
            if (pixel_type == image::PixelType_Unknown)
                pixel_type = image::PixelType_Vec4f;

            // Convert if necessary
            params.fixed.push_back(ImageColorf(fixed_img));
            params.moving.push_back(ImageColorf(moving_img));

            break;
        case image::PixelType_Vec3u8:
        case image::PixelType_Vec3f:
        case image::PixelType_Vec3d:
        default:
            PYTHON_ERROR(ValueError, "RegistrationNode: Could not register image pair.");
        }
    }

    if (params.fixed.empty())
        PYTHON_ERROR(ValueError, "Expects at least one valid image pair.");

    ImageObject* constraint_mask = ctx->read_pin<ImageObject*>("ConstraintMask");
    if (constraint_mask)
    {
        Image mask_img = constraint_mask->image();
        if (mask_img.pixel_type() != image::PixelType_UInt8)
        {
            PYTHON_ERROR(ValueError, "Constraint mask needs to be of UInt8 type.");
        }

        if (mask_img.size() != params.fixed.back().size())
            PYTHON_ERROR(ValueError, "Constraint mask needs to be of same size as the source and reference images.");

        params.constraint_mask = mask_img;
    }

    ImageObject* constraint_values = ctx->read_pin<ImageObject*>("ConstraintValues");
    if (constraint_values)
    {
        Image values_img = constraint_values->image();
        if (values_img.pixel_type() != image::PixelType_Vec3d)
            PYTHON_ERROR(ValueError, "Constraint value image needs to be of Vec3d type.");

        if (values_img.size() != params.fixed.back().size())
            PYTHON_ERROR(ValueError, "Constraint value image needs to be of same size as the source and reference images.");

        params.constraint_values = values_img;
    }

    ImageObject* starting_guess = ctx->read_pin<ImageObject*>("StartingGuess");
    if (starting_guess)
    {
        Image guess_img = starting_guess->image();
        if (guess_img.pixel_type() != image::PixelType_Vec3d)
            PYTHON_ERROR(ValueError, "Starting guess needs to be of Vec3d type.");

        if (guess_img.size() != params.fixed.back().size())
            PYTHON_ERROR(ValueError, "Starting guess needs to be of same size as the source and reference images.");

        params.starting_guess = guess_img;
    }

    Image def;
    //switch (pixel_type)
    //{
    //case image::PixelType_Float32:
    //    def = exec_registration<ImageFloat32>(params);
    //    break;
    //case image::PixelType_Float64:
    //    def = exec_registration<ImageFloat64>(params);
    //    break;
    //case image::PixelType_Vec4f:
    //    def = exec_registration<ImageColorf>(params);
    //    break;
    //default:
    //    PYTHON_ERROR(ValueError, "RegistrationNode: Could not register image pair.");
    //}

    if (def)
    {
        ImageObject* result = object_new<ImageObject>(def);
        ctx->write_pin("Deformation", result);
    }
}
