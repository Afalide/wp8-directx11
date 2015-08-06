
//globals

cbuffer matrix_buffer
{
    //matrix world_matrix;
    //matrix view_matrix;
    //matrix projection_matrix;

    //can also specify the matrix size (float4x4 
    //instead of generic matrix type)
    //HLSL uses 16 bytes aligned structures
    //so C++ structs must also be 16 bytes aligned
    //(DirectX::XMMATRIX are already aligned like that)

    float4x4 world_matrix;
    float4x4 model_matrix;
    float4x4 view_matrix;
    float4x4 projection_matrix;
}

//type given to the vertex shader

struct vertex_input_type
{
    float4 position : POSITION;
    float2 uvcoord : TEXCOORD0;
};

//type given to the pixel shader

struct pixel_input_type
{
    float4 position : SV_POSITION;
    float2 uvcoord: TEXCOORD0;
};

//process a vertex: outputs a value to give to the pixel shader

pixel_input_type texture_vertex_shader(vertex_input_type input)
{
    pixel_input_type output;
    input.position.w = 1.0f;

    output.position = mul(input.position, world_matrix);
    output.position = mul(output.position, model_matrix);
    output.position = mul(output.position, view_matrix);
    output.position = mul(output.position, projection_matrix);
    output.uvcoord = input.uvcoord;

    return output;
}
