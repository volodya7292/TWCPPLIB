#include "pch.h"
#include "TW3DSCFrame.h"

TW3DSCFrame::TW3DSCFrame(TW3DResourceManager* ResourceManager, ID3D12Resource* RenderTargetBuffer) :
	resource_manager(ResourceManager) {

	RenderTarget = ResourceManager->CreateRenderTarget(RenderTargetBuffer);
}

TW3DSCFrame::~TW3DSCFrame() {
	ClearCommandLists();
	delete RenderTarget;
}

TW3DCommandList* TW3DSCFrame::RequestCommandList(TWT::String const& Name, TW3DCommandListType Type) {
	auto& cl = command_lists[Name];
	cl.command_list[0] = resource_manager->CreateCommandList(Type);

	return cl.command_list[0];
}

void TW3DSCFrame::RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, CLRecorder Recorder, TWT::uint RecordPriority) {
	auto& cl = command_lists[Name];

	if (!cl.command_list[0]) {
		cl.command_list[0] = resource_manager->CreateCommandList(Type);
		cl.command_list[1] = resource_manager->CreateCommandList(Type);
		cl.recorder = Recorder;
		cl.priority = RecordPriority;

		synchronized(cl_queue_mutex)
			cl_queue.emplace(cl);
	}
}

void TW3DSCFrame::SwapCommandList(TWT::String const& Name) {
	auto& cl = command_lists[Name];
	std::swap(cl.command_list[0], cl.command_list[1]);
}

void TW3DSCFrame::ExecuteCommandList(TWT::String const& Name) {
	auto& cl = command_lists[Name];

	if (cl.swap) {
		std::swap(cl.command_list[0], cl.command_list[1]);
		cl.swap = false;
	}

	if (cl.recorder)
		resource_manager->ExecuteCommandList(cl.command_list[1]); // dynamic cl
	else
		resource_manager->ExecuteCommandList(cl.command_list[0]); // static cl
}

void TW3DSCFrame::RequestCommandListRecord(TWT::String const& Name) {
	synchronized(cl_queue_mutex)
		cl_queue.emplace(command_lists[Name]);
}

void TW3DSCFrame::ClearCommandLists() {
	synchronized(cl_queue_mutex) {
		for (auto [name, cl] : command_lists) {
			delete cl.command_list[0];
			delete cl.command_list[1];
		}
	}

	cl_queue.empty();
	command_lists.clear();
}

void TW3DSCFrame::RecordCommandLists() {
	QueueCommandList cl;
	synchronized(cl_queue_mutex) {
		if (cl_queue.size() == 0)
			return;

		cl = cl_queue.top();
		cl_queue.pop();
	}

	resource_manager->FlushCommandList(cl.command_list[0]);

	cl.command_list[0]->Reset();
	cl.command_list[0]->BindResources(resource_manager);
	cl.recorder(cl.command_list[0]);
	cl.command_list[0]->Close();

	cl.swap = true;
}
