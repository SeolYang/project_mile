#include "Vector3.h"

namespace Mile
{
    Vector3 Vector3::Up = Vector3( 0.0f, 1.0f, 0.0f );
    Vector3 Vector3::Down = Vector3( 0.0f, -1.0f, 0.0f );
    Vector3 Vector3::Left = Vector3( -1.0f, 0.0f, 0.0f );
    Vector3 Vector3::Right = Vector3( 1.0f, 0.0f, 0.0f );
    Vector3 Vector3::Forward = Vector3( 0.0f, 0.0f, 1.0f );
    Vector3 Vector3::Backward = Vector3( 0.0f, 0.0f, -1.0f );
    Vector3 Vector3::One = Vector3( 1.0f, 1.0f, 1.0f );
    Vector3 Vector3::Zero = Vector3( 0.0f, 0.0f, 0.0f );
}