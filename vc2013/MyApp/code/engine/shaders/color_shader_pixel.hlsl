
//type given to the pixel shader
//must have the same signature as in 
//the vertex shader

struct pixel_input_type
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

//process a pixel: set it's color

float4 color_pixel_shader(pixel_input_type input) : SV_TARGET
{
    //return float4(0.5, 0.6, 0.7, 1.0);
    return input.color;
}