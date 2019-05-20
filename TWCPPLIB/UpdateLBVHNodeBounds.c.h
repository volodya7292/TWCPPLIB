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
//       uint leaves_offset;            // Offset:    0
//       uint leaf_count;               // Offset:    4
//
//   } input;                           // Offset:    0 Size:     8
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
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
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
dcl_uav_structured U0[0:0], 48, space=0
dcl_input vThreadID.x
dcl_temps 12
dcl_thread_group 64, 1, 1
iadd r0.x, vThreadID.x, CB0[0][0].x
uge r0.y, r0.x, CB0[0][0].y
if_nz r0.y
  ret 
endif 
ld_structured r0.y, r0.x, l(36), U0[0].xxxx
ld_structured r0.zw, r0.y, l(40), U0[0].xxxy
mov r1.w, l(0)
mov r2.w, l(0)
mov r3.xyzw, l(0,0,0,0)
mov r2.xy, l(0,0,0,0)
mov r4.z, r0.x
mov r4.xyw, r0.zwzy
loop 
  ld_structured r5.x, r4.w, l(40), U0[0].xxxx
  ine r5.x, r4.z, r5.x
  breakc_nz r5.x
  ld_structured r5.xyz, r4.x, l(0), U0[0].xyzx
  ld_structured r6.xyz, r4.x, l(16), U0[0].xyzx
  ld_structured r7.xyz, r4.y, l(0), U0[0].xyzx
  ld_structured r8.xyz, r4.y, l(16), U0[0].xyzx
  ine r9.xy, r4.xyxx, l(-1, -1, 0, 0)
  and r6.w, r9.y, r9.x
  min r10.xyz, r5.xyzx, r7.xyzx
  max r11.xyz, r6.xyzx, r8.xyzx
  mov r7.w, r8.x
  movc r7.xyzw, r9.yyyy, r7.xyzw, r3.xyzw
  movc r8.xy, r9.yyyy, r8.yzyy, r2.xyxx
  mov r5.w, r6.x
  movc r5.xyzw, r9.xxxx, r5.xyzw, r7.xyzw
  movc r6.xy, r9.xxxx, r6.yzyy, r8.xyxx
  mov r10.w, r11.x
  movc r3.xyzw, r6.wwww, r10.xyzw, r5.xyzw
  movc r2.xy, r6.wwww, r11.yzyy, r6.xyxx
  ne r5.xyzw, r3.xyzw, l(-340282346638528859811704183484516925440.000000, -340282346638528859811704183484516925440.000000, -340282346638528859811704183484516925440.000000, 340282346638528859811704183484516925440.000000)
  and r5.x, r5.y, r5.x
  and r5.x, r5.z, r5.x
  ne r5.yz, r2.xxyx, l(0.000000, 340282346638528859811704183484516925440.000000, 340282346638528859811704183484516925440.000000, 0.000000)
  and r5.y, r5.y, r5.w
  and r5.y, r5.z, r5.y
  and r5.x, r5.y, r5.x
  if_nz r5.x
    mov r1.xyz, r3.xyzx
    store_structured U0[0].xyzw, r4.w, l(0), r1.xyzw
    mov r2.z, r3.w
    store_structured U0[0].xyzw, r4.w, l(16), r2.zxyw
    ld_structured r1.x, r4.w, l(36), U0[0].xxxx
    mov r4.z, r4.w
    mov r4.w, r1.x
  endif 
  ieq r1.x, r4.w, l(-1)
  if_nz r1.x
    break 
  endif 
  ld_structured r4.xy, r4.w, l(40), U0[0].xyxx
endloop 
ret 
// Approximately 57 instruction slots used
#endif

const BYTE UpdateLBVHNodeBounds_ByteCode[] =
{
     68,  88,  66,  67, 239,  92, 
    216, 222, 188, 194, 205, 157, 
    191,  95, 190, 138, 242, 185, 
    222,  46,   1,   0,   0,   0, 
     72,  10,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     80,   3,   0,   0,  96,   3, 
      0,   0, 112,   3,   0,   0, 
    172,   9,   0,   0,  82,  68, 
     69,  70,  20,   3,   0,   0, 
      2,   0,   0,   0, 152,   0, 
      0,   0,   2,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
     83,  67,   0,   1,  32,   0, 
    236,   2,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    140,   0,   0,   0,   6,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   0,   0,   0,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 146,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 110, 111, 100, 101, 
    115,   0, 105, 110, 112, 117, 
    116,   0, 146,   0,   0,   0, 
      1,   0,   0,   0, 200,   0, 
      0,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 140,   0,   0,   0, 
      1,   0,   0,   0, 124,   1, 
      0,   0,  48,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0, 146,   0,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,   2,   0,   0,   0, 
     88,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     73, 110, 112, 117, 116,  68, 
     97, 116,  97,   0, 108, 101, 
     97, 118, 101, 115,  95, 111, 
    102, 102, 115, 101, 116,   0, 
    100, 119, 111, 114, 100,   0, 
    171, 171,   0,   0,  19,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   8,   1, 
      0,   0, 108, 101,  97, 102, 
     95,  99, 111, 117, 110, 116, 
      0, 171, 250,   0,   0,   0, 
     16,   1,   0,   0,   0,   0, 
      0,   0,  52,   1,   0,   0, 
     16,   1,   0,   0,   4,   0, 
      0,   0,   5,   0,   0,   0, 
      1,   0,   2,   0,   0,   0, 
      2,   0,  64,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 240,   0, 
      0,   0, 164,   1,   0,   0, 
      0,   0,   0,   0,  48,   0, 
      0,   0,   2,   0,   0,   0, 
    200,   2,   0,   0,   0,   0, 
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
     52,   0,   1,   0,   3,   0, 
      1,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 201,   1, 
      0,   0, 112,  77,  97, 120, 
      0, 171, 171, 171, 196,   1, 
      0,   0, 208,   1,   0,   0, 
      0,   0,   0,   0, 244,   1, 
      0,   0, 208,   1,   0,   0, 
     16,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,   8,   0, 
      0,   0,   2,   0, 252,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    189,   1,   0,   0, 101, 108, 
    101, 109, 101, 110, 116,  95, 
    105, 110, 100, 101, 120,   0, 
    171, 171,   0,   0,  19,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   8,   1, 
      0,   0, 112,  97, 114, 101, 
    110, 116,   0, 108, 101, 102, 
    116,  95,  99, 104, 105, 108, 
    100,   0, 114, 105, 103, 104, 
    116,  95,  99, 104, 105, 108, 
    100,   0, 171, 171, 182,   1, 
      0,   0,  20,   2,   0,   0, 
      0,   0,   0,   0,  56,   2, 
      0,   0,  72,   2,   0,   0, 
     32,   0,   0,   0, 108,   2, 
      0,   0,  72,   2,   0,   0, 
     36,   0,   0,   0, 115,   2, 
      0,   0,  72,   2,   0,   0, 
     40,   0,   0,   0, 126,   2, 
      0,   0,  72,   2,   0,   0, 
     44,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,  12,   0, 
      0,   0,   5,   0, 140,   2, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    173,   1,   0,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  49,  48,  46, 
     49,   0,  73,  83,  71,  78, 
      8,   0,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     79,  83,  71,  78,   8,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  83,  72, 
     69,  88,  52,   6,   0,   0, 
     81,   0,   5,   0, 141,   1, 
      0,   0, 106,   8,   8,   1, 
     89,   0,   0,   7,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0, 158,   0, 
      0,   7,  70, 238,  49,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     48,   0,   0,   0,   0,   0, 
      0,   0,  95,   0,   0,   2, 
     18,   0,   2,   0, 104,   0, 
      0,   2,  12,   0,   0,   0, 
    155,   0,   0,   4,  64,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  30,   0, 
      0,   8,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
      2,   0,  10, 128,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     80,   0,   0,   9,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  26, 128,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     31,   0,   4,   3,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     62,   0,   0,   1,  21,   0, 
      0,   1, 167,   0,   0,  10, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  36,   0,   0,   0, 
      6, 224,  33,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    167,   0,   0,  10, 194,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
     40,   0,   0,   0,   6, 228, 
     33,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   8, 
    242,   0,  16,   0,   3,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   8,  50,   0,  16,   0, 
      2,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   5,  66,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    178,   0,  16,   0,   4,   0, 
      0,   0, 230,   6,  16,   0, 
      0,   0,   0,   0,  48,   0, 
      0,   1, 167,   0,   0,  10, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,   1,  64, 
      0,   0,  40,   0,   0,   0, 
      6, 224,  33,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     39,   0,   0,   7,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     42,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,   3,   0, 
      4,   3,  10,   0,  16,   0, 
      5,   0,   0,   0, 167,   0, 
      0,  10, 114,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  70, 226,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 167,   0,   0,  10, 
    114,   0,  16,   0,   6,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,   1,  64, 
      0,   0,  16,   0,   0,   0, 
     70, 226,  33,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    167,   0,   0,  10, 114,   0, 
     16,   0,   7,   0,   0,   0, 
     26,   0,  16,   0,   4,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  70, 226, 
     33,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 167,   0, 
      0,  10, 114,   0,  16,   0, 
      8,   0,   0,   0,  26,   0, 
     16,   0,   4,   0,   0,   0, 
      1,  64,   0,   0,  16,   0, 
      0,   0,  70, 226,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  39,   0,   0,  10, 
     50,   0,  16,   0,   9,   0, 
      0,   0,  70,   0,  16,   0, 
      4,   0,   0,   0,   2,  64, 
      0,   0, 255, 255, 255, 255, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   7, 130,   0, 
     16,   0,   6,   0,   0,   0, 
     26,   0,  16,   0,   9,   0, 
      0,   0,  10,   0,  16,   0, 
      9,   0,   0,   0,  51,   0, 
      0,   7, 114,   0,  16,   0, 
     10,   0,   0,   0,  70,   2, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16,   0,   7,   0, 
      0,   0,  52,   0,   0,   7, 
    114,   0,  16,   0,  11,   0, 
      0,   0,  70,   2,  16,   0, 
      6,   0,   0,   0,  70,   2, 
     16,   0,   8,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,   7,   0,   0,   0, 
     10,   0,  16,   0,   8,   0, 
      0,   0,  55,   0,   0,   9, 
    242,   0,  16,   0,   7,   0, 
      0,   0,  86,   5,  16,   0, 
      9,   0,   0,   0,  70,  14, 
     16,   0,   7,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  55,   0,   0,   9, 
     50,   0,  16,   0,   8,   0, 
      0,   0,  86,   5,  16,   0, 
      9,   0,   0,   0, 150,   5, 
     16,   0,   8,   0,   0,   0, 
     70,   0,  16,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
    130,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      6,   0,   0,   0,  55,   0, 
      0,   9, 242,   0,  16,   0, 
      5,   0,   0,   0,   6,   0, 
     16,   0,   9,   0,   0,   0, 
     70,  14,  16,   0,   5,   0, 
      0,   0,  70,  14,  16,   0, 
      7,   0,   0,   0,  55,   0, 
      0,   9,  50,   0,  16,   0, 
      6,   0,   0,   0,   6,   0, 
     16,   0,   9,   0,   0,   0, 
    150,   5,  16,   0,   6,   0, 
      0,   0,  70,   0,  16,   0, 
      8,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
     10,   0,   0,   0,  10,   0, 
     16,   0,  11,   0,   0,   0, 
     55,   0,   0,   9, 242,   0, 
     16,   0,   3,   0,   0,   0, 
    246,  15,  16,   0,   6,   0, 
      0,   0,  70,  14,  16,   0, 
     10,   0,   0,   0,  70,  14, 
     16,   0,   5,   0,   0,   0, 
     55,   0,   0,   9,  50,   0, 
     16,   0,   2,   0,   0,   0, 
    246,  15,  16,   0,   6,   0, 
      0,   0, 150,   5,  16,   0, 
     11,   0,   0,   0,  70,   0, 
     16,   0,   6,   0,   0,   0, 
     57,   0,   0,  10, 242,   0, 
     16,   0,   5,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,   2,  64,   0,   0, 
    255, 255, 127, 255, 255, 255, 
    127, 255, 255, 255, 127, 255, 
    255, 255, 127, 127,   1,   0, 
      0,   7,  18,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,   1,   0,   0,   7, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  42,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     57,   0,   0,  10,  98,   0, 
     16,   0,   5,   0,   0,   0, 
      6,   1,  16,   0,   2,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    127, 127, 255, 255, 127, 127, 
      0,   0,   0,   0,   1,   0, 
      0,   7,  34,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
     16,   0,   5,   0,   0,   0, 
     58,   0,  16,   0,   5,   0, 
      0,   0,   1,   0,   0,   7, 
     34,   0,  16,   0,   5,   0, 
      0,   0,  42,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
     16,   0,   5,   0,   0,   0, 
      1,   0,   0,   7,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  31,   0, 
      4,   3,  10,   0,  16,   0, 
      5,   0,   0,   0,  54,   0, 
      0,   5, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
    168,   0,   0,  10, 242, 224, 
     33,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      2,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
    168,   0,   0,  10, 242, 224, 
     33,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
      1,  64,   0,   0,  16,   0, 
      0,   0,  38,  13,  16,   0, 
      2,   0,   0,   0, 167,   0, 
      0,  10,  18,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
      1,  64,   0,   0,  36,   0, 
      0,   0,   6, 224,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     21,   0,   0,   1,  32,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
      1,  64,   0,   0, 255, 255, 
    255, 255,  31,   0,   4,   3, 
     10,   0,  16,   0,   1,   0, 
      0,   0,   2,   0,   0,   1, 
     21,   0,   0,   1, 167,   0, 
      0,  10,  50,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
      1,  64,   0,   0,  40,   0, 
      0,   0,  70, 224,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  22,   0,   0,   1, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 148,   0,   0,   0, 
     57,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   4,   0, 
      0,   0,   4,   0,   0,   0, 
      7,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  13,   0, 
      0,   0,   6,   0,   0,   0, 
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
