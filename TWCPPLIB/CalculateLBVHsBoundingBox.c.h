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
//       uint gnb_offset_count;         // Offset:    0
//       uint iteration_count;          // Offset:    4
//       uint element_count;            // Offset:    8
//
//   } input;                           // Offset:    0 Size:    12
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
//           float4 pMin;               // Offset:    0
//           float4 pMax;               // Offset:   16
//
//       } bounds;                      // Offset:    0
//       uint element_index;            // Offset:   32
//       uint parent;                   // Offset:   36
//       uint left_child;               // Offset:   40
//       uint right_child;              // Offset:   44
//
//   } $Element;                        // Offset:    0 Size:    48
//
// }
//
// Resource bind info for gnb_offsets
// {
//
//   struct SceneLBVHInstance
//   {
//       
//       uint vertex_offset;            // Offset:    0
//       uint node_offset;              // Offset:    4
//       float4x4 transform;            // Offset:    8
//       float4x4 transform_inverse;    // Offset:   72
//
//   } $Element;                        // Offset:    0 Size:   136
//
// }
//
// Resource bind info for bounding_box
// {
//
//   struct Bounds
//   {
//       
//       float4 pMin;                   // Offset:    0
//       float4 pMax;                   // Offset:   16
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
// gnb                               texture  struct         r/o      T0             t0      1 
// gnb_offsets                       texture  struct         r/o      T1             t1      1 
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
dcl_globalFlags refactoringAllowed | allResourcesBound
dcl_constantbuffer CB0[0:0][1], immediateIndexed, space=0
dcl_resource_structured T0[0:0], 48, space=0
dcl_resource_structured T1[1:1], 136, space=0
dcl_uav_structured U0[0:0], 32, space=0
dcl_input vThreadID.x
dcl_temps 13
dcl_thread_group 64, 1, 1
if_z CB0[0][0].y
  ishl r0.x, vThreadID.x, l(4)
  mov r1.w, l(1.000000)
  mov r2.xyz, l(0,0,0,0)
  mov r3.xyz, l(0,0,0,0)
  mov r0.y, CB0[0][0].x
  mov r0.z, l(0)
  loop 
    uge r0.w, r0.z, l(16)
    breakc_nz r0.w
    iadd r0.w, r0.z, r0.x
    uge r4.x, r0.w, r0.y
    if_nz r4.x
      break 
    endif 
    ld_structured r4.xyzw, r0.w, l(4), T1[1].xyzw
    ld_structured r5.xyz, r0.w, l(24), T1[1].xyzx
    ld_structured r6.xyz, r0.w, l(40), T1[1].xyzx
    ld_structured r7.xyw, r0.w, l(56), T1[1].xyxz
    ld_structured r1.xyz, r4.x, l(0), T0[0].xyzx
    ld_structured r8.xyz, r4.x, l(16), T0[0].xyzx
    mov r9.x, r4.y
    mov r9.y, r5.x
    mov r9.z, r6.x
    mov r9.w, r7.x
    dp4 r10.x, r9.xyzw, r1.xyzw
    mov r11.x, r4.z
    mov r11.y, r5.y
    mov r11.z, r6.y
    mov r11.w, r7.y
    dp4 r10.y, r11.xyzw, r1.xyzw
    mov r7.x, r4.w
    mov r7.y, r5.z
    mov r7.z, r6.z
    dp4 r10.z, r7.xyzw, r1.xyzw
    mov r4.xyw, r1.xyxw
    mov r4.z, r8.z
    dp4 r5.x, r9.xyzw, r4.xyzw
    dp4 r5.y, r11.xyzw, r4.xyzw
    dp4 r5.z, r7.xyzw, r4.xyzw
    min r6.xyz, r5.xyzx, r10.xyzx
    max r5.xyz, r5.xyzx, r10.xyzx
    mov r10.xzw, r1.xxzw
    mov r10.y, r8.y
    dp4 r12.x, r9.xyzw, r10.xyzw
    dp4 r12.y, r11.xyzw, r10.xyzw
    dp4 r12.z, r7.xyzw, r10.xyzw
    min r6.xyz, r6.xyzx, r12.xyzx
    max r5.xyz, r5.xyzx, r12.xyzx
    mov r4.xyw, r10.xyxw
    dp4 r10.x, r9.xyzw, r4.xyzw
    dp4 r10.y, r11.xyzw, r4.xyzw
    dp4 r10.z, r7.xyzw, r4.xyzw
    min r6.xyz, r6.xyzx, r10.xyzx
    max r5.xyz, r5.xyzx, r10.xyzx
    mov r8.yzw, r1.yyzw
    dp4 r1.x, r9.xyzw, r8.xyzw
    dp4 r1.y, r11.xyzw, r8.xyzw
    dp4 r1.z, r7.xyzw, r8.xyzw
    min r6.xyz, r1.xyzx, r6.xyzx
    max r1.xyz, r1.xyzx, r5.xyzx
    mov r5.xyw, r8.xyxw
    mov r5.z, r4.z
    dp4 r10.x, r9.xyzw, r5.xyzw
    dp4 r10.y, r11.xyzw, r5.xyzw
    dp4 r10.z, r7.xyzw, r5.xyzw
    min r6.xyz, r6.xyzx, r10.xyzx
    max r1.xyz, r1.xyzx, r10.xyzx
    mov r4.xzw, r8.xxzw
    dp4 r8.x, r9.xyzw, r4.xyzw
    dp4 r8.y, r11.xyzw, r4.xyzw
    dp4 r8.z, r7.xyzw, r4.xyzw
    min r6.xyz, r6.xyzx, r8.xyzx
    max r1.xyz, r1.xyzx, r8.xyzx
    mov r5.xyw, r4.xyxw
    dp4 r4.x, r9.xyzw, r5.xyzw
    dp4 r4.y, r11.xyzw, r5.xyzw
    dp4 r4.z, r7.xyzw, r5.xyzw
    min r5.xyz, r4.xyzx, r6.xyzx
    max r1.xyz, r1.xyzx, r4.xyzx
    min r4.xyz, r2.xyzx, r5.xyzx
    max r6.xyz, r1.xyzx, r3.xyzx
    movc r2.xyz, r0.zzzz, r4.xyzx, r5.xyzx
    movc r3.xyz, r0.zzzz, r6.xyzx, r1.xyzx
    iadd r0.z, r0.z, l(1)
  endloop 
else 
  ishl r0.x, vThreadID.x, l(4)
  mov r2.xyz, l(0,0,0,0)
  mov r3.xyz, l(0,0,0,0)
  mov r0.y, l(0)
  loop 
    uge r0.z, r0.y, l(16)
    breakc_nz r0.z
    imad r0.z, r0.x, CB0[0][0].y, r0.y
    uge r0.w, r0.z, CB0[0][0].z
    if_nz r0.w
      break 
    endif 
    ld_structured r1.xyz, r0.z, l(0), U0[0].xyzx
    ld_structured r4.xyz, r0.z, l(16), U0[0].xyzx
    min r5.xyz, r1.xyzx, r2.xyzx
    max r6.xyz, r3.xyzx, r4.xyzx
    movc r2.xyz, r0.yyyy, r5.xyzx, r1.xyzx
    movc r3.xyz, r0.yyyy, r6.xyzx, r4.xyzx
    iadd r0.y, r0.y, l(1)
  endloop 
endif 
mov r2.w, l(0)
store_structured U0[0].xyzw, vThreadID.x, l(0), r2.xyzw
mov r3.w, l(0)
store_structured U0[0].xyzw, vThreadID.x, l(16), r3.xyzw
ret 
// Approximately 113 instruction slots used
#endif

const BYTE CalculateLBVHsBoundingBox_ByteCode[] =
{
     68,  88,  66,  67, 242, 206, 
    241, 114,  20,  86, 143, 139, 
    101, 199,  57,  39, 239, 140, 
    244, 161,   1,   0,   0,   0, 
    116,  17,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     40,   5,   0,   0,  56,   5, 
      0,   0,  72,   5,   0,   0, 
    216,  16,   0,   0,  82,  68, 
     69,  70, 236,   4,   0,   0, 
      4,   0,   0,   0,   0,   1, 
      0,   0,   4,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
     83,  67,   0,   1,  32,   0, 
    196,   4,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    220,   0,   0,   0,   5,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   0,   0,   0,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 224,   0, 
      0,   0,   5,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0, 136,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 236,   0,   0,   0, 
      6,   0,   0,   0,   6,   0, 
      0,   0,   1,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    249,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 103, 110, 
     98,   0, 103, 110,  98,  95, 
    111, 102, 102, 115, 101, 116, 
    115,   0,  98, 111, 117, 110, 
    100, 105, 110, 103,  95,  98, 
    111, 120,   0, 105, 110, 112, 
    117, 116,   0, 171, 249,   0, 
      0,   0,   1,   0,   0,   0, 
     96,   1,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 220,   0, 
      0,   0,   1,   0,   0,   0, 
     56,   2,   0,   0,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0, 224,   0, 
      0,   0,   1,   0,   0,   0, 
    168,   3,   0,   0, 136,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0, 236,   0, 
      0,   0,   1,   0,   0,   0, 
    156,   4,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0, 249,   0, 
      0,   0,   0,   0,   0,   0, 
     12,   0,   0,   0,   2,   0, 
      0,   0,  20,   2,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  73, 110, 112, 117, 
    116,  68,  97, 116,  97,   0, 
    103, 110,  98,  95, 111, 102, 
    102, 115, 101, 116,  95,  99, 
    111, 117, 110, 116,   0, 100, 
    119, 111, 114, 100,   0, 171, 
    171, 171,   0,   0,  19,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 163,   1, 
      0,   0, 105, 116, 101, 114, 
     97, 116, 105, 111, 110,  95, 
     99, 111, 117, 110, 116,   0, 
    101, 108, 101, 109, 101, 110, 
    116,  95,  99, 111, 117, 110, 
    116,   0, 171, 171, 146,   1, 
      0,   0, 172,   1,   0,   0, 
      0,   0,   0,   0, 208,   1, 
      0,   0, 172,   1,   0,   0, 
      4,   0,   0,   0, 224,   1, 
      0,   0, 172,   1,   0,   0, 
      8,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,   3,   0, 
      0,   0,   3,   0, 240,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    136,   1,   0,   0,  96,   2, 
      0,   0,   0,   0,   0,   0, 
     48,   0,   0,   0,   2,   0, 
      0,   0, 132,   3,   0,   0, 
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
     97, 116,  52,   0,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    133,   2,   0,   0, 112,  77, 
     97, 120,   0, 171, 171, 171, 
    128,   2,   0,   0, 140,   2, 
      0,   0,   0,   0,   0,   0, 
    176,   2,   0,   0, 140,   2, 
      0,   0,  16,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
      8,   0,   0,   0,   2,   0, 
    184,   2,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 121,   2,   0,   0, 
    101, 108, 101, 109, 101, 110, 
    116,  95, 105, 110, 100, 101, 
    120,   0, 171, 171,   0,   0, 
     19,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    163,   1,   0,   0, 112,  97, 
    114, 101, 110, 116,   0, 108, 
    101, 102, 116,  95,  99, 104, 
    105, 108, 100,   0, 114, 105, 
    103, 104, 116,  95,  99, 104, 
    105, 108, 100,   0, 171, 171, 
    114,   2,   0,   0, 208,   2, 
      0,   0,   0,   0,   0,   0, 
    244,   2,   0,   0,   4,   3, 
      0,   0,  32,   0,   0,   0, 
     40,   3,   0,   0,   4,   3, 
      0,   0,  36,   0,   0,   0, 
     47,   3,   0,   0,   4,   3, 
      0,   0,  40,   0,   0,   0, 
     58,   3,   0,   0,   4,   3, 
      0,   0,  44,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
     12,   0,   0,   0,   5,   0, 
     72,   3,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 105,   2,   0,   0, 
     96,   2,   0,   0,   0,   0, 
      0,   0, 136,   0,   0,   0, 
      2,   0,   0,   0, 120,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  83,  99, 
    101, 110, 101,  76,  66,  86, 
     72,  73, 110, 115, 116,  97, 
    110,  99, 101,   0, 118, 101, 
    114, 116, 101, 120,  95, 111, 
    102, 102, 115, 101, 116,   0, 
    110, 111, 100, 101,  95, 111, 
    102, 102, 115, 101, 116,   0, 
    116, 114,  97, 110, 115, 102, 
    111, 114, 109,   0, 102, 108, 
    111,  97, 116,  52, 120,  52, 
      0, 171,   3,   0,   3,   0, 
      4,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   6,   4, 
      0,   0, 116, 114,  97, 110, 
    115, 102, 111, 114, 109,  95, 
    105, 110, 118, 101, 114, 115, 
    101,   0, 171, 171, 226,   3, 
      0,   0,   4,   3,   0,   0, 
      0,   0,   0,   0, 240,   3, 
      0,   0,   4,   3,   0,   0, 
      4,   0,   0,   0, 252,   3, 
      0,   0,  16,   4,   0,   0, 
      8,   0,   0,   0,  52,   4, 
      0,   0,  16,   4,   0,   0, 
     72,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,  34,   0, 
      0,   0,   4,   0,  72,   4, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    208,   3,   0,   0,  96,   2, 
      0,   0,   0,   0,   0,   0, 
     32,   0,   0,   0,   2,   0, 
      0,   0, 208,   2,   0,   0, 
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
    136,  11,   0,   0,  81,   0, 
      5,   0, 226,   2,   0,   0, 
    106,   8,   8,   1,  89,   0, 
      0,   7,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0, 162,   0,   0,   7, 
     70, 126,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  48,   0, 
      0,   0,   0,   0,   0,   0, 
    162,   0,   0,   7,  70, 126, 
     48,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 136,   0,   0,   0, 
      0,   0,   0,   0, 158,   0, 
      0,   7,  70, 238,  49,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,  95,   0,   0,   2, 
     18,   0,   2,   0, 104,   0, 
      0,   2,  13,   0,   0,   0, 
    155,   0,   0,   4,  64,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  31,   0, 
      0,   5,  26, 128,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     41,   0,   0,   6,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,   2,   0,   1,  64, 
      0,   0,   4,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  54,   0,   0,   8, 
    114,   0,  16,   0,   2,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   8, 114,   0,  16,   0, 
      3,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     48,   0,   0,   1,  80,   0, 
      0,   7, 130,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  16,   0, 
      0,   0,   3,   0,   4,   3, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  30,   0,   0,   7, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     80,   0,   0,   7,  18,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  31,   0, 
      4,   3,  10,   0,  16,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   1,  21,   0,   0,   1, 
    167,   0,   0,  10, 242,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      4,   0,   0,   0,  70, 126, 
     32,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 167,   0, 
      0,  10, 114,   0,  16,   0, 
      5,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  24,   0, 
      0,   0,  70, 114,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 167,   0,   0,  10, 
    114,   0,  16,   0,   6,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  40,   0,   0,   0, 
     70, 114,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    167,   0,   0,  10, 178,   0, 
     16,   0,   7,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
     56,   0,   0,   0,  70, 120, 
     32,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 167,   0, 
      0,  10, 114,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  70, 114,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 167,   0,   0,  10, 
    114,   0,  16,   0,   8,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,   1,  64, 
      0,   0,  16,   0,   0,   0, 
     70, 114,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   5,  18,   0, 
     16,   0,   9,   0,   0,   0, 
     26,   0,  16,   0,   4,   0, 
      0,   0,  54,   0,   0,   5, 
     34,   0,  16,   0,   9,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      9,   0,   0,   0,  10,   0, 
     16,   0,   6,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,   9,   0,   0,   0, 
     10,   0,  16,   0,   7,   0, 
      0,   0,  17,   0,   0,   7, 
     18,   0,  16,   0,  10,   0, 
      0,   0,  70,  14,  16,   0, 
      9,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5,  18,   0, 
     16,   0,  11,   0,   0,   0, 
     42,   0,  16,   0,   4,   0, 
      0,   0,  54,   0,   0,   5, 
     34,   0,  16,   0,  11,   0, 
      0,   0,  26,   0,  16,   0, 
      5,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
     11,   0,   0,   0,  26,   0, 
     16,   0,   6,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,  11,   0,   0,   0, 
     26,   0,  16,   0,   7,   0, 
      0,   0,  17,   0,   0,   7, 
     34,   0,  16,   0,  10,   0, 
      0,   0,  70,  14,  16,   0, 
     11,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5,  18,   0, 
     16,   0,   7,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  54,   0,   0,   5, 
     34,   0,  16,   0,   7,   0, 
      0,   0,  42,   0,  16,   0, 
      5,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      7,   0,   0,   0,  42,   0, 
     16,   0,   6,   0,   0,   0, 
     17,   0,   0,   7,  66,   0, 
     16,   0,  10,   0,   0,   0, 
     70,  14,  16,   0,   7,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   5, 178,   0,  16,   0, 
      4,   0,   0,   0,  70,  12, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5,  66,   0, 
     16,   0,   4,   0,   0,   0, 
     42,   0,  16,   0,   8,   0, 
      0,   0,  17,   0,   0,   7, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  70,  14,  16,   0, 
      9,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     17,   0,   0,   7,  34,   0, 
     16,   0,   5,   0,   0,   0, 
     70,  14,  16,   0,  11,   0, 
      0,   0,  70,  14,  16,   0, 
      4,   0,   0,   0,  17,   0, 
      0,   7,  66,   0,  16,   0, 
      5,   0,   0,   0,  70,  14, 
     16,   0,   7,   0,   0,   0, 
     70,  14,  16,   0,   4,   0, 
      0,   0,  51,   0,   0,   7, 
    114,   0,  16,   0,   6,   0, 
      0,   0,  70,   2,  16,   0, 
      5,   0,   0,   0,  70,   2, 
     16,   0,  10,   0,   0,   0, 
     52,   0,   0,   7, 114,   0, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
     10,   0,   0,   0,  54,   0, 
      0,   5, 210,   0,  16,   0, 
     10,   0,   0,   0,   6,  14, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5,  34,   0, 
     16,   0,  10,   0,   0,   0, 
     26,   0,  16,   0,   8,   0, 
      0,   0,  17,   0,   0,   7, 
     18,   0,  16,   0,  12,   0, 
      0,   0,  70,  14,  16,   0, 
      9,   0,   0,   0,  70,  14, 
     16,   0,  10,   0,   0,   0, 
     17,   0,   0,   7,  34,   0, 
     16,   0,  12,   0,   0,   0, 
     70,  14,  16,   0,  11,   0, 
      0,   0,  70,  14,  16,   0, 
     10,   0,   0,   0,  17,   0, 
      0,   7,  66,   0,  16,   0, 
     12,   0,   0,   0,  70,  14, 
     16,   0,   7,   0,   0,   0, 
     70,  14,  16,   0,  10,   0, 
      0,   0,  51,   0,   0,   7, 
    114,   0,  16,   0,   6,   0, 
      0,   0,  70,   2,  16,   0, 
      6,   0,   0,   0,  70,   2, 
     16,   0,  12,   0,   0,   0, 
     52,   0,   0,   7, 114,   0, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
     12,   0,   0,   0,  54,   0, 
      0,   5, 178,   0,  16,   0, 
      4,   0,   0,   0,  70,  12, 
     16,   0,  10,   0,   0,   0, 
     17,   0,   0,   7,  18,   0, 
     16,   0,  10,   0,   0,   0, 
     70,  14,  16,   0,   9,   0, 
      0,   0,  70,  14,  16,   0, 
      4,   0,   0,   0,  17,   0, 
      0,   7,  34,   0,  16,   0, 
     10,   0,   0,   0,  70,  14, 
     16,   0,  11,   0,   0,   0, 
     70,  14,  16,   0,   4,   0, 
      0,   0,  17,   0,   0,   7, 
     66,   0,  16,   0,  10,   0, 
      0,   0,  70,  14,  16,   0, 
      7,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     51,   0,   0,   7, 114,   0, 
     16,   0,   6,   0,   0,   0, 
     70,   2,  16,   0,   6,   0, 
      0,   0,  70,   2,  16,   0, 
     10,   0,   0,   0,  52,   0, 
      0,   7, 114,   0,  16,   0, 
      5,   0,   0,   0,  70,   2, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16,   0,  10,   0, 
      0,   0,  54,   0,   0,   5, 
    226,   0,  16,   0,   8,   0, 
      0,   0,  86,  14,  16,   0, 
      1,   0,   0,   0,  17,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   9,   0,   0,   0, 
     70,  14,  16,   0,   8,   0, 
      0,   0,  17,   0,   0,   7, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
     11,   0,   0,   0,  70,  14, 
     16,   0,   8,   0,   0,   0, 
     17,   0,   0,   7,  66,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   7,   0, 
      0,   0,  70,  14,  16,   0, 
      8,   0,   0,   0,  51,   0, 
      0,   7, 114,   0,  16,   0, 
      6,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   6,   0, 
      0,   0,  52,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   5,   0,   0,   0, 
     54,   0,   0,   5, 178,   0, 
     16,   0,   5,   0,   0,   0, 
     70,  12,  16,   0,   8,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   5,   0, 
      0,   0,  42,   0,  16,   0, 
      4,   0,   0,   0,  17,   0, 
      0,   7,  18,   0,  16,   0, 
     10,   0,   0,   0,  70,  14, 
     16,   0,   9,   0,   0,   0, 
     70,  14,  16,   0,   5,   0, 
      0,   0,  17,   0,   0,   7, 
     34,   0,  16,   0,  10,   0, 
      0,   0,  70,  14,  16,   0, 
     11,   0,   0,   0,  70,  14, 
     16,   0,   5,   0,   0,   0, 
     17,   0,   0,   7,  66,   0, 
     16,   0,  10,   0,   0,   0, 
     70,  14,  16,   0,   7,   0, 
      0,   0,  70,  14,  16,   0, 
      5,   0,   0,   0,  51,   0, 
      0,   7, 114,   0,  16,   0, 
      6,   0,   0,   0,  70,   2, 
     16,   0,   6,   0,   0,   0, 
     70,   2,  16,   0,  10,   0, 
      0,   0,  52,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,  10,   0,   0,   0, 
     54,   0,   0,   5, 210,   0, 
     16,   0,   4,   0,   0,   0, 
      6,  14,  16,   0,   8,   0, 
      0,   0,  17,   0,   0,   7, 
     18,   0,  16,   0,   8,   0, 
      0,   0,  70,  14,  16,   0, 
      9,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     17,   0,   0,   7,  34,   0, 
     16,   0,   8,   0,   0,   0, 
     70,  14,  16,   0,  11,   0, 
      0,   0,  70,  14,  16,   0, 
      4,   0,   0,   0,  17,   0, 
      0,   7,  66,   0,  16,   0, 
      8,   0,   0,   0,  70,  14, 
     16,   0,   7,   0,   0,   0, 
     70,  14,  16,   0,   4,   0, 
      0,   0,  51,   0,   0,   7, 
    114,   0,  16,   0,   6,   0, 
      0,   0,  70,   2,  16,   0, 
      6,   0,   0,   0,  70,   2, 
     16,   0,   8,   0,   0,   0, 
     52,   0,   0,   7, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      8,   0,   0,   0,  54,   0, 
      0,   5, 178,   0,  16,   0, 
      5,   0,   0,   0,  70,  12, 
     16,   0,   4,   0,   0,   0, 
     17,   0,   0,   7,  18,   0, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   9,   0, 
      0,   0,  70,  14,  16,   0, 
      5,   0,   0,   0,  17,   0, 
      0,   7,  34,   0,  16,   0, 
      4,   0,   0,   0,  70,  14, 
     16,   0,  11,   0,   0,   0, 
     70,  14,  16,   0,   5,   0, 
      0,   0,  17,   0,   0,   7, 
     66,   0,  16,   0,   4,   0, 
      0,   0,  70,  14,  16,   0, 
      7,   0,   0,   0,  70,  14, 
     16,   0,   5,   0,   0,   0, 
     51,   0,   0,   7, 114,   0, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  70,   2,  16,   0, 
      6,   0,   0,   0,  52,   0, 
      0,   7, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  51,   0,   0,   7, 
    114,   0,  16,   0,   4,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   5,   0,   0,   0, 
     52,   0,   0,   7, 114,   0, 
     16,   0,   6,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  55,   0, 
      0,   9, 114,   0,  16,   0, 
      2,   0,   0,   0, 166,  10, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  70,   2,  16,   0, 
      5,   0,   0,   0,  55,   0, 
      0,   9, 114,   0,  16,   0, 
      3,   0,   0,   0, 166,  10, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   6,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  30,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   1,   0, 
      0,   0,  22,   0,   0,   1, 
     18,   0,   0,   1,  41,   0, 
      0,   6,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
      2,   0,   1,  64,   0,   0, 
      4,   0,   0,   0,  54,   0, 
      0,   8, 114,   0,  16,   0, 
      2,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   8, 114,   0, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
     34,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  48,   0, 
      0,   1,  80,   0,   0,   7, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  16,   0,   0,   0, 
      3,   0,   4,   3,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     35,   0,   0,  11,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  26, 128,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  80,   0,   0,   9, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,  42, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  31,   0,   4,   3, 
     58,   0,  16,   0,   0,   0, 
      0,   0,   2,   0,   0,   1, 
     21,   0,   0,   1, 167,   0, 
      0,  10, 114,   0,  16,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  70, 226,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 167,   0,   0,  10, 
    114,   0,  16,   0,   4,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  16,   0,   0,   0, 
     70, 226,  33,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     51,   0,   0,   7, 114,   0, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  52,   0, 
      0,   7, 114,   0,  16,   0, 
      6,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  55,   0,   0,   9, 
    114,   0,  16,   0,   2,   0, 
      0,   0,  86,   5,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  55,   0,   0,   9, 
    114,   0,  16,   0,   3,   0, 
      0,   0,  86,   5,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   6,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  30,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   1,   0,   0,   0, 
     22,   0,   0,   1,  21,   0, 
      0,   1,  54,   0,   0,   5, 
    130,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0, 168,   0, 
      0,   9, 242, 224,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  10,   0,   2,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
    168,   0,   0,   9, 242, 224, 
     33,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  10,   0, 
      2,   0,   1,  64,   0,   0, 
     16,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 148,   0,   0,   0, 
    113,   0,   0,   0,  13,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  42,   0, 
      0,   0,   6,   0,   0,   0, 
      4,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  31,   0, 
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
      2,   0,   0,   0
};
