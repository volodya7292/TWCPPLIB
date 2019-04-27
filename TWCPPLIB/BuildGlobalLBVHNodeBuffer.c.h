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
//       uint node_offset;              // Offset:    0
//
//   } input;                           // Offset:    0 Size:     4
//
// }
//
// Resource bind info for lbvh
// {
//
//   struct LBVHNode
//   {
//       
//       struct Bounds
//       {
//           
//           float3 pMin;               // Offset:    0
//           float3 pMax;               // Offset:   12
//
//       } bounds;                      // Offset:    0
//       uint primitive_index;          // Offset:   24
//       uint parent;                   // Offset:   28
//       uint left_child;               // Offset:   32
//       uint right_child;              // Offset:   36
//
//   } $Element;                        // Offset:    0 Size:    40
//
// }
//
// Resource bind info for glbvh_nodes
// {
//
//   struct LBVHNode
//   {
//       
//       struct Bounds
//       {
//           
//           float3 pMin;               // Offset:    0
//           float3 pMax;               // Offset:   12
//
//       } bounds;                      // Offset:    0
//       uint primitive_index;          // Offset:   24
//       uint parent;                   // Offset:   28
//       uint left_child;               // Offset:   32
//       uint right_child;              // Offset:   36
//
//   } $Element;                        // Offset:    0 Size:    40
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
// lbvh                              texture  struct         r/o      T0             t0      1 
// glbvh_nodes                           UAV  struct         r/w      U0             u0      1 
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
dcl_globalFlags refactoringAllowed | allResourcesBound
dcl_constantbuffer CB0[0:0][1], immediateIndexed, space=0
dcl_resource_structured T0[0:0], 40, space=0
dcl_uav_structured U0[0:0], 40, space=0
dcl_input vThreadID.x
dcl_temps 3
dcl_thread_group 1, 1, 1
iadd r0.x, vThreadID.x, CB0[0][0].x
ld_structured r1.xyzw, vThreadID.x, l(0), T0[0].xyzw
ld_structured r2.xyzw, vThreadID.x, l(16), T0[0].xyzw
ld_structured r0.yz, vThreadID.x, l(32), T0[0].xxyx
store_structured U0[0].xyzw, r0.x, l(0), r1.xyzw
store_structured U0[0].xyzw, r0.x, l(16), r2.xyzw
store_structured U0[0].xy, r0.x, l(32), r0.yzyy
ret 
// Approximately 8 instruction slots used
#endif

const BYTE BuildGlobalLBVHNodeBuffer_ByteCode[] =
{
     68,  88,  66,  67, 193, 220, 
    235, 120, 125, 105, 197, 140, 
    204, 122, 255, 208, 153, 170, 
     36, 116,   1,   0,   0,   0, 
    244,   5,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    168,   3,   0,   0, 184,   3, 
      0,   0, 200,   3,   0,   0, 
     88,   5,   0,   0,  82,  68, 
     69,  70, 108,   3,   0,   0, 
      3,   0,   0,   0, 204,   0, 
      0,   0,   3,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
     83,  67,   0,   1,  32,   0, 
     68,   3,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    180,   0,   0,   0,   5,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   0,   0,   0,  40,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 185,   0, 
      0,   0,   6,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,  40,   0,   0,   0, 
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
    108,  98, 118, 104,   0, 103, 
    108,  98, 118, 104,  95, 110, 
    111, 100, 101, 115,   0, 105, 
    110, 112, 117, 116,   0, 171, 
    197,   0,   0,   0,   1,   0, 
      0,   0,  20,   1,   0,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    180,   0,   0,   0,   1,   0, 
      0,   0, 172,   1,   0,   0, 
     40,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
    185,   0,   0,   0,   1,   0, 
      0,   0,  28,   3,   0,   0, 
     40,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
    197,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0, 136,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  73, 110, 
    112, 117, 116,  68,  97, 116, 
     97,   0, 110, 111, 100, 101, 
     95, 111, 102, 102, 115, 101, 
    116,   0, 100, 119, 111, 114, 
    100,   0,   0,   0,  19,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  82,   1, 
      0,   0,  70,   1,   0,   0, 
     88,   1,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
      1,   0,   1,   0,   0,   0, 
      1,   0, 124,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  60,   1, 
      0,   0, 212,   1,   0,   0, 
      0,   0,   0,   0,  40,   0, 
      0,   0,   2,   0,   0,   0, 
    248,   2,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     36,  69, 108, 101, 109, 101, 
    110, 116,   0,  76,  66,  86, 
     72,  78, 111, 100, 101,   0, 
     98, 111, 117, 110, 100, 115, 
      0,  66, 111, 117, 110, 100, 
    115,   0, 112,  77, 105, 110, 
      0, 102, 108, 111,  97, 116, 
     51,   0,   1,   0,   3,   0, 
      1,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 249,   1, 
      0,   0, 112,  77,  97, 120, 
      0, 171, 171, 171, 244,   1, 
      0,   0,   0,   2,   0,   0, 
      0,   0,   0,   0,  36,   2, 
      0,   0,   0,   2,   0,   0, 
     12,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,   6,   0, 
      0,   0,   2,   0,  44,   2, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    237,   1,   0,   0, 112, 114, 
    105, 109, 105, 116, 105, 118, 
    101,  95, 105, 110, 100, 101, 
    120,   0,   0,   0,  19,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  82,   1, 
      0,   0, 112,  97, 114, 101, 
    110, 116,   0, 108, 101, 102, 
    116,  95,  99, 104, 105, 108, 
    100,   0, 114, 105, 103, 104, 
    116,  95,  99, 104, 105, 108, 
    100,   0, 171, 171, 230,   1, 
      0,   0,  68,   2,   0,   0, 
      0,   0,   0,   0, 104,   2, 
      0,   0, 120,   2,   0,   0, 
     24,   0,   0,   0, 156,   2, 
      0,   0, 120,   2,   0,   0, 
     28,   0,   0,   0, 163,   2, 
      0,   0, 120,   2,   0,   0, 
     32,   0,   0,   0, 174,   2, 
      0,   0, 120,   2,   0,   0, 
     36,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,  10,   0, 
      0,   0,   5,   0, 188,   2, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    221,   1,   0,   0, 212,   1, 
      0,   0,   0,   0,   0,   0, 
     40,   0,   0,   0,   2,   0, 
      0,   0, 248,   2,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  49,  48,  46,  49,   0, 
     73,  83,  71,  78,   8,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  79,  83, 
     71,  78,   8,   0,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  83,  72,  69,  88, 
    136,   1,   0,   0,  81,   0, 
      5,   0,  98,   0,   0,   0, 
    106,   8,   8,   1,  89,   0, 
      0,   7,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0, 162,   0,   0,   7, 
     70, 126,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  40,   0, 
      0,   0,   0,   0,   0,   0, 
    158,   0,   0,   7,  70, 238, 
     49,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  40,   0,   0,   0, 
      0,   0,   0,   0,  95,   0, 
      0,   2,  18,   0,   2,   0, 
    104,   0,   0,   2,   3,   0, 
      0,   0, 155,   0,   0,   4, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     30,   0,   0,   8,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,   2,   0,  10, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 167,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,   2,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  70, 126,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 167,   0,   0,   9, 
    242,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,   2,   0, 
      1,  64,   0,   0,  16,   0, 
      0,   0,  70, 126,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 167,   0,   0,   9, 
     98,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,   2,   0, 
      1,  64,   0,   0,  32,   0, 
      0,   0,   6, 113,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 168,   0,   0,  10, 
    242, 224,  33,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
    168,   0,   0,  10, 242, 224, 
     33,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  16,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0, 168,   0, 
      0,  10,  50, 224,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  32,   0,   0,   0, 
    150,   5,  16,   0,   0,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 148,   0, 
      0,   0,   8,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
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
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0
};
