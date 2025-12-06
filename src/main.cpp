#define GL_SILENCE_DEPRECATION

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
#include <chrono> 
#include <cstdio> 
#include "../include/hanoi.h"

// Visualization Constants
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

struct GameState {
    std::vector<int> pegs[3];
};

void resetGameState(GameState& state, int n) {
    for(int i=0; i<3; i++) state.pegs[i].clear();
    for(int i=n; i>=1; i--) state.pegs[0].push_back(i);
}

int main(int, char**) {
    if (!glfwInit()) return 1;

    // GL+GLSL Versions
    const char* glsl_version = "#version 130"; 
#if defined(__APPLE__)
    glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hanoi Benchmark & Vis", NULL, NULL);
    if (window == NULL) return 1;
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark(); 
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.13f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    HanoiSolver solver;
    int numDisks = 5; 
    
    // Visualization State
    int currentStep = 0;
    bool autoPlay = false;
    float playSpeed = 0.1f; 
    float timer = 0.0f;
    bool useIterativeVis = false; // Toggle for Visualization tab
    GameState currentState;
    resetGameState(currentState, numDisks);

    // Benchmark State
    int benchMaxN = 20;
    bool useIterativeBench = false; // Toggle for Benchmark tab
    std::vector<BenchmarkResult> benchResults;
    char csvFilename[128] = "hanoi_results.csv";
    std::string statusMsg = "Ready";

    // --- MAIN LOOP ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float screenW = ImGui::GetIO().DisplaySize.x;
        float screenH = ImGui::GetIO().DisplaySize.y;
        float sidebarWidth = 350.0f; 

        // --- 1. LOGIC: Auto Play ---
        if (autoPlay && currentStep < (int)solver.moveHistory.size()) {
            timer += ImGui::GetIO().DeltaTime;
            if (timer >= playSpeed) {
                timer = 0.0f;
                Move m = solver.moveHistory[currentStep];
                if (!currentState.pegs[m.fromPeg].empty()) {
                    int disk = currentState.pegs[m.fromPeg].back();
                    currentState.pegs[m.fromPeg].pop_back();
                    currentState.pegs[m.toPeg].push_back(disk);
                }
                currentStep++;
            }
        }

        // --- 2. SIDEBAR GUI ---
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(sidebarWidth, screenH));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
        
        ImGui::Begin("Control Panel", NULL, window_flags);
        
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); 
        ImGui::Text("HANOI VISUALIZER"); 
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::BeginTabBar("Tabs")) {
            
            // [TAB 1] VISUALIZATION
            if (ImGui::BeginTabItem("Visualization")) {
                ImGui::Spacing();
                ImGui::TextWrapped("Configure and run the visual simulation.");
                ImGui::Spacing();

                ImGui::BeginChild("SimControls", ImVec2(0, 180), true);
                    ImGui::Text("Configuration");
                    
                    // --- CHANGED: InputInt instead of Slider to allow typing ---
                    // The '1' is the step size, '5' is the fast step size
                    ImGui::InputInt("Disks", &numDisks, 1, 5); 
                    if (numDisks < 1) numDisks = 1;
                    if (numDisks > 20) numDisks = 20; // Soft limit for visuals

                    // Reset if edited
                    if (ImGui::IsItemDeactivatedAfterEdit()) {
                        currentStep = 0; autoPlay = false; solver.reset();
                        resetGameState(currentState, numDisks);
                    }

                    // --- ADDED: Checkbox for Algorithm ---
                    ImGui::Spacing();
                    ImGui::Checkbox("Use Iterative Algorithm", &useIterativeVis);
                    ImGui::Spacing();

                    if (ImGui::Button("SOLVE", ImVec2(-1, 40))) { 
                        solver.reset();
                        auto start = std::chrono::high_resolution_clock::now();
                        
                        // Pass 'useIterativeVis' to solver
                        solver.solve(numDisks, 0, 2, 1, true, useIterativeVis); 
                        
                        auto end = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<double, std::milli> elapsed = end - start;
                        solver.lastExecutionTimeMs = elapsed.count();
                        
                        currentStep = 0;
                        resetGameState(currentState, numDisks);
                    }
                    
                    ImGui::Text("Total Moves: %zu", solver.moveHistory.size());
                ImGui::EndChild();

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Playback Controls");
                
                float buttonW = (ImGui::GetContentRegionAvail().x - 10) / 2;
                if (ImGui::Button("Step >", ImVec2(buttonW, 30)) && currentStep < (int)solver.moveHistory.size()) {
                    Move m = solver.moveHistory[currentStep];
                    if (!currentState.pegs[m.fromPeg].empty()) {
                        int disk = currentState.pegs[m.fromPeg].back();
                        currentState.pegs[m.fromPeg].pop_back();
                        currentState.pegs[m.toPeg].push_back(disk);
                    }
                    currentStep++;
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset", ImVec2(buttonW, 30))) {
                    currentStep = 0; autoPlay = false;
                    resetGameState(currentState, numDisks);
                }

                ImGui::Checkbox("Auto-Play", &autoPlay);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::SliderFloat("Speed", &playSpeed, 0.0f, 0.5f, "%.2fs");

                float progress = solver.moveHistory.empty() ? 0.0f : (float)currentStep / (float)solver.moveHistory.size();
                ImGui::ProgressBar(progress, ImVec2(-1, 0.0f));
                ImGui::Text("Step: %d / %zu", currentStep, solver.moveHistory.size());

                ImGui::EndTabItem();
            }

            // [TAB 2] BENCHMARK
            if (ImGui::BeginTabItem("Benchmark")) {
                ImGui::Spacing();
                ImGui::TextWrapped("Performance testing mode.");
                ImGui::Spacing();
                
                // --- CHANGED: InputInt here as well ---
                ImGui::InputInt("Max Disks", &benchMaxN, 1, 5);
                
                // --- ADDED: Checkbox for Algorithm in Benchmark ---
                ImGui::Checkbox("Use Iterative Algorithm", &useIterativeBench);
                ImGui::Spacing();

                if (ImGui::Button("RUN BENCHMARK", ImVec2(-1, 40))) {
                    benchResults.clear();
                    statusMsg = "Running...";
                    for (int i = 1; i <= benchMaxN; i++) {
                        HanoiSolver tempSolver;
                        auto start = std::chrono::high_resolution_clock::now();
                        
                        // Pass 'useIterativeBench' and false for recordMoves
                        tempSolver.solve(i, 0, 2, 1, false, useIterativeBench);
                        
                        auto end = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<double, std::milli> elapsed = end - start;
                        benchResults.push_back({i, elapsed.count()});
                    }
                    statusMsg = "Done!";
                }
                ImGui::TextColored(ImVec4(0,1,0,1), "%s", statusMsg.c_str());
                
                ImGui::BeginChild("TableScroll", ImVec2(0, 300), true);
                if (ImGui::BeginTable("result_table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                    ImGui::TableSetupColumn("N");
                    ImGui::TableSetupColumn("Time (ms)");
                    ImGui::TableHeadersRow();
                    for (auto& row : benchResults) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("%d", row.n);
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%.4f", row.timeMs);
                    }
                    ImGui::EndTable();
                }
                ImGui::EndChild();
                
                ImGui::InputText("File", csvFilename, sizeof(csvFilename));
                if(ImGui::Button("Export CSV", ImVec2(-1, 30))) {
                     if(HanoiSolver::saveToCSV(benchResults, csvFilename)) 
                        statusMsg = "Saved!";
                     else 
                        statusMsg = "Error saving.";
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End(); 

        // --- 3. GAME VISUALIZATION (Background) ---
        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        
        float gameAreaX = sidebarWidth;
        float gameAreaW = screenW - sidebarWidth;
        float centerX = gameAreaX + (gameAreaW / 2.0f);
        float floorY = screenH - 100.0f;
        float pegSpacing = gameAreaW / 3.5f; 
        
        draw->AddRectFilled(ImVec2(sidebarWidth, 0), ImVec2(screenW, floorY), IM_COL32(35, 35, 40, 255));
        draw->AddRectFilled(ImVec2(sidebarWidth, floorY), ImVec2(screenW, screenH), IM_COL32(50, 50, 55, 255));
        draw->AddLine(ImVec2(sidebarWidth, 0), ImVec2(sidebarWidth, screenH), IM_COL32(0,0,0,255), 2.0f);

        float firstPegX = centerX - pegSpacing;

        for (int i = 0; i < 3; i++) {
            float pegX = firstPegX + (i * pegSpacing);
            draw->AddRectFilled(ImVec2(pegX - 60, floorY), ImVec2(pegX + 60, floorY + 15), IM_COL32(80, 80, 80, 255), 4.0f);
            draw->AddRectFilled(ImVec2(pegX - 6, floorY - 300), ImVec2(pegX + 6, floorY), IM_COL32(100, 100, 100, 255));

            for (size_t j = 0; j < currentState.pegs[i].size(); j++) {
                int diskSize = currentState.pegs[i][j];
                float width = 40.0f + (diskSize * 22.0f);
                float height = 22.0f;
                float yBottom = floorY - (j * height) - 2; 
                float yTop = yBottom - height + 3; 

                ImU32 color = IM_COL32(80 + (diskSize * 10), 100, 240 - (diskSize * 10), 255);
                draw->AddRectFilled(ImVec2(pegX - width/2, yTop), ImVec2(pegX + width/2, yBottom), color, 8.0f);
                draw->AddRect(ImVec2(pegX - width/2, yTop), ImVec2(pegX + width/2, yBottom), IM_COL32(255,255,255,100), 8.0f, 0, 1.5f);
            }
            
            char label[2]; snprintf(label, 2, "%c", 'A' + i);
            draw->AddText(NULL, 30.0f, ImVec2(pegX - 10, floorY + 30), IM_COL32(150,150,150,255), label);
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.13f, 0.13f, 0.15f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
