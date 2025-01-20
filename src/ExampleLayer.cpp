#include "ExampleLayer.h"

#include <imgui_internal.h>

ExampleLayer::ExampleLayer() = default;


void ExampleLayer::OnUpdate()
{
}

void ExampleLayer::OnEvent(AE::Events::Event& event)
{
}

void ExampleLayer::OnImGuiRender()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    if (!m_DockSpaceInitialized)
    {
        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);

        auto dockIdMain = dockspaceId;
        auto dockIdRight = ImGui::DockBuilderSplitNode(dockIdMain, ImGuiDir_Right, 0.25f, nullptr, &dockIdMain);

        ImGui::DockBuilderDockWindow("Scene inspector", dockIdRight);

        ImGui::DockBuilderFinish(dockspaceId);

        m_DockSpaceInitialized = true;
    }

    // Create the right-side docked window
    ImGui::Begin("Scene inspector");
    ImGui::Text("This is where the scene inspector will go...");
    if (ImGui::Button("Button"))
    {
        AE_INFO("Button clicked!");
    }
    ImGui::End();

    ImGui::End();
}

void ExampleLayer::OnAttach()
{
}
