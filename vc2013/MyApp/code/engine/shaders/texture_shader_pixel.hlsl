
Texture2D some_texture;
SamplerState some_sampler;

struct pixel_input_type
{
    float4 position : SV_POSITION;
    float2 uvcoord: TEXCOORD0;
};

float4 texture_pixel_shader(pixel_input_type input) : SV_TARGET
{
    float4 color;
    color = some_texture.Sample(some_sampler, input.uvcoord);
    return color;
}
