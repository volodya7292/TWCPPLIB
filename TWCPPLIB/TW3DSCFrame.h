#pragma once
#include "TW3DResourceManager.h"
#include "TW3DSwapChain.h"

class TW3DSCFrame {
private:
	using CLRecorder = std::function<void(TW3DCommandList* CommandList)>;

	struct QueueCommandList {
		TW3DCommandList* command_list[2];
		CLRecorder recorder;
		TWT::uint priority = 0;
		bool swap = false;
	};

public:
	TW3DSCFrame(TW3DResourceManager* ResourceManager, ID3D12Resource* RenderTargetBuffer);
	~TW3DSCFrame();

	TW3DCommandList* RequestCommandList(TWT::String const& Name, TW3DCommandListType Type = TW3D_CL_DIRECT); // Create/Get static command list
	void RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, CLRecorder Recorder, TWT::uint RecordPriority = 0); // Create dynamic command list
	void SwapCommandList(TWT::String const& Name);
	void ExecuteCommandList(TWT::String const& Name);
	void RequestCommandListRecord(TWT::String const& Name);
	void RecordCommandLists(); // Executed by dedicated threads
	void ClearCommandLists();

	TW3DRenderTarget* RenderTarget;

private:
	struct QueueCommandListCompare {
		bool operator()(const QueueCommandList& v0, const QueueCommandList& v1) {
			return v0.priority < v1.priority;
		}
	};

	TW3DResourceManager* resource_manager;
	std::unordered_map<TWT::String, QueueCommandList> command_lists; // [0] - record, [1] - execute
	std::priority_queue<QueueCommandList, std::vector<QueueCommandList>, QueueCommandListCompare> cl_queue;
	std::mutex cl_queue_mutex;
};

