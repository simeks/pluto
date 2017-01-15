#ifndef __CORE_IMAGE_H__
#define __CORE_IMAGE_H__

#include "Color.h"
#include "Types.h"
#include "Vec3.h"
#include "Vec2.h"

#include <Core/Python/NumPy.h>

namespace image
{
    enum BorderMode
    {
        Border_Constant,
        Border_Replicate
    };

}

class CORE_API Image
{
public:
    Image();
    Image(int ndims, const Vec3i& size, int pixel_type, const uint8_t* data = nullptr);
    ~Image();

    void create(int ndims, const Vec3i& size, int pixel_type, const uint8_t* data = nullptr);
    void set(int ndims, const Vec3i& size, int pixel_type, const NumpyArray& data);

    void release();

    void set_origin(const Vec3d& origin);
    void set_spacing(const Vec3d& spacing);

    const Vec3i& size() const;
    const Vec3d& origin() const;
    const Vec3d& spacing() const;
    int ndims() const;

    int pixel_type() const;
    size_t pixel_size() const;
    size_t pixel_count() const;

    Image clone() const;

    bool valid() const;

    INLINE const uint8_t* ptr() const;
    INLINE uint8_t* ptr();

    template<typename T>
    const T* ptr() const;
    template<typename T>
    T* ptr();

    // Offset in bytes to the specified element
    size_t offset(int x, int y, int z) const;
    size_t offset(int x, int y) const;
    size_t offset(int x) const;

    /// Copies the content of this image to the specified destination.
    /// The caller is self responsible for making sure dest is large enough.
    void copy_to(uint8_t* dest) const;

    Image convert_to(int type) const;

    /// @remark This does not copy the data, use clone if you want a separate copy.
    Image(const Image& other);
    Image& operator=(const Image& other);

    operator bool() const;

    const NumpyArray& numpy_array() const;

protected:
    void numpy_shape(int ndims, const Vec3i& size, int pixel_type, int& npy_ndims, npy_intp* npy_dims) const;

    NumpyArray _data;
    int _ndims;
    Vec3i _size;
    size_t _step[3];

    Vec3d _origin;
    Vec3d _spacing;

    int _pixel_type;
};

template<typename T>
class ImageTpl : public Image
{
public:
    typedef T TPixelType;

    ImageTpl();
    explicit ImageTpl(int ndims, const Vec3i& size, const T* data = nullptr);
    
    /// Fills the image with the specified value.
    explicit ImageTpl(int ndims, const Vec3i& size, const T& value);

    ImageTpl(const Image& other);
    ImageTpl& operator=(const Image& other);

    void fill(const T& value);

    const TPixelType& at(int x, int y, int z) const;
    const TPixelType& at(const Vec3i& v) const;

    TPixelType& at(int x, int y, int z);
    TPixelType& at(const Vec3i& v);

    TPixelType at(int x, int y, int z, image::BorderMode border) const;
    TPixelType at(const Vec3i& v, image::BorderMode border) const;

    INLINE TPixelType linear_at(double x, double y, double z, image::BorderMode border = image::Border_Constant) const;
    INLINE TPixelType linear_at(const Vec3d& v, image::BorderMode border = image::Border_Constant) const;

    const TPixelType& operator[](int64_t index) const;
    TPixelType& operator[](int64_t index);

    INLINE const TPixelType& operator()(int x, int y, int z) const;
    INLINE TPixelType& operator()(int x, int y, int z);

    INLINE const TPixelType& operator()(int x, int y) const;
    INLINE TPixelType& operator()(int x, int y);

    INLINE const TPixelType& operator()(const Vec3i& p) const;
    INLINE TPixelType& operator()(const Vec3i& p);
};

typedef ImageTpl<uint8_t> ImageUInt8;
typedef ImageTpl<int8_t> ImageInt8;
typedef ImageTpl<uint16_t> ImageUInt16;
typedef ImageTpl<int16_t> ImageInt16;
typedef ImageTpl<uint32_t> ImageUInt32;
typedef ImageTpl<int32_t> ImageInt32;
typedef ImageTpl<float> ImageFloat32;
typedef ImageTpl<double> ImageFloat64;
typedef ImageTpl<Vec3u8> ImageVec3u8;
typedef ImageTpl<Vec3f> ImageVec3f;
typedef ImageTpl<Vec3d> ImageVec3d;
typedef ImageTpl<RGBA32> ImageRGBA32;
typedef ImageTpl<Colorf> ImageColorf;
typedef ImageTpl<Colord> ImageColord;


#include "Image.inl"

#endif // __CORE_IMAGE_H__
