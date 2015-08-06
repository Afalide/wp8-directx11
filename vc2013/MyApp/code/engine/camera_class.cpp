
#include <iostream>
#include "camera_class.hpp"

camera_class::camera_class()
    : _matrix_view()
    , _posx(0)
    , _posy(0)
    , _posz(0)
    , _rotx(0)
    , _roty(0)
    , _rotz(0)
{
}

camera_class::~camera_class()
{
}

void
camera_class::set_position(float x, float y, float z)
{
    _posx = x;
    _posy = y;
    _posz = z;
}

void
camera_class::set_rotation(float x, float y, float z)
{
    _rotx = x;
    _roty = y;
    _rotz = z;
}

DirectX::XMFLOAT3
camera_class::get_position()
{
    return DirectX::XMFLOAT3(_posx, _posy, _posz);
}

DirectX::XMFLOAT3
camera_class::get_rotation()
{
    return DirectX::XMFLOAT3(_rotx, _roty, _rotz);
}

DirectX::XMFLOAT4X4
camera_class::get_view_matrix()
{
    return _matrix_view;
}

void
camera_class::render()
{
    //D3DXVECTOR3 up, position, lookAt;
    //float yaw, pitch, roll;
    //D3DXMATRIX rotationMatrix;


    // Setup the vector that points upwards.
    DirectX::XMFLOAT3 up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    // Setup the position of the camera in the world.
    DirectX::XMFLOAT3 position;
    position.x = _posx;
    position.y = _posy;
    position.z = _posz;

    // Setup where the camera is looking by default.
    DirectX::XMFLOAT3 lookat;
    lookat.x = 0.0f;
    lookat.y = 0.0f;
    lookat.z = 1.0f;

    // Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
    const float pi =  0.0174532925f;

    float roll = _rotz * pi;
    float pitch = _rotx * pi;
    float yaw = _roty * pi;
    
    // Create the rotation matrix from the yaw, pitch, and roll values.
    DirectX::XMMATRIX rotation_matrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    // Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
    DirectX::XMVECTOR xm_lookat = DirectX::XMLoadFloat3(&lookat);
    DirectX::XMVECTOR xm_up = DirectX::XMLoadFloat3(&up);
    DirectX::XMVector3TransformCoord(xm_lookat, rotation_matrix);
    DirectX::XMVector3TransformCoord(xm_up, rotation_matrix);

    // Translate the rotated camera position to the location of the viewer.
    DirectX::XMVECTOR xm_position = DirectX::XMLoadFloat3(&position);
    xm_lookat = DirectX::XMVectorAdd(xm_position, xm_lookat);

    // Finally create the view matrix from the three updated vectors.
    DirectX::XMMATRIX xm_matrix_view = DirectX::XMMatrixLookAtLH(xm_position, xm_lookat, xm_up);
    DirectX::XMStoreFloat4x4(&_matrix_view, xm_matrix_view);
}
