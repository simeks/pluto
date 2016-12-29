#ifndef __CORE_FLOW_IMAGE_H__
#define __CORE_FLOW_IMAGE_H__

#include <Core/Object/Object.h>
#include <Core/Image/Image.h>

class CORE_API ImageObject : public Object
{
    DECLARE_OBJECT(ImageObject, Object);
public:
    DECLARE_OBJECT_CONSTRUCTOR(ImageObject);
    ~ImageObject();

    void object_init();
    void object_init(const Image& image);
    void object_python_init(const Tuple& args, const Dict& kw);

    void set_image(const Image& img);
    bool set_image(PyObject* npy_array, int pixel_type_hint);
    
    void set_origin(const Vec3d&);
    void set_spacing(const Vec3d&);

    const Vec3i& size() const;
    const Vec3d& origin() const;
    const Vec3d& spacing() const;
    int ndims() const;

    int pixel_type() const;

    Image& image();
    const Image& image() const;

    /// Returns the numpy array
    PyObject* array() const;
    PyObject* __array__(const Tuple& t) const;

    /// Sets the image data to the data in the specified array. 
    /// Remark: This may change the dimensions and the pixel format of the image but origin and spacing will remain the same. 
    void set_array(PyObject* arr, image::PixelType pixel_type_hint);

private:
    Image _image;
};


#endif // __CORE_FLOW_IMAGE_H__
