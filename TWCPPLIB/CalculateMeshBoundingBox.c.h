#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
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
//       uint gvb_vertex_offset;        // Offset:    0
//       uint vertex_count;             // Offset:    4
//       uint iteration_count;          // Offset:    8
//       uint element_count;            // Offset:   12
//
//   } input;                           // Offset:    0 Size:    16
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
// Resource bind info for bounding_box
// {
//
//   struct Bounds
//   {
//       
//       float3 pMin;                   // Offset:    0
//       float3 pMax;                   // Offset:   12
//
//   } $Element;                        // Offset:    0 Size:    24
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
// gvb                               texture  struct         r/o      T0             t0      1 
// bounding_box                          UAV  struct         r/w      U0             u0      1 
// input                             cbuffer      NA          NA     CB0            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// no Input
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// no Output
cs_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[0:0][1], immediateIndexed, space=0
dcl_resource_structured T0[0:0], 32, space=0
dcl_uav_structured U0[0:0], 24, space=0
dcl_input vThreadID.x
dcl_temps 6
dcl_thread_group 1, 1, 1
ishl r0.x, vThreadID.x, l(4)
uge r0.y, r0.x, CB0[0][0].w
if_nz r0.y
  ret 
endif 
if_z CB0[0][0].z
  mov r1.xyz, l(0,0,0,0)
  mov r0.yzw, l(0,0,0,0)
  mov r2.x, l(0)
  loop 
    uge r2.y, r2.x, l(16)
    breakc_nz r2.y
    imad r2.y, vThreadID.x, l(48), CB0[0][0].x
    imad r2.y, r2.x, l(3), r2.y
    iadd r2.z, CB0[0][0].y, CB0[0][0].x
    uge r2.z, r2.y, r2.z
    if_nz r2.z
      break 
    endif 
    ld_structured r3.xyz, r2.y, l(0), T0[0].xyzx
    iadd r2.yz, r2.yyyy, l(0, 1, 2, 0)
    ld_structured r4.xyz, r2.y, l(0), T0[0].xyzx
    ld_structured r2.yzw, r2.z, l(0), T0[0].xxyz
    if_z r2.x
      min r5.xyz, r2.yzwy, r4.xyzx
      min r1.xyz, r3.xyzx, r5.xyzx
      max r5.xyz, r2.yzwy, r4.xyzx
      max r0.yzw, r3.yyzx, r5.yyzx
    else 
      min r5.xyz, r2.yzwy, r4.xyzx
      min r5.xyz, r3.xyzx, r5.xyzx
      min r1.xyz, r1.xyzx, r5.xyzx
      max r2.yzw, r2.yyzw, r4.xxyz
      max r2.yzw, r2.yyzw, r3.xxyz
      max r0.yzw, r0.yyzw, r2.zzwy
    endif 
    iadd r2.x, r2.x, l(1)
  endloop 
  mov r1.w, r0.w
else 
  imul null, r0.x, r0.x, CB0[0][0].z
  mov r1.xyz, l(0,0,0,0)
  mov r0.yzw, l(0,0,0,0)
  mov r2.x, l(0)
  loop 
    uge r2.y, r2.x, l(16)
    breakc_nz r2.y
    imad r2.y, r0.x, l(3), r2.x
    ld_structured r3.xyzw, r2.y, l(0), U0[0].xyzw
    ld_structured r4.xy, r2.y, l(16), U0[0].xyxx
    if_z r2.x
      mov r1.xyz, r3.xyzx
      mov r4.w, r3.w
      mov r0.yzw, r4.xxyw
    else 
      min r1.xyz, r1.xyzx, r3.xyzx
      mov r4.z, r3.w
      max r0.yzw, r0.yyzw, r4.xxyz
    endif 
    iadd r2.x, r2.x, l(1)
  endloop 
  mov r1.w, r0.w
endif 
store_structured U0[0].xyzw, vThreadID.x, l(0), r1.xyzw
store_structured U0[0].xy, vThreadID.x, l(16), r0.yzyy
ret 
// Approximately 66 instruction slots used
#endif

const BYTE CalculateMeshBoundingBox_ByteCode[] =
{
     68,  88,  66,  67, 168,  50, 
    216, 172, 231, 197, 201,  55, 
    246, 174,  89,  81,  64,  71, 
    109,   4,   1,   0,   0,   0, 
    224,  10,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    208,   3,   0,   0, 224,   3, 
      0,   0, 240,   3,   0,   0, 
     68,  10,   0,   0,  82,  68, 
     69,  70, 148,   3,   0,   0, 
      3,   0,   0,   0, 204,   0, 
      0,   0,   3,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
     83,  67,   0,   5,   0,   0, 
    108,   3,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    180,   0,   0,   0,   5,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 184,   0, 
      0,   0,   6,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,  24,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 197,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    103, 118,  98,   0,  98, 111, 
    117, 110, 100, 105, 110, 103, 
     95,  98, 111, 120,   0, 105, 
    110, 112, 117, 116,   0, 171, 
    197,   0,   0,   0,   1,   0, 
      0,   0,  20,   1,   0,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    180,   0,   0,   0,   1,   0, 
      0,   0,   4,   2,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
    184,   0,   0,   0,   1,   0, 
      0,   0, 244,   2,   0,   0, 
     24,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
    197,   0,   0,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0, 224,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  73, 110, 
    112, 117, 116,  68,  97, 116, 
     97,   0, 103, 118,  98,  95, 
    118, 101, 114, 116, 101, 120, 
     95, 111, 102, 102, 115, 101, 
    116,   0, 100, 119, 111, 114, 
    100,   0, 171, 171,   0,   0, 
     19,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     88,   1,   0,   0, 118, 101, 
    114, 116, 101, 120,  95,  99, 
    111, 117, 110, 116,   0, 105, 
    116, 101, 114,  97, 116, 105, 
    111, 110,  95,  99, 111, 117, 
    110, 116,   0, 101, 108, 101, 
    109, 101, 110, 116,  95,  99, 
    111, 117, 110, 116,   0, 171, 
     70,   1,   0,   0,  96,   1, 
      0,   0,   0,   0,   0,   0, 
    132,   1,   0,   0,  96,   1, 
      0,   0,   4,   0,   0,   0, 
    145,   1,   0,   0,  96,   1, 
      0,   0,   8,   0,   0,   0, 
    161,   1,   0,   0,  96,   1, 
      0,   0,  12,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
      4,   0,   0,   0,   4,   0, 
    176,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  60,   1,   0,   0, 
     44,   2,   0,   0,   0,   0, 
      0,   0,  32,   0,   0,   0, 
      2,   0,   0,   0, 208,   2, 
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
      0,   0,   0,   0,  64,   2, 
      0,   0, 116, 101, 120,  95, 
     99, 111, 111, 114, 100,   0, 
    102, 108, 111,  97, 116,  50, 
      0, 171, 171, 171,   1,   0, 
      3,   0,   1,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    118,   2,   0,   0, 110, 111, 
    114, 109,  97, 108,   0, 171, 
     60,   2,   0,   0,  72,   2, 
      0,   0,   0,   0,   0,   0, 
    108,   2,   0,   0, 128,   2, 
      0,   0,  12,   0,   0,   0, 
    164,   2,   0,   0,  72,   2, 
      0,   0,  20,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
      8,   0,   0,   0,   3,   0, 
    172,   2,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  53,   2,   0,   0, 
     44,   2,   0,   0,   0,   0, 
      0,   0,  24,   0,   0,   0, 
      2,   0,   0,   0,  72,   3, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  66, 111, 
    117, 110, 100, 115,   0, 112, 
     77, 105, 110,   0, 112,  77, 
     97, 120,   0, 171, 171, 171, 
     35,   3,   0,   0,  72,   2, 
      0,   0,   0,   0,   0,   0, 
     40,   3,   0,   0,  72,   2, 
      0,   0,  12,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
      6,   0,   0,   0,   2,   0, 
     48,   3,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  28,   3,   0,   0, 
     77, 105,  99, 114, 111, 115, 
    111, 102, 116,  32,  40,  82, 
     41,  32,  72,  76,  83,  76, 
     32,  83, 104,  97, 100, 101, 
    114,  32,  67, 111, 109, 112, 
    105, 108, 101, 114,  32,  49, 
     48,  46,  49,   0,  73,  83, 
     71,  78,   8,   0,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  79,  83,  71,  78, 
      8,   0,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     83,  72,  69,  88,  76,   6, 
      0,   0,  81,   0,   5,   0, 
    147,   1,   0,   0, 106,   8, 
      0,   1,  89,   0,   0,   7, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
    162,   0,   0,   7,  70, 126, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0, 158,   0, 
      0,   7,  70, 238,  49,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     24,   0,   0,   0,   0,   0, 
      0,   0,  95,   0,   0,   2, 
     18,   0,   2,   0, 104,   0, 
      0,   2,   6,   0,   0,   0, 
    155,   0,   0,   4,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  41,   0, 
      0,   6,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
      2,   0,   1,  64,   0,   0, 
      4,   0,   0,   0,  80,   0, 
      0,   9,  34,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     58, 128,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  31,   0, 
      4,   3,  26,   0,  16,   0, 
      0,   0,   0,   0,  62,   0, 
      0,   1,  21,   0,   0,   1, 
     31,   0,   0,   5,  42, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   8, 
    114,   0,  16,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   8, 226,   0,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   5,  18,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  48,   0,   0,   1, 
     80,   0,   0,   7,  34,   0, 
     16,   0,   2,   0,   0,   0, 
     10,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
     16,   0,   0,   0,   3,   0, 
      4,   3,  26,   0,  16,   0, 
      2,   0,   0,   0,  35,   0, 
      0,  10,  34,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
      2,   0,   1,  64,   0,   0, 
     48,   0,   0,   0,  10, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  35,   0,   0,   9, 
     34,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   2,   0, 
      0,   0,  30,   0,   0,  11, 
     66,   0,  16,   0,   2,   0, 
      0,   0,  26, 128,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     10, 128,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  80,   0, 
      0,   7,  66,   0,  16,   0, 
      2,   0,   0,   0,  26,   0, 
     16,   0,   2,   0,   0,   0, 
     42,   0,  16,   0,   2,   0, 
      0,   0,  31,   0,   4,   3, 
     42,   0,  16,   0,   2,   0, 
      0,   0,   2,   0,   0,   1, 
     21,   0,   0,   1, 167,   0, 
      0,  10, 114,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  70, 114,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  30,   0,   0,  10, 
     98,   0,  16,   0,   2,   0, 
      0,   0,  86,   5,  16,   0, 
      2,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
    167,   0,   0,  10, 114,   0, 
     16,   0,   4,   0,   0,   0, 
     26,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  70, 114, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 167,   0, 
      0,  10, 226,   0,  16,   0, 
      2,   0,   0,   0,  42,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,   6, 121,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  31,   0,   0,   3, 
     10,   0,  16,   0,   2,   0, 
      0,   0,  51,   0,   0,   7, 
    114,   0,  16,   0,   5,   0, 
      0,   0, 150,   7,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   4,   0,   0,   0, 
     51,   0,   0,   7, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      5,   0,   0,   0,  52,   0, 
      0,   7, 114,   0,  16,   0, 
      5,   0,   0,   0, 150,   7, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  52,   0,   0,   7, 
    226,   0,  16,   0,   0,   0, 
      0,   0,  86,   2,  16,   0, 
      3,   0,   0,   0,  86,   2, 
     16,   0,   5,   0,   0,   0, 
     18,   0,   0,   1,  51,   0, 
      0,   7, 114,   0,  16,   0, 
      5,   0,   0,   0, 150,   7, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  51,   0,   0,   7, 
    114,   0,  16,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   5,   0,   0,   0, 
     51,   0,   0,   7, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      5,   0,   0,   0,  52,   0, 
      0,   7, 226,   0,  16,   0, 
      2,   0,   0,   0,  86,  14, 
     16,   0,   2,   0,   0,   0, 
      6,   9,  16,   0,   4,   0, 
      0,   0,  52,   0,   0,   7, 
    226,   0,  16,   0,   2,   0, 
      0,   0,  86,  14,  16,   0, 
      2,   0,   0,   0,   6,   9, 
     16,   0,   3,   0,   0,   0, 
     52,   0,   0,   7, 226,   0, 
     16,   0,   0,   0,   0,   0, 
     86,  14,  16,   0,   0,   0, 
      0,   0, 166,   7,  16,   0, 
      2,   0,   0,   0,  21,   0, 
      0,   1,  30,   0,   0,   7, 
     18,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   1,   0,   0,   0, 
     22,   0,   0,   1,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     18,   0,   0,   1,  38,   0, 
      0,  10,   0, 208,   0,   0, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  42, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   8, 
    114,   0,  16,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   8, 226,   0,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   5,  18,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  48,   0,   0,   1, 
     80,   0,   0,   7,  34,   0, 
     16,   0,   2,   0,   0,   0, 
     10,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
     16,   0,   0,   0,   3,   0, 
      4,   3,  26,   0,  16,   0, 
      2,   0,   0,   0,  35,   0, 
      0,   9,  34,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      2,   0,   0,   0, 167,   0, 
      0,  10, 242,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  70, 238,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 167,   0,   0,  10, 
     50,   0,  16,   0,   4,   0, 
      0,   0,  26,   0,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,  16,   0,   0,   0, 
     70, 224,  33,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     31,   0,   0,   3,  10,   0, 
     16,   0,   2,   0,   0,   0, 
     54,   0,   0,   5, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  54,   0,   0,   5, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  54,   0, 
      0,   5, 226,   0,  16,   0, 
      0,   0,   0,   0,   6,  13, 
     16,   0,   4,   0,   0,   0, 
     18,   0,   0,   1,  51,   0, 
      0,   7, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  52,   0, 
      0,   7, 226,   0,  16,   0, 
      0,   0,   0,   0,  86,  14, 
     16,   0,   0,   0,   0,   0, 
      6,   9,  16,   0,   4,   0, 
      0,   0,  21,   0,   0,   1, 
     30,   0,   0,   7,  18,   0, 
     16,   0,   2,   0,   0,   0, 
     10,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      1,   0,   0,   0,  22,   0, 
      0,   1,  54,   0,   0,   5, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  21,   0, 
      0,   1, 168,   0,   0,   9, 
    242, 224,  33,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     10,   0,   2,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0, 168,   0,   0,   9, 
     50, 224,  33,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     10,   0,   2,   0,   1,  64, 
      0,   0,  16,   0,   0,   0, 
    150,   5,  16,   0,   0,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 148,   0, 
      0,   0,  66,   0,   0,   0, 
      6,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     12,   0,   0,   0,   9,   0, 
      0,   0,   4,   0,   0,   0, 
      7,   0,   0,   0,   6,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     12,   0,   0,   0,   0,   0, 
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
      0,   0,   2,   0,   0,   0
};
