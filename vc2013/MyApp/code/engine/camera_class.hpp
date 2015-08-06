
#pragma once

#include <DirectXMath.h>

class camera_class
{
public:
    camera_class();
    ~camera_class();

    void render();
    void set_position(float, float, float);
    void set_rotation(float, float, float);

    DirectX::XMFLOAT3 get_position();
    DirectX::XMFLOAT3 get_rotation();
    DirectX::XMFLOAT4X4 get_view_matrix();
   
private:

    float _posx, _posy, _posz, _rotx, _roty, _rotz;
    DirectX::XMFLOAT4X4 _matrix_view;
};

