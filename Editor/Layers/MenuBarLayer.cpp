#include "Layers/MenuBarLayer.h"
#include "Core/Window.h"
#include "Core/Logger.h"
#include "GameFramework/World.h"
#include "imgui.h"
#include <Windows.h>

namespace Mile
{
   namespace Editor
   {
      MenuBarLayer::MenuBarLayer(Context* context) :
         m_world(nullptr),
         m_window(nullptr),
         Layer(context, TEXT("MenuBarLayer"))
      {
      }

      void MenuBarLayer::OnIMGUIRender()
      {
         if (ImGui::BeginMainMenuBar())
         {
            if (ImGui::BeginMenu("File"))
            {
               FileMenu();
               ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
         }
      }

      void MenuBarLayer::FileMenu()
      {
         constexpr unsigned int MAX_FILE_LENGTH = 260;
         OPENFILENAME openFileName;
         TCHAR szFile[MAX_FILE_LENGTH] = { 0 };
         ZeroMemory(&openFileName, sizeof(openFileName));

         if (ImGui::MenuItem("New World"))
         {
            /* New World **/
            if (m_world != nullptr)
            {
               m_world->Clear();
            }
         }
         if (ImGui::MenuItem("Open World", "Ctrl+O"))
         {
            /* Open World **/
            if (m_window != nullptr)
            {
               openFileName.lStructSize = sizeof(openFileName);
               openFileName.hwndOwner = (HWND)m_window->GetHandle();
               openFileName.lpstrTitle = TEXT("Select world file to open.");
               openFileName.lpstrFile = szFile;
               openFileName.nMaxFile = sizeof(szFile);
               openFileName.lpstrFilter = TEXT("World\0*.world\0All\0*.*\0");
               openFileName.nFilterIndex = 1;
               openFileName.nMaxFileTitle = 0;
               openFileName.lpstrInitialDir = NULL;
               openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

               if (GetOpenFileName(&openFileName) == TRUE)
               {
                  String filePath = String(openFileName.lpstrFile);
                  MELog(GetContext(), TEXT("MenuBarLayer"), ELogType::MESSAGE, String(TEXT("Open world from ")) + filePath);
                  if (m_world != nullptr)
                  {
                     m_world->LoadFrom(filePath);
                  }
               }
            }
         }

         bool failedToSave = false;
         if (ImGui::MenuItem("Save World", "Ctrl+S"))
         {
            /* Save World **/
            if (m_world != nullptr)
            {
               failedToSave = m_world->Save();
            }
         }
         if (ImGui::MenuItem("Save World As..") || failedToSave)
         {
            /* Save World As .. **/
            if (m_window != nullptr)
            {
               openFileName.lStructSize = sizeof(openFileName);
               openFileName.hwndOwner = (HWND)m_window->GetHandle();
               openFileName.lpstrTitle = TEXT("Save As..");
               openFileName.lpstrFile = szFile;
               openFileName.nMaxFile = sizeof(szFile);
               openFileName.lpstrFilter = TEXT("World\0*.world\0All\0*.*\0");
               openFileName.nFilterIndex = 0;
               openFileName.nMaxFileTitle = 0;
               openFileName.lpstrInitialDir = NULL;
               openFileName.Flags = OFN_NOCHANGEDIR;

               if (GetSaveFileName(&openFileName) == TRUE)
               {
                  String filePath = String(openFileName.lpstrFile) + String(TEXT(".")) + String(openFileName.lpstrFilter);
                  MELog(GetContext(), TEXT("MenuBarLayer"), ELogType::MESSAGE, String(TEXT("Save world as ")) + filePath);
                  if (m_world != nullptr)
                  {
                     m_world->SaveTo(filePath);
                  }
               }
            }
         }
      }
   }
}