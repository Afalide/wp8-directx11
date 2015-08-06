
#include "matrix_tools.hpp"

const float matrix_tools::pi =  0.0174532925f;

DirectX::XMFLOAT4X4 
matrix_tools::make_rotation_matrix(float rotation_x
                                 , float rotation_y
                                 , float rotation_z)
{
    
    float pitch  = degs_to_rads(rotation_x);
    float yaw    = degs_to_rads(rotation_y);
    float roll   = degs_to_rads(rotation_z);

    DirectX::XMMATRIX xm_rotation_matrix = DirectX::XMMatrixRotationRollPitchYaw(pitch
                                                                                , yaw
                                                                                , roll);
    DirectX::XMFLOAT4X4 ret;
    DirectX::XMStoreFloat4x4(&ret, xm_rotation_matrix);
    return ret;
}

float 
matrix_tools::degs_to_rads(float deg)
{
    const float conv = 0.0174532925f;
    return deg * conv;
}
