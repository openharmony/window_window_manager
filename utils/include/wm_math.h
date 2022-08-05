/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_ROSEN_WM_MATH_H
#define OHOS_ROSEN_WM_MATH_H

#include <cmath>
#include <limits>

namespace OHOS::Rosen {
namespace MathHelper {
constexpr float PI = 3.14159265f;
constexpr float INF = std::numeric_limits<float>::infinity();
constexpr float NAG_INF = -std::numeric_limits<float>::infinity();
constexpr float POS_ZERO = 0.001f;
constexpr float NAG_ZERO = -POS_ZERO;
inline bool NearZero(float val)
{
    return val < POS_ZERO && val > NAG_ZERO;
}

inline float ToRadians(float degrees)
{
    return degrees * PI / 180.0f;
}

inline float ToDegrees(float radians)
{
    return radians * 180.0f / PI;
}

inline bool LessNotEqual(double left, double right)
{
    static constexpr double eps = -0.001f;
    return (left - right) < eps;
}

inline bool GreatNotEqual(double left, double right)
{
    static constexpr double eps = 0.001f;
    return (left - right) > eps;
}

template <typename T>
T Max(const T& a, const T& b)
{
    return (a < b ? b : a);
}

template <typename T, typename... Ts>
T Max(const T& a, Ts... bs)
{
    T b = Max(bs...);
    return (a > b ? a : b);
}

template <typename T>
T Min(const T& a, const T& b)
{
    return (a < b ? a : b);
}

template <typename T, typename... Ts>
T Min(const T& a, Ts... bs)
{
    T b = Min(bs...);
    return (a < b ? a : b);
}

template <typename T>
T Clamp(const T& value, const T& lower, const T& upper)
{
    return Min(upper, Max(lower, value));
}
} // namespace MathHelper

namespace TransformHelper {
struct Vector2 {
    float x_, y_;
    Vector2() : x_(0.0f), y_(0.0f) {}
    Vector2(float inX, float inY)
        : x_(inX), y_(inY) {}
    friend Vector2 operator-(const Vector2& v)
    {
        return Vector2 { -v.x_, -v.y_ };
    }
    friend Vector2 operator+(const Vector2& a, const Vector2& b)
    {
        return Vector2 { a.x_ + b.x_, a.y_ + b.y_ };
    }
    friend Vector2 operator-(const Vector2& a, const Vector2& b)
    {
        return Vector2 { a.x_ - b.x_, a.y_ - b.y_ };
    }
    float LengthSq() const
    {
        return (x_ * x_ + y_ * y_);
    }
    float Length() const
    {
        return (std::sqrt(LengthSq()));
    }
};

struct Vector3 {
    float x_, y_, z_;
    Vector3() : x_(0.0f), y_(0.0f), z_(0.0f) {}
    Vector3(float inX, float inY, float inZ)
        : x_(inX), y_(inY), z_(inZ) {}
    friend Vector3 operator-(const Vector3& v)
    {
        return Vector3 { -v.x_, -v.y_, -v.z_ };
    }
    friend Vector3 operator+(const Vector3& a, const Vector3& b)
    {
        return Vector3 { a.x_ + b.x_, a.y_ + b.y_, a.z_ + b.z_ };
    }
    friend Vector3 operator-(const Vector3& a, const Vector3& b)
    {
        return Vector3 { a.x_ - b.x_, a.y_ - b.y_, a.z_ - b.z_ };
    }
    float LengthSq() const
    {
        return (x_ * x_ + y_ * y_ + z_ * z_);
    }
    float Length() const
    {
        return (std::sqrt(LengthSq()));
    }
    void Normalize()
    {
        float length = Length();
        if (length > MathHelper::POS_ZERO) {
            x_ /= length;
            y_ /= length;
            z_ /= length;
        }
    }
    static Vector3 Normalize(const Vector3& vec)
    {
        Vector3 temp = vec;
        temp.Normalize();
        return temp;
    }
    static float Dot(const Vector3& a, const Vector3& b)
    {
        return (a.x_ * b.x_ + a.y_ * b.y_ + a.z_ * b.z_);
    }
    static Vector3 Cross(const Vector3& a, const Vector3& b)
    {
        Vector3 temp;
        temp.x_ = a.y_ * b.z_ - a.z_ * b.y_;
        temp.y_ = a.z_ * b.x_ - a.x_ * b.z_;
        temp.z_ = a.x_ * b.y_ - a.y_ * b.x_;
        return temp;
    }
};

struct Matrix3 {
    float mat_[3][3];

    friend Matrix3 operator*(const Matrix3& left, const Matrix3& right);
    Matrix3& operator*=(const Matrix3& right);
    static const Matrix3 Identity;
};

struct Matrix4 {
    float mat_[4][4];

    friend Matrix4 operator*(const Matrix4& left, const Matrix4& right);
    Matrix4& operator*=(const Matrix4& right);
    void SwapRow(int row1, int row2);
    // Inverse matrix with Gauss-Jordan method
    void Invert();
    // Extract the scale component from the matrix
    Vector3 GetScale() const;
    // Get the translation component of the matrix
    Vector3 GetTranslation() const;
    static const Matrix4 Identity;
    static constexpr int MAT_SIZE = 4;
};

struct Plane {
    Plane() : normal_ { 0, 0, 1 }, d_ { 0 } {}
    Plane(const Vector3& normal, float d);
    Plane(const Vector3& a, const Vector3& b, const Vector3& c);
    float ComponentZ(float x, float y) const;
    // Compute the distance of parallel lines projected from this plane to xy plane
    // it is assumed that distance of parallel lines in this plane is 1
    // a, b determine a line in this plane
    float ParallelDistanceGrad(const Vector3& a, const Vector3& b) const;

    Vector3 normal_; // Normal vector of plane
    float d_; // Signed distance from (0,0,0) to plane
};
// Create a scale matrix with x and y scales(in xy-plane)
Matrix3 CreateScale(float xScale, float yScale);
// Create a rotation matrix about the Z axis
// theta is in radians
Matrix3 CreateRotation(float theta);
// Create a translation matrix (on the xy-plane)
Matrix3 CreateTranslation(const Vector2& trans);
// Create a scale matrix with x, y, and z scales
Matrix4 CreateScale(float xScale, float yScale, float zScale);
// Create a rotation matrix about X axis
// theta is in radians
Matrix4 CreateRotationX(float theta);
// Create a rotation matrix about Y axis
// theta is in radians
Matrix4 CreateRotationY(float theta);
// Create a rotation matrix about Z axis
// theta is in radians
Matrix4 CreateRotationZ(float theta);
// Create a 3D translation matrix
Matrix4 CreateTranslation(const Vector3& trans);
// Transform a Vector2 in xy-plane by matrix3
Vector2 Transform(const Vector2& vec, const Matrix3& mat);
// Transform a Vector3 in 3D world by matrix4
Vector3 Transform(const Vector3& vec, const Matrix4& mat);
} // namespace TransformHelper
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WM_MATH_H