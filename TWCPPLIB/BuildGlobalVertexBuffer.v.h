#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Note: shader requires additional functionality:
//       UAVs at every shader stage
//
//
// Buffer Definitions: 
//
// cbuffer input
// {
//
//   struct InputData
//   {
//       
//       uint vertex_offset;            // Offset:    0
//
//   } input;                           // Offset:    0 Size:     4
//
// }
//
// Resource bind info for gvb
// {
//
//   struct Vertex
//   {
//       
//       float3 pos;                    // Offset:    0
//       float2 tex_coord;              // Offset:   12
//       float3 normal;                 // Offset:   20
//
//   } $Element;                        // Offset:    0 Size:    32
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
// gvb                                   UAV  struct         r/w      U0             u0      1 
// input                             cbuffer      NA          NA     CB0            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyz         0     NONE   float   xyz 
// TEXCOORD                 0   xy          1     NONE   float   xy  
// NORMAL                   0   xyz         2     NONE   float   xyz 
// SV_VertexID              0   x           3   VERTID    uint   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// no Output
vs_5_1
dcl_globalFlags refactoringAllowed | allResourcesBound
dcl_constantbuffer CB0[0:0][1], immediateIndexed, space=0
dcl_uav_structured U0[0:0], 32, space=0
dcl_input v0.xyz
dcl_input v1.xy
dcl_input v2.xyz
dcl_input_sgv v3.x, vertex_id
dcl_temps 2
iadd r0.x, v3.x, CB0[0][0].x
mov r1.xyz, v0.xyzx
mov r1.w, v1.x
store_structured U0[0].xyzw, r0.x, l(0), r1.xyzw
mov r1.x, v1.y
mov r1.yzw, v2.xxyz
store_structured U0[0].xyzw, r0.x, l(16), r1.xyzw
ret 
// Approximately 8 instruction slots used
#endif

const BYTE BuildGlobalVertexBuffer_VertexByteCode[] =
{
     68,  88,  66,  67, 235, 200, 
    224,   8, 137,  32,  93,  15, 
     87, 132, 104,  11, 202,  38, 
    150,  20,   1,   0,   0,   0, 
     96,   5,   0,   0,   6,   0, 
      0,   0,  56,   0,   0,   0, 
    188,   2,   0,   0,  84,   3, 
      0,   0, 100,   3,   0,   0, 
    180,   4,   0,   0, 196,   4, 
      0,   0,  82,  68,  69,  70, 
    124,   2,   0,   0,   2,   0, 
      0,   0, 152,   0,   0,   0, 
      2,   0,   0,   0,  60,   0, 
      0,   0,   1,   5, 254, 255, 
      0,   1,  32,   0,  84,   2, 
      0,   0,  19,  19,  68,  37, 
     60,   0,   0,   0,  24,   0, 
      0,   0,  40,   0,   0,   0, 
     40,   0,   0,   0,  36,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0, 140,   0, 
      0,   0,   6,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 144,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    103, 118,  98,   0, 105, 110, 
    112, 117, 116,   0, 171, 171, 
    144,   0,   0,   0,   1,   0, 
      0,   0, 200,   0,   0,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    140,   0,   0,   0,   1,   0, 
      0,   0, 100,   1,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
    144,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0,  64,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  73, 110, 
    112, 117, 116,  68,  97, 116, 
     97,   0, 118, 101, 114, 116, 
    101, 120,  95, 111, 102, 102, 
    115, 101, 116,   0, 100, 119, 
    111, 114, 100,   0, 171, 171, 
      0,   0,  19,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   8,   1,   0,   0, 
    250,   0,   0,   0,  16,   1, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
      1,   0,   0,   0,   1,   0, 
     52,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 240,   0,   0,   0, 
    140,   1,   0,   0,   0,   0, 
      0,   0,  32,   0,   0,   0, 
      2,   0,   0,   0,  48,   2, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  36,  69, 
    108, 101, 109, 101, 110, 116, 
      0,  86, 101, 114, 116, 101, 
    120,   0, 112, 111, 115,   0, 
    102, 108, 111,  97, 116,  51, 
      0, 171,   1,   0,   3,   0, 
      1,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 160,   1, 
      0,   0, 116, 101, 120,  95, 
     99, 111, 111, 114, 100,   0, 
    102, 108, 111,  97, 116,  50, 
      0, 171, 171, 171,   1,   0, 
      3,   0,   1,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    214,   1,   0,   0, 110, 111, 
    114, 109,  97, 108,   0, 171, 
    156,   1,   0,   0, 168,   1, 
      0,   0,   0,   0,   0,   0, 
    204,   1,   0,   0, 224,   1, 
      0,   0,  12,   0,   0,   0, 
      4,   2,   0,   0, 168,   1, 
      0,   0,  20,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
      8,   0,   0,   0,   3,   0, 
     12,   2,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 149,   1,   0,   0, 
     77, 105,  99, 114, 111, 115, 
    111, 102, 116,  32,  40,  82, 
     41,  32,  72,  76,  83,  76, 
     32,  83, 104,  97, 100, 101, 
    114,  32,  67, 111, 109, 112, 
    105, 108, 101, 114,  32,  49, 
     48,  46,  49,   0,  73,  83, 
     71,  78, 144,   0,   0,   0, 
      4,   0,   0,   0,   8,   0, 
      0,   0, 104,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   7,   7, 
      0,   0, 113,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   3,   3, 
      0,   0, 122,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   7,   7, 
      0,   0, 129,   0,   0,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   1,   1, 
      0,   0,  80,  79,  83,  73, 
     84,  73,  79,  78,   0,  84, 
     69,  88,  67,  79,  79,  82, 
     68,   0,  78,  79,  82,  77, 
     65,  76,   0,  83,  86,  95, 
     86, 101, 114, 116, 101, 120, 
     73,  68,   0, 171, 171, 171, 
     79,  83,  71,  78,   8,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  83,  72, 
     69,  88,  72,   1,   0,   0, 
     81,   0,   1,   0,  82,   0, 
      0,   0, 106,   8,   8,   1, 
     89,   0,   0,   7,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0, 158,   0, 
      0,   7,  70, 238,  49,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,  95,   0,   0,   3, 
    114,  16,  16,   0,   0,   0, 
      0,   0,  95,   0,   0,   3, 
     50,  16,  16,   0,   1,   0, 
      0,   0,  95,   0,   0,   3, 
    114,  16,  16,   0,   2,   0, 
      0,   0,  96,   0,   0,   4, 
     18,  16,  16,   0,   3,   0, 
      0,   0,   6,   0,   0,   0, 
    104,   0,   0,   2,   2,   0, 
      0,   0,  30,   0,   0,   9, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,  16,  16,   0, 
      3,   0,   0,   0,  10, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,  18,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      1,   0,   0,   0,  10,  16, 
     16,   0,   1,   0,   0,   0, 
    168,   0,   0,  10, 242, 224, 
     33,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   5,  18,   0,  16,   0, 
      1,   0,   0,   0,  26,  16, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5, 226,   0, 
     16,   0,   1,   0,   0,   0, 
      6,  25,  16,   0,   2,   0, 
      0,   0, 168,   0,   0,  10, 
    242, 224,  33,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
     16,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     62,   0,   0,   1,  83,  70, 
     73,  48,   8,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  83,  84,  65,  84, 
    148,   0,   0,   0,   8,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0
};
