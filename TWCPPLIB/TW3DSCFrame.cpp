#include "pch.h"
#include "TW3DSCFrame.h"

TW3DSCFrame::TW3DSCFrame(TW3DResourceManager* ResourceManager, TWT::uint Index, ID3D12Resource* RenderTargetBuffer) :
	resource_manager(ResourceManager), index(Index) {

	RenderTarget = ResourceManager->CreateRenderTarget(RenderTargetBuffer);
}

TW3DSCFrame::~TW3DSCFrame() {
	ClearCommandLists();
	delete RenderTarget;
}

void TW3DSCFrame::Release() {
	ClearCommandLists();
	RenderTarget->Release();
}

TW3DCommandList* TW3DSCFrame::RequestCommandList(TWT::String const& Name, TW3DCommandListType Type) {
	auto& cl = command_lists[Name];

	if (!cl.command_list[0])
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

		synchronized(cl_queue_sync) {
			QueueCommandList qcl = {Name, cl.priority};
			cl_queue.emplace(qcl);
		}
	}
}

void TW3DSCFrame::FlushCommandList(TWT::String const& Name) {
	auto& cl = command_lists[Name];
	if (cl.command_list[0]) {
		if (cl.recorder)
			resource_manager->FlushCommandList(cl.command_list[1]); // dynamic cl
		else
			resource_manager->FlushCommandList(cl.command_list[0]); // static cl
	}
}

void TW3DSCFrame::PerformSwap() {
	for (auto& [name, cl] : command_lists) {
		if (cl.swap) {
			std::swap(cl.command_list[0], cl.command_list[1]);
			cl.swap = false;
		}
	}
}

void TW3DSCFrame::ExecuteCommandList(TWT::String const& Name, bool FlushBeforeExecution, bool FlushAfterExecution) {
	auto& cl = command_lists[Name];

	if (cl.command_list[0]) {
		if (cl.recorder) { // dynamic cl
			if (FlushBeforeExecution)
				resource_manager->FlushCommandList(cl.command_list[1]);
			resource_manager->ExecuteCommandList(cl.command_list[1]);
			if (FlushAfterExecution)
				resource_manager->FlushCommandList(cl.command_list[1]);
		} else { // static cl
			if (FlushBeforeExecution)
				resource_manager->FlushCommandList(cl.command_list[0]);
			resource_manager->ExecuteCommandList(cl.command_list[0]);
			if (FlushAfterExecution)
				resource_manager->FlushCommandList(cl.command_list[0]);
		}
	}
}

void TW3DSCFrame::RequestCommandListRecord(TWT::String const& Name) {
	synchronized(cl_queue_sync) {
		CommandList& cl = command_lists[Name];
		QueueCommandList qcl = { Name, cl.priority };
		cl_queue.emplace(qcl);
	}
}

void TW3DSCFrame::ClearCommandLists() {
	synchronized(cl_queue_sync) {
		for (auto& [name, cl] : command_lists) {
			delete cl.command_list[0];
			delete cl.command_list[1];
		}
	}

	for (TWT::uint i = 0; i < cl_queue.size(); i++)
		cl_queue.pop();

	command_lists.clear();
}

void TW3DSCFrame::RecordCommandLists() {
	QueueCommandList qcl;
	synchronized(cl_queue_sync) {
		if (cl_queue.size() == 0)
			return;

		qcl = cl_queue.top();
		cl_queue.pop();
	}

	CommandList& cl = command_lists[qcl.name];

	resource_manager->FlushCommandList(cl.command_list[0]);

	cl.command_list[0]->Reset();
	cl.command_list[0]->BindResources(resource_manager);
	cl.recorder(cl.command_list[0]);
	cl.command_list[0]->Close();

	cl.swap = true;
}