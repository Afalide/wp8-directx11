
//globals

cbuffer matrix_buffer
{
	matrix world_matrix;
    matrix view_matrix;
    matrix projection_matrix;
}

//type given to the vertex shader

struct vertex_input_type
{
    float4 position : POSITION;
    float4 color : COLOR;
};

//type given to the pixel shader

struct pixel_input_type
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

//process a vertex: outputs a value to give to the pixel shader

pixel_input_type color_vertex_shader(vertex_input_type input)
{
    pixel_input_type output;
    input.position.w = 1.0f;

    output.position = mul(input.position, world_matrix);
    output.position = mul(output.position, view_matrix);
    output.position = mul(output.position, projection_matrix);
    output.color = input.color;
    
    return output;
}
