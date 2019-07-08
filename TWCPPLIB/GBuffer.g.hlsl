struct VSOutput {
	//float4             clip_pos : SV_Position;
	//float4        prev_clip_pos : PrevPosition;
	linear float3     world_pos : WorldPosition;
	float2            tex_coord : TexCoord;
	uint            material_id : MaterialId;
	linear float3  world_normal : WorldNormal;
	linear float3       tangent : Tangent;
};

struct GSOutput {
	uint               rt_index : SV_RenderTargetArrayIndex;
	float4             clip_pos : SV_Position;
	//float4        prev_clip_pos : PrevPosition;
	linear float3     world_pos : WorldPosition;
	float2            tex_coord : TexCoord;
	uint            material_id : MaterialId;
	linear float3  world_normal : WorldNormal;
	linear float3       tangent : Tangent;
};

struct InputData {
	float4x4 proj_view[6];
	//float4x4 prev_proj_view[6];
};

ConstantBuffer<InputData> input_data : register(b0);

[maxvertexcount(18)]
void main(triangle VSOutput input[3], inout TriangleStream<GSOutput> output) {
	[unroll]
	for (uint t = 0; t < 6; t++) { // create 6 triangles
		GSOutput element;
		element.rt_index = t;

		[unroll]
		for (uint v = 0; v < 3; v++) {
			element.clip_pos = mul(input_data.proj_view[t], float4(input[v].world_pos, 1));
			//element.prev_clip_pos = mul(input_data.proj_view[t], float4(input[v].world_pos, 1));
			element.world_pos = input[v].world_pos;
			element.tex_coord = input[v].tex_coord;
			element.material_id = input[v].material_id;
			element.world_normal = input[v].world_normal;
			element.tangent = input[v].tangent;

			output.Append(element);
		}

		output.RestartStrip();
	}
}