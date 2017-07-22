#include "Vector4.h"

namespace Mile
{
    Vector4 Vector4::Up = Vector4( 0.0f, 1.0f, 0.0f, 1.0f );
    Vector4 Vector4::Down = Vector4( 0.0f, -1.0f, 0.0f, 1.0f );
    Vector4 Vector4::Left = Vector4( -1.0f, 0.0f, 0.0f, 1.0f );
    Vector4 Vector4::Right = Vector4( 1.0f, 0.0f, 0.0f, 1.0f );
    Vector4 Vector4::Forward = Vector4( 0.0f, 0.0f, 1.0f, 1.0f );
    Vector4 Vector4::Backward = Vector4( 0.0f, 0.0f, -1.0f, 1.0f );
    Vector4 Vector4::One = Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
    Vector4 Vector4::Zero = Vector4( 0.0f, 0.0f, 0.0f, 1.0f );
}