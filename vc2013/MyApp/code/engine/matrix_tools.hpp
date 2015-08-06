
#pragma once

#include <directxmath.h>

class matrix_tools
{

public:

    const static float pi;

    static DirectX::XMFLOAT4X4 make_rotation_matrix(float rotation_x
                                                  , float rotation_y
                                                  , float rotation_z);

    static float degs_to_rads(float deg);

};
