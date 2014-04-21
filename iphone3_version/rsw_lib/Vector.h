#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <stdio.h>

// 180 / Pi
#define DEG_PER_RAD 57.29577951

// Pi / 180
#define RAD_PER_DEG 0.017453292

/*
 * Vector datatype and operators
 */
class Vector
{
public:
    
    float x;
	float y;
    
    Vector(float fx = 0.0, float fy = 0.0) : x(fx), y(fy)
    {
    }
    
	inline void zero() { x = 0; y = 0; }
    inline void set(float fx, float fy)  { x = fx; y = fy; }
    inline void set(const Vector& v) { x = v.x; y = v.y; }
    inline void add(float fx, float fy)  { x += fx; y += fy; }
    inline void add(const Vector& v) { x += v.x; y += v.y; }
    
    inline float lengthSq() const {return x * x + y * y;}

    inline float length() {return lengthSq() > 0 ? sqrtf(lengthSq()) : 0;}
    
	//inline float length() { return (x * x + y * y) > 0 ? sqrtf(x * x + y * y) : 1; }

    inline Vector& reverse()
    {
        x *= -1;
        y *= -1;
        return *this;
    }
    
    inline Vector& limit(float max)
    {
        if (max < 1) max = 1;
        
        if (lengthSq() > max * max)
        {
            float len = length();
            x = (x * max)/len;
            y = (y * max)/len;
        }
        return *this;
    }

    inline Vector& normalize()
    {
        return limit(1);
    }
    
    inline Vector perp()
    {
        Vector v;
        v.set(-y, x);
        return v;
        //return Vector(-y, x);
    }

};

inline float operator|(const Vector &left, const Vector &right)
{
    return left.x * right.x + left.y * right.y;
}

inline Vector operator+(const Vector &left, const Vector &right)
{
    Vector v;
    v.set(left.x + right.x, left.y + right.y);
    return v;
    //return Vector(left.x + right.x, left.y + right.y);
}

inline Vector operator-(const Vector &left, const Vector &right)
{
    Vector v;
    v.set(left.x - right.x, left.y - right.y);
    return v;
    //return Vector(left.x - right.x, left.y - right.y);
}

inline Vector operator/(const Vector &left, const float right)
{
    Vector v;
    v.set(left.x / right, left.y / right);
    return v;
    //return Vector(left.x / right, left.y / right);
}

inline Vector operator*(const Vector &left, const float right)
{
    Vector v;
    v.set(left.x * right, left.y * right);
    return v;
    //return Vector(left.x * right, left.y * right);
}

inline Vector operator*(const float left, const Vector &right)
{
    Vector v;
    v.set(left * right.x, left * right.y);
    return v;
    //return Vector(left * right.x, left * right.y);
}

inline Vector operator>>(const Vector &left, const int right)
{
    Vector v;
    v.set((int)left.x >> right, (int)left.y >> right);
    return v;
    //return Vector((int)left.x >> right, (int)left.y >> right);
}

inline Vector operator<<(const Vector &left, const int right)
{
    Vector v;
    v.set((int)left.x << right, (int)left.y << right);
    return v;
    //return Vector((int)left.x << right, (int)left.y << right);
}

inline bool operator<(const Vector& left, const Vector& right)
{
    return left.x < right.x && left.y < right.y;
}

inline bool operator>(const Vector& left, const Vector& right)
{
    return left.x > right.x && left.y > right.y;
}

inline Vector getAngleVector(int angle)
{
    angle %= 360;
    if (angle < 0) angle += 360;
    
    //static float radPerDeg = 2.0 * M_PI / 360.0;
    
    return Vector(cos(float(angle) * RAD_PER_DEG), sin(float(angle) * RAD_PER_DEG));
}

inline int getVectorAngleDegrees(Vector v) // by value
{
    if (v.lengthSq() == 0) return 0;
    
    v.normalize();
    
    //static float degPerRad = 360 / (2.0 * M_PI);
    int angle = acos(v.x) * DEG_PER_RAD;
    
    //int angle = (acos(v.x) / (2 * M_PI)) * 360;    
    
    if (v.y < 0) angle = (360 - angle);
    
    return angle % 360;
}


#endif
