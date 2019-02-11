Texture2D<float4> t1 : register(t0);
Texture2D<float4> tex : register(t2);
sampler s1 : register(s0);
sampler texs : register(s1);

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float2 texCoord: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    // return interpolated color

    return lerp(t1.Sample(s1, input.texCoord), tex.Sample(texs, input.texCoord), 0.5);
}