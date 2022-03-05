#include "SDL_events.h"
#include "gui/imguivulkan.h"
#include "display/imaging/imagedata.h"
#include "../ext/imgui/imgui_impl_vulkan.h"

using namespace Boiler;

void ImGuiVulkan::initialize()
{
	logger.log("Initializing");
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();

	int width, height;
	unsigned char *pixels = nullptr;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);


	ImageData imageData(pixels, Size(width, height), 4, true);
	Material material = renderer.createMaterial();
	material.baseTexture = renderer.loadTexture(imageData);
}

void ImGuiVulkan::cleanup() const
{
	logger.log("Shutting down");
	ImGui::DestroyContext();
}

void ImGuiVulkan::processEvent(const SDL_Event &event) const
{
}

void ImGuiVulkan::preRender() const
{
	ImGui::NewFrame();
}

void ImGuiVulkan::render() const
{
	ImGui::ShowDemoWindow();
	ImGui::Render();

	ImDrawData *drawData = ImGui::GetDrawData();

	// TODO: Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	// TODO: Setup viewport covering draw_data->DisplayPos to draw_data->DisplayPos + draw_data->DisplaySize
	// TODO: Setup orthographic projection matrix cover draw_data->DisplayPos to draw_data->DisplayPos + draw_data->DisplaySize
	// TODO: Setup shader: vertex { float2 pos, float2 uv, u32 color }, fragment shader sample color from 1 texture, multiply by vertex color.
	ImVec2 clip_off = drawData->DisplayPos;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;  // vertex buffer generated by Dear ImGui
		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;   // index buffer generated by Dear ImGui
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				// Project scissor/clipping rectangles into framebuffer space
				ImVec2 clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
				ImVec2 clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
				if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
					continue;

				// We are using scissoring to clip some objects. All low-level graphics API should support it.
				// - If your engine doesn't support scissoring yet, you may ignore this at first. You will get some small glitches
				//   (some elements visible outside their bounds) but you can fix that once everything else works!
				// - Clipping coordinates are provided in imgui coordinates space:
				//   - For a given viewport, draw_data->DisplayPos == viewport->Pos and draw_data->DisplaySize == viewport->Size
				//   - In a single viewport application, draw_data->DisplayPos == (0,0) and draw_data->DisplaySize == io.DisplaySize, but always use GetMainViewport()->Pos/Size instead of hardcoding those values.
				//   - In the interest of supporting multi-viewport applications (see 'docking' branch on github),
				//     always subtract draw_data->DisplayPos from clipping bounds to convert them to your viewport space.
				// - Note that pcmd->ClipRect contains Min+Max bounds. Some graphics API may use Min+Max, other may use Min+Size (size being Max-Min)
				//TODO: MyEngineSetScissor(clip_min.x, clip_min.y, clip_max.x, clip_max.y);

				// The texture for the draw call is specified by pcmd->GetTexID().
				// The vast majority of draw calls will use the Dear ImGui texture atlas, which value you have set yourself during initialization.
				//MyEngineBindTexture((MyTexture*)pcmd->GetTexID());

				// Render 'pcmd->ElemCount/3' indexed triangles.
				// By default the indices ImDrawIdx are 16-bit, you can change them to 32-bit in imconfig.h if your engine doesn't support 16-bit indices.
				//MyEngineDrawIndexedTriangles(pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer + pcmd->IdxOffset, vtx_buffer, pcmd->VtxOffset);
			}
		}
	}
}
