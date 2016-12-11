#ifndef __CORE_FLOW_IMAGE_H__
#define __CORE_FLOW_IMAGE_H__

#include <Core/Object/Object.h>
#include <Core/Image/Image.h>

class CORE_API ImageObject : public Object
{
    DECLARE_OBJECT(ImageObject, Object);
public:
    ImageObject();
    ImageObject(const Image& img);
    virtual ~ImageObject();

    void allocate(int type, const std::vector<uint32_t>& size);
    void set_image(const Image& img);
    bool set_image(PyObject* npy_array, int pixel_type_hint);
    void release_image();

    void set_origin(const Vec3d&);
    void set_spacing(const Vec3d&);

    const Vec3i& size() const;
    const Vec3d& origin() const;
    const Vec3d& spacing() const;
    int ndims() const;

    int pixel_type() const;

    Image& image();
    const Image& image() const;

    ImageObject(const ImageObject&);
    ImageObject& operator=(const ImageObject&);

private:
    int object_init(const Tuple& args, const Dict&) OVERRIDE;


    Image _image;
};


#endif // __CORE_FLOW_IMAGE_H__
