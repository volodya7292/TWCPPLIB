struct tet {
	float4 gov;
	float4 o;
};

RWTexture2D<float4> OutputTexture : register(u0);
RWStructuredBuffer<tet> buffer : register(u1);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	tet g = { float4(DTid.x / 1280.0, 0, 0, 1), float4(0, DTid.x / 1280.0, 0, 1) };
	buffer[DTid.x] = g;
	OutputTexture[DTid.xy] = float4(normalize(DTid.xy + DTid.yx), normalize(DTid.yx).x, 1);
}