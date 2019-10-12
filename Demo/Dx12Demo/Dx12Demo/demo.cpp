#include "..//..//..//Dumpling/Gui/Dx12/define_dx12.h"
#include "..//../..//Dumpling/Gui/Win32/form.h"
#include <assert.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
using namespace Dumpling;
using Dxgi::FormatPixel;

using namespace Dxgi::DataType;
using namespace Dx12::Enum;
using Win32::ThrowIfFault;

namespace fs = std::filesystem;

int main()
{


#ifdef _DEBUG
	Dx12::InitDebugLayout();
#endif

	fs::path resource_path;

#ifdef _DEBUG
#ifdef _WIN64
	resource_path = u"..\\x64\\debug\\";
#else
	resource_path = U"..\\debug\\";
#endif
#else
#ifdef _WIN64
	resource_path = U"..\\x64\\Release\\";
#else
	resource_path = U"..\\Release\\";
#endif
#endif

	auto p = fs::current_path();

	auto load_file = [&](const char32_t* input_path) {
		auto path = resource_path;
		path.append(input_path);
		auto total_path = fs::absolute(path);
		std::ifstream input(path, std::ios::binary);
		assert(input.is_open());
		size_t file_size = fs::file_size(path);
		std::vector<std::byte> all_buffer;
		all_buffer.resize(file_size);
		input.read(reinterpret_cast<char*>(all_buffer.data()), all_buffer.size());
		return std::move(all_buffer);
	};

	std::vector<std::byte> vs_shader = load_file(U"VertexShader.cso");
	std::vector<std::byte> ps_shader = load_file(U"PixelShader.cso");

	auto Context = ThrowIfFault(Dx12::Context::Create(0));

	auto CommandQueue = ThrowIfFault(Context->CreateCommandQueue(CommandListType::Direct));
	auto CommandAllocator = ThrowIfFault(Context->CreateCommandAllocator(CommandListType::Direct));
	auto Form = Dx12::Form::Create(*CommandQueue);

	auto DescHeap = Context->CreateDescriptorHeap(DescriptorHeapType::RT);

	std::this_thread::sleep_for(std::chrono::milliseconds{ 5000 });
	std::cout << "Down" << std::endl;

	/*

	auto [Reflect, ReR] = Dx12::Reflect(ps_shader.data(), ps_shader.size());

	auto [Factory, re_f] = Dxgi::CreateFactory();b
	auto AllAdapters = Dxgi::EnumAdapter(Factory);
	DXGI_ADAPTER_DESC desc;
	AllAdapters[0]->GetDesc(&desc);

	auto [Device, re_d] = Dx12::CreateDevice(AllAdapters[0], D3D_FEATURE_LEVEL_12_0);

	auto [Queue, re_c] = Device++.CreateCommmandQueue(*CommandListType::Direct);
	Queue->SetName(L"WTF");
	auto [Allocator, re_a] = Device++.CreateCommandAllocator(*CommandListType::Direct);
	Win32::Form form = Win32::Form::create();

	auto swap_chain_desc = Dxgi::CreateDefaultSwapChainDesc(*Dxgi::FormatPixel::RGBA16_Float, 1024, 768);
	auto [SwapChain, re_s] = Dx12::CreateSwapChain(Factory, Queue, form, swap_chain_desc);
	auto [RTDescHead, re_rt1] = Device++.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
	auto [DTDescHead, re_rt2] = Device++.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

	auto DescriptorSize = Device++.GetDescriptorHandleIncrementSize();
	
	auto [DTResource, red] = Device++.CreateDepthStencil2DCommitted(*Dxgi::FormatPixel::D24S8_Unorn_Uint, 1024, 768, 0);
	Device++.CreateDepthStencilView2D(DTResource, DescriptorSize.DSVOffset(DTDescHead, 0));

	auto viewport = Dx12::CreateFullScreenViewport(1024, 768);

	auto [Fence, ref] = Device++.CreateFence(0);

	bool exit = false;
	uint32_t current_buffer = 0;

	struct Point
	{
		float3 Position;
		float2 UV;
	};

	struct Instance
	{
		float2 Shift;
	};

	auto AllInputElement = std::tuple{
		Dx12::ElementVertex{&Point::Position, "Position", 0},
		Dx12::ElementVertex{&Point::UV, "UV", 0},
	};

	auto InstanceElement = std::tuple{
		Dx12::ElementInstance{&Instance::Shift, "Shift", 0, 3}
	};

	Point Rec[] = {
		{float3{0.0, 0.2f, 0.0}, float2{0.5f, 0.0}},
		{float3{0.2f, 0.0, 0.0},float2{0.5f, 0.0} },
		{float3{-0.2f, 0.0, 0.0}, float2{ 0.5f, 0.0 }},
	};

	auto [UploadBuffer, RrUB] = Device++.CreateBufferUploadCommitted(sizeof(Point) * 3);

	HRESULT MappingResult = Dx12::MappingBufferArray(UploadBuffer, [&](Point* input) {
		for (size_t i = 0; i < 3; ++i)
			input[i] = Rec[i];
	}, 0, 3);

	auto [VertexBuffer, re_VB] = Device++.CreateBufferVertexCommitted(sizeof(Point) * 3, 0, 0);

	auto [UpdateCommandList, re_UC] = Device++.CreateGraphicCommandList(Allocator, *CommandListType::Direct);

	UpdateCommandList->CopyResource(VertexBuffer, UploadBuffer);

	UpdateCommandList->Close();

	Dx12::CommandList* const CL[] = { UpdateCommandList };

	Queue->ExecuteCommandLists(0, CL);

	while (!exit)
	{

		auto [BBResource, re1] = Dx12::GetBuffer(SwapChain, current_buffer);
		Device++.CreateRenderTargetView2D(BBResource, DescriptorSize.RTVOffset(RTDescHead, 0));
		
		auto [CommandList, re_l] = Device++.CreateGraphicCommandList(Allocator, *CommandListType::Direct);

		auto RTHandle = RTDescHead->GetCPUDescriptorHandleForHeapStart();
		auto DTHandle = DTDescHead->GetCPUDescriptorHandleForHeapStart();

		FLOAT Color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		
		Dx12::ResourceBarrier tem[2] = {
			Dx12::TransitionState(BBResource, *ResourceState::Present, *ResourceState::RenderTarget),
			Dx12::TransitionState(DTResource, *ResourceState::Common, *ResourceState::DepthWrite)
		};

		CommandList->ResourceBarrier(2, tem);
		CommandList->ClearRenderTargetView(RTDescHead->GetCPUDescriptorHandleForHeapStart(), Color, 0, nullptr);
		CommandList->ClearDepthStencilView(DTDescHead->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0, 0, 0, nullptr);
		CommandList->OMSetRenderTargets(1, &RTHandle, false, &DTHandle);
		Dx12::SwapTransitionState(2, tem);
		CommandList->ResourceBarrier(2, tem);
		CommandList->Close();
		Dx12::CommandList* const CommandlListArray = CommandList;
		Queue->ExecuteCommandLists(1, &CommandlListArray);
		SwapChain->Present(1, 0);
		Queue->Signal(Fence, 1);

		D3D12_DESCRIPTOR_RANGE range{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0,  D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND };

		D3D12_ROOT_PARAMETER tempara;
		tempara.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		tempara.Descriptor = D3D12_ROOT_DESCRIPTOR{ 0, 0 };
		tempara.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_ROOT_PARAMETER RootParameter[] = {
			tempara
		};

		D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		RootSignatureDesc.NumParameters = 1;
		RootSignatureDesc.pParameters = RootParameter;
		RootSignatureDesc.NumStaticSamplers = 0;
		RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

		Dx12::BlobPtr Data, Error;

		HRESULT re = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, Data(), Error());

		if (Error)
		{
			const char* buffer = (const char*)Error->GetBufferPointer();
			__debugbreak();
		}

		//CommandList->SetGraphicsRootDescriptorTable();

		//Device

		while (Fence->GetCompletedValue() != 1)
		{
			std::cout << Fence->GetCompletedValue() << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
		}
		
		current_buffer += 1;
		current_buffer = current_buffer % 2;
		Allocator->Reset();
		CommandList->Reset(Allocator, nullptr);
		Fence->Signal(0);
		std::cout << "down" << std::endl;
		MSG msg;
		while (form.pook_event(msg))
		{
			if (msg.message == WM_CLOSE)
			{
				exit = true;
				break;
			}
		}
	}
	*/
	return 0;
}