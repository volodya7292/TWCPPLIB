#include "pch.h"
#include "TW3DGraphicsCommandList.h"


TW3D::TW3DGraphicsCommandList::TW3DGraphicsCommandList(TW3D::TW3DDevice device) {
}


TW3D::TW3DGraphicsCommandList::~TW3DGraphicsCommandList() {
	TWU::DXSafeRelease(commandList);
}
