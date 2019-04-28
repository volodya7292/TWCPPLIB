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
//       uint gnb_offsets_count;        // Offset:    0
//       uint leaves_offset;            // Offset:    4
//
//   } input;                           // Offset:    0 Size:     8
//
// }
//
// Resource bind info for gnb
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
//       uint element_index;            // Offset:   24
//       uint parent;                   // Offset:   28
//       uint left_child;               // Offset:   32
//       uint right_child;              // Offset:   36
//
//   } $Element;                        // Offset:    0 Size:    40
//
// }
//
// Resource bind info for gnb_offsets
// {
//
//   uint $Element;                     // Offset:    0 Size:     4
//
// }
//
// Resource bind info for morton_code_indices
// {
//
//   uint $Element;                     // Offset:    0 Size:     4
//
// }
//
// Resource bind info for nodes
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
//       uint element_index;            // Offset:   24
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
// gnb                               texture  struct         r/o      T0             t0      1 
// gnb_offsets                       texture  struct         r/o      T1             t1      1 
// morton_code_indices               texture  struct         r/o      T2             t2      1 
// nodes                                 UAV  struct         r/w      U0             u0      1 
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
dcl_resource_structured T1[1:1], 4, space=0
dcl_resource_structured T2[2:2], 4, space=0
dcl_uav_structured U0[0:0], 40, space=0
dcl_input vThreadID.x
dcl_temps 3
dcl_thread_group 1, 1, 1
uge r0.x, vThreadID.x, CB0[0][0].y
if_nz r0.x
  ld_structured r0.x, vThreadID.x, l(0), T1[1].xxxx
  iadd r0.y, vThreadID.x, -CB0[0][0].y
  ld_structured r1.z, r0.y, l(0), T2[2].xxxx
  ld_structured r2.xyzw, r0.x, l(0), T0[0].xyzw
  ld_structured r1.xy, r0.x, l(16), T0[0].xyxx
else 
  mov r2.xyzw, l(-340282346638528859811704183484516925440.000000,-340282346638528859811704183484516925440.000000,-340282346638528859811704183484516925440.000000,340282346638528859811704183484516925440.000000)
  mov r1.xyz, l(340282346638528859811704183484516925440.000000,340282346638528859811704183484516925440.000000,-1,0)
endif 
store_structured U0[0].xyzw, vThreadID.x, l(0), r2.xyzw
mov r1.w, l(-1)
store_structured U0[0].xyzw, vThreadID.x, l(16), r1.xyzw
store_structured U0[0].xy, vThreadID.x, l(32), l(-1,-1,0,0)
ret 
// Approximately 16 instruction slots used
#endif

const BYTE SetupLBVHNodesFromLBVHs_ByteCode[] =
{
     68,  88,  66,  67, 195, 174, 
     14,  31, 252,  62, 211,  55, 
    208, 123, 177,  33,  17, 249, 
     51, 200,   1,   0,   0,   0, 
    244,   7,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    180,   4,   0,   0, 196,   4, 
      0,   0, 212,   4,   0,   0, 
     88,   7,   0,   0,  82,  68, 
     69,  70, 120,   4,   0,   0, 
      5,   0,   0,   0,  52,   1, 
      0,   0,   5,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
     83,  67,   0,   1,  32,   0, 
     80,   4,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   1,   0,   0,   5,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   0,   0,   0,  40,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   8,   1, 
      0,   0,   5,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,   4,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  20,   1,   0,   0, 
      5,   0,   0,   0,   6,   0, 
      0,   0,   1,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     40,   1,   0,   0,   6,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   0,   0,   0,  40,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  46,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 103, 110,  98,   0, 
    103, 110,  98,  95, 111, 102, 
    102, 115, 101, 116, 115,   0, 
    109, 111, 114, 116, 111, 110, 
     95,  99, 111, 100, 101,  95, 
    105, 110, 100, 105,  99, 101, 
    115,   0, 110, 111, 100, 101, 
    115,   0, 105, 110, 112, 117, 
    116,   0,  46,   1,   0,   0, 
      1,   0,   0,   0, 172,   1, 
      0,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   4,   1,   0,   0, 
      1,   0,   0,   0, 104,   2, 
      0,   0,  40,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   8,   1,   0,   0, 
      1,   0,   0,   0, 216,   3, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  20,   1,   0,   0, 
      1,   0,   0,   0,   0,   4, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  40,   1,   0,   0, 
      1,   0,   0,   0,  40,   4, 
      0,   0,  40,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  46,   1,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,   2,   0,   0,   0, 
     68,   2,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     73, 110, 112, 117, 116,  68, 
     97, 116,  97,   0, 103, 110, 
     98,  95, 111, 102, 102, 115, 
    101, 116, 115,  95,  99, 111, 
    117, 110, 116,   0, 100, 119, 
    111, 114, 100,   0, 171, 171, 
      0,   0,  19,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 240,   1,   0,   0, 
    108, 101,  97, 118, 101, 115, 
     95, 111, 102, 102, 115, 101, 
    116,   0, 171, 171, 222,   1, 
      0,   0, 248,   1,   0,   0, 
      0,   0,   0,   0,  28,   2, 
      0,   0, 248,   1,   0,   0, 
      4,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,   2,   0, 
      0,   0,   2,   0,  44,   2, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    212,   1,   0,   0, 144,   2, 
      0,   0,   0,   0,   0,   0, 
     40,   0,   0,   0,   2,   0, 
      0,   0, 180,   3,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  36,  69, 108, 101, 
    109, 101, 110, 116,   0,  76, 
     66,  86,  72,  78, 111, 100, 
    101,   0,  98, 111, 117, 110, 
    100, 115,   0,  66, 111, 117, 
    110, 100, 115,   0, 112,  77, 
    105, 110,   0, 102, 108, 111, 
     97, 116,  51,   0,   1,   0, 
      3,   0,   1,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    181,   2,   0,   0, 112,  77, 
     97, 120,   0, 171, 171, 171, 
    176,   2,   0,   0, 188,   2, 
      0,   0,   0,   0,   0,   0, 
    224,   2,   0,   0, 188,   2, 
      0,   0,  12,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
      6,   0,   0,   0,   2,   0, 
    232,   2,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 169,   2,   0,   0, 
    101, 108, 101, 109, 101, 110, 
    116,  95, 105, 110, 100, 101, 
    120,   0, 171, 171,   0,   0, 
     19,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    240,   1,   0,   0, 112,  97, 
    114, 101, 110, 116,   0, 108, 
    101, 102, 116,  95,  99, 104, 
    105, 108, 100,   0, 114, 105, 
    103, 104, 116,  95,  99, 104, 
    105, 108, 100,   0, 171, 171, 
    162,   2,   0,   0,   0,   3, 
      0,   0,   0,   0,   0,   0, 
     36,   3,   0,   0,  52,   3, 
      0,   0,  24,   0,   0,   0, 
     88,   3,   0,   0,  52,   3, 
      0,   0,  28,   0,   0,   0, 
     95,   3,   0,   0,  52,   3, 
      0,   0,  32,   0,   0,   0, 
    106,   3,   0,   0,  52,   3, 
      0,   0,  36,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
     10,   0,   0,   0,   5,   0, 
    120,   3,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 153,   2,   0,   0, 
    144,   2,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0,  52,   3, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 144,   2, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  52,   3,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 144,   2,   0,   0, 
      0,   0,   0,   0,  40,   0, 
      0,   0,   2,   0,   0,   0, 
    180,   3,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
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
     83,  72,  69,  88, 124,   2, 
      0,   0,  81,   0,   5,   0, 
    159,   0,   0,   0, 106,   8, 
      8,   1,  89,   0,   0,   7, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
    162,   0,   0,   7,  70, 126, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  40,   0,   0,   0, 
      0,   0,   0,   0, 162,   0, 
      0,   7,  70, 126,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 162,   0,   0,   7, 
     70, 126,  48,   0,   2,   0, 
      0,   0,   2,   0,   0,   0, 
      2,   0,   0,   0,   4,   0, 
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
     80,   0,   0,   8,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,   2,   0,  26, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  31,   0,   4,   3, 
     10,   0,  16,   0,   0,   0, 
      0,   0, 167,   0,   0,   9, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,   2,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,   6, 112,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  30,   0,   0,   9, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,   2,   0, 
     26, 128,  48, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 167,   0,   0,  10, 
     66,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
      6, 112,  32,   0,   2,   0, 
      0,   0,   2,   0,   0,   0, 
    167,   0,   0,  10, 242,   0, 
     16,   0,   2,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  70, 126, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 167,   0, 
      0,  10,  50,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  16,   0, 
      0,   0,  70, 112,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  18,   0,   0,   1, 
     54,   0,   0,   8, 242,   0, 
     16,   0,   2,   0,   0,   0, 
      2,  64,   0,   0, 255, 255, 
    127, 255, 255, 255, 127, 255, 
    255, 255, 127, 255, 255, 255, 
    127, 127,  54,   0,   0,   8, 
    114,   0,  16,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
    255, 255, 127, 127, 255, 255, 
    127, 127, 255, 255, 255, 255, 
      0,   0,   0,   0,  21,   0, 
      0,   1, 168,   0,   0,   9, 
    242, 224,  33,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     10,   0,   2,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
    130,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
    255, 255, 255, 255, 168,   0, 
      0,   9, 242, 224,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  10,   0,   2,   0, 
      1,  64,   0,   0,  16,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0, 168,   0, 
      0,  12,  50, 224,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  10,   0,   2,   0, 
      1,  64,   0,   0,  32,   0, 
      0,   0,   2,  64,   0,   0, 
    255, 255, 255, 255, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      0,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    148,   0,   0,   0,  16,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   2,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
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
      0,   0,   0,   0,   3,   0, 
      0,   0
};