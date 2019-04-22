#include "HLSLHelper.hlsli"

//inline int longest_common_prefix(int i, int j, int nObjects, unsigned int* mortonCodes) {
//	if (j < 0 || j >= nObjects) {
//		return -1;
//	} else {
//		uint mortonCodeA = mortonCodes[i];
//		uint mortonCodeB = mortonCodes[j];
//		if (mortonCodeA != mortonCodeB) {
//			return 31 - firstbithigh((unsigned int)(mortonCodes[i] ^ mortonCodes[j]));
//			//return __lzcnt((unsigned int)(mortonCodes[i] ^ mortonCodes[j]));
//		} else {
//			// TODO: Technically this should be primitive ID
//			return (31 - firstbithigh(i ^ j)) + 31;
//			//return (unsigned int)lzcnt64(i ^ j);// + 31;
//		}
//	}
//}

RWStructuredBuffer<LBVHNode> nodes : register(u0);

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
}