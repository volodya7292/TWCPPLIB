float4 main(uint vI : SV_VERTEXID) : SV_POSITION {
	float2 texcoord = float2(vI&1, vI>>1);
	return float4((texcoord.x-0.5f)*2, -(texcoord.y-0.5f)*2, 0, 1);
}