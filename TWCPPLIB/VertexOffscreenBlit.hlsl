struct VS_OUTPUT {
	float4 pos: SV_POSITION;
	float2 texCoord: TEXCOORD;
};

VS_OUTPUT main(uint vI : SV_VERTEXID) {
	float2 texcoord = float2(vI&1, vI>>1);

	VS_OUTPUT output;
	output.texCoord = texcoord;
	output.pos = float4((texcoord.x-0.5f)*2, -(texcoord.y-0.5f)*2, 0, 1);
	return output;
}