#pragma once
#include "TW3DResourceManager.h"
#include "TW3DSwapChain.h"

class TW3DSCFrame {
private:
	using CLRecorder = std::function<void(TW3DSCFrame* Frame, TW3DCommandList* CommandList)>;

	struct CommandList {
		TW3DCommandList* command_list[2];
		CLRecorder recorder;
		TWT::uint priority = 0;
		bool swap = false;
	};

	struct QueueCommandList {
		TWT::String name;
		TWT::uint priority = 0;
	};

public:
	TW3DSCFrame(TW3DResourceManager* ResourceManager, TWT::uint Index, ID3D12Resource* RenderTargetBuffer);
	~TW3DSCFrame();

	void Release();

	// Create/Get static command list
	TW3DCommandList* GetCommandList(TWT::String const& Name);
	TW3DCommandList* RequestCommandList(TWT::String const& Name, TW3DCommandListType Type);
	TW3DCommandList* RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, TW3DGraphicsPipeline* InitialState);
	TW3DCommandList* RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, TW3DComputePipeline* InitialState);

	// Create dynamic command list
	void RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, CLRecorder Recorder, TWT::uint RecordPriority = 0);
	void RequestCommandList(TWT::String const& Name, TW3DCommandListType Type,
		TW3DGraphicsPipeline* InitialState, CLRecorder Recorder, TWT::uint RecordPriority = 0); // Create dynamic command list
	void RequestCommandList(TWT::String const& Name, TW3DCommandListType Type,
		TW3DComputePipeline* InitialState, CLRecorder Recorder, TWT::uint RecordPriority = 0); // Create dynamic command list

	void FlushCommandList(TWT::String const& Name);
	void PerformSwap();
	void ExecuteCommandList(TWT::String const& Name, bool FlushBeforeExecution = false, bool FlushAfterExecution = false);
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
	TWT::uint index;
	std::unordered_map<TWT::String, CommandList> command_lists; // [0] - record, [1] - execute
	std::priority_queue<QueueCommandList, std::vector<QueueCommandList>, QueueCommandListCompare> cl_queue;
	std::mutex cl_queue_sync;
};

