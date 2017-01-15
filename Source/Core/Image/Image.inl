
const uint8_t* Image::ptr() const
{
    assert(valid());
    return (uint8_t*)_data.data();
}
uint8_t* Image::ptr()
{
    assert(valid());
    return (uint8_t*)_data.data();
}

template<typename T>
const T* Image::ptr() const
{
    return (T*)ptr();
}
template<typename T>
T* Image::ptr()
{
    return (T*)ptr();
}

INLINE size_t Image::offset(int x, int y, int z) const
{
    assert(x >= 0 && x < _size.x);
    assert(y >= 0 && y < _size.y);
    assert(z >= 0 && z < _size.z);
    return x * _step[0] + y * _step[1] + z * _step[2];
}
INLINE size_t Image::offset(int x, int y) const
{
    assert(_ndims >= 2);
    assert(x >= 0 && x < _size.x);
    assert(y >= 0 && y < _size.y);

    return x * _step[0] + y * _step[1];
}
INLINE size_t Image::offset(int x) const
{
    assert(_ndims >= 1);
    assert(x >= 0 && x < _size.x);
    return x * _step[0];
}
INLINE Image::operator bool() const
{
    return valid();
}

template<typename T>
ImageTpl<T>::ImageTpl() : Image()
{
    _pixel_type = image::pixel_type<T>::TypeId;
}
template<typename T>
ImageTpl<T>::ImageTpl(int ndims, const Vec3i& size, const T* data) : Image(ndims, size, image::pixel_type<T>::TypeId, (const uint8_t*)data)
{
}
template<typename T>
ImageTpl<T>::ImageTpl(int ndims, const Vec3i& size, const T& value) : Image(ndims, size, image::pixel_type<T>::TypeId)
{
    fill(value);
}

template<typename T>
ImageTpl<T>::ImageTpl(const Image& other) : Image(other)
{
    *this = other;
}
template<typename T>
ImageTpl<T>& ImageTpl<T>::operator=(const Image& other)
{
    if (other.pixel_type() == image::pixel_type<T>::TypeId)
    {
        Image::operator=(other);
        return *this;
    }

    *this = other.convert_to(image::pixel_type<T>::TypeId);
    return *this;
}
template<typename T>
void ImageTpl<T>::fill(const T& value)
{
    assert(_data.is_contiguous());
    std::fill(ptr<T>(), ptr<T>() + pixel_count(), value);
}

template<typename T>
const T& ImageTpl<T>::at(int x, int y, int z) const
{
    return *((T*)(_data_ptr + offset(x, y, z)));
}
template<typename T>
const T& ImageTpl<T>::at(const Vec3i& v) const
{
    return at(v.x, v.y, v.z);
}
template<typename T>
T& ImageTpl<T>::at(int x, int y, int z)
{
    return *((T*)(_data_ptr + offset(x, y, z)));
}
template<typename T>
T& ImageTpl<T>::at(const Vec3i& v)
{
    return at(v.x, v.y, v.z);
}

template<typename T>
T ImageTpl<T>::at(int x, int y, int z, image::BorderMode border) const
{
    if (border == image::Border_Constant) 
    {
        if (x < 0 || ceil(x) >= _size.x ||
            y < 0 || ceil(y) >= _size.y ||
            z < 0 || ceil(z) >= _size.z) 
        {
            return T(0);
        }
    }
    else if (border == image::Border_Replicate)
    {
        x = std::max(x, 0);
        x = std::min(x, _size.x - 1);
        y = std::max(y, 0);
        y = std::min(y, _size.y - 1);
        z = std::max(z, 0);
        z = std::min(z, _size.z - 1);
    }

    return *((T*)(ptr() + offset(x, y, z)));
}
template<typename T>
T ImageTpl<T>::at(const Vec3i& v, image::BorderMode border) const
{
    return at(v.x, v.y, v.z, border);
}
template<typename T>
T ImageTpl<T>::linear_at(double x, double y, double z, image::BorderMode border) const
{
    if (border == image::Border_Constant) {
        if (x < 0 || ceil(x) >= _size.x ||
            y < 0 || ceil(y) >= _size.y ||
            z < 0 || ceil(z) >= _size.z) {
            return T(0);
        }
    }
    else if (border == image::Border_Replicate)
    {
        x = std::max(x, 0.0);
        x = std::min(x, _size.x - 1.0);
        y = std::max(y, 0.0);
        y = std::min(y, _size.y - 1.0);
        z = std::max(z, 0.0);
        z = std::min(z, _size.z - 1.0);
    }

    double xt = x - floor(x);
    double yt = y - floor(y);
    double zt = z - floor(z);

    int x1 = int(floor(x));
    int x2 = int(ceil(x));
    int y1 = int(floor(y));
    int y2 = int(ceil(y));
    int z1 = int(floor(z));
    int z2 = int(ceil(z));

    return T((1 - zt)*((1 - yt)*((1 - xt)*operator()(x1, y1, z1) +
        (xt)*operator()(x2, y1, z1)) +
        (yt)*((1 - xt)*operator()(x1, y2, z1) +
        (xt)*operator()(x2, y2, z1))) +
        (zt)*((1 - yt)*((1 - xt)*operator()(x1, y1, z2) +
        (xt)*operator()(x2, y1, z2)) +
        (yt)*((1 - xt)*operator()(x1, y2, z2) +
        (xt)*operator()(x2, y2, z2))));
}
template<typename T>
T ImageTpl<T>::linear_at(const Vec3d& v, image::BorderMode border) const
{
    return linear_at(v.x, v.y, v.z, border);
}
template<typename T>
const T& ImageTpl<T>::operator[](int64_t index) const
{
    assert(valid());
    assert(index < (int64_t)pixel_count());
    // TODO: Assumes continuous memory
    return ((T*)ptr())[index];
}
template<typename T>
T& ImageTpl<T>::operator[](int64_t index)
{
    assert(valid());
    assert(index < (int64_t)pixel_count());
    assert(_data.is_contiguous());
    // TODO: Assumes contiguous memory
    return ((T*)ptr())[index];
}
template<typename T>
const T& ImageTpl<T>::operator()(int x, int y, int z) const
{
    assert(valid());
    assert(x >= 0 && x < _size.x);
    assert(y >= 0 && y < _size.y);
    assert(z >= 0 && z < _size.z);
    return *((T*)(ptr() + offset(x, y, z)));
}
template<typename T>
T& ImageTpl<T>::operator()(int x, int y, int z)
{
    assert(valid());
    assert(x >= 0 && x < _size.x);
    assert(y >= 0 && y < _size.y);
    assert(z >= 0 && z < _size.z);
    return *((T*)(ptr() + offset(x, y, z)));
}
template<typename T>
const T& ImageTpl<T>::operator()(int x, int y) const
{
    assert(valid());
    assert(x >= 0 && x < _size.x);
    assert(y >= 0 && y < _size.y);
    return *((T*)(ptr() + offset(x, y)));
}
template<typename T>
T& ImageTpl<T>::operator()(int x, int y)
{
    assert(valid());
    assert(x >= 0 && x < _size.x);
    assert(y >= 0 && y < _size.y);
    return *((T*)(ptr() + offset(x, y)));
}
template<typename T>
const T& ImageTpl<T>::operator()(const Vec3i& p) const
{
    assert(valid());
    assert(p.x >= 0 && p.x < _size.x);
    assert(p.y >= 0 && p.y < _size.y);
    assert(p.z >= 0 && p.z < _size.z);
    return *((T*)(ptr() + offset(p.x, p.y, p.z)));
}
template<typename T>
T& ImageTpl<T>::operator()(const Vec3i& p)
{
    assert(valid());
    assert(p.x >= 0 && p.x < _size.x);
    assert(p.y >= 0 && p.y < _size.y);
    assert(p.z >= 0 && p.z < _size.z);
    return *((T*)(ptr() + offset(p.x, p.y, p.z)));
}
