// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 *
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 *
 * This example shows the implementation of a standalone process peripheral.
 * This process gets three arguments passed to it by the simulation side:
 *
 * - main socket: this socket is used for requests initiated by the emulator
 * - irq socket: this socket is used for requests initiated by this process
 * - address: the IP address of the emulator
 *
 * By connecting back to the emulator using the supplied sockets, we can then
 * communicate with the emulator side and get messages sent to us whenever
 * firmware reads or writes some memory assigned to this particular device.
 *
 * This example is intentionally kept raw and very simple - using raw unix
 * sockets. This is to show the core concept without any dependencies. Remember
 * that sockets are byte streams - so ideally there should be a packet protocol
 * on top. Here it works anyway because we are running very close to the
 * simulation and connections never break.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <cstdlib>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <renode/renode.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>

enum plant_register {
	PLANT_REG_U = 0,
	PLANT_REG_Y = 4,
	PLANT_REG_COMPUTE = 8,
	PLANT_REG_R = 12,
};

#define ADIM 2
#define RDIM 1
#define YDIM 1

#include <lqi-design.h>

int main(int argc, char **argv)
{
	if (argc != 4) {
		printf("Usage: %s <mainPort> <irqPort> <address>\n", argv[0]);
		return 1;
	}
	int mainPort = atoi(argv[1]);
	int irqPort = atoi(argv[2]);
	const char *address = argv[3];
	struct renode *renode = renode_new();

	printf("Connecting to %s %d %d\n", address, mainPort, irqPort);

	if (renode_connect(renode, mainPort, irqPort, address) != 0) {
		fprintf(stderr, "Could not connect to renode (IP: %s)\n", address);
		return -1;
	}

	printf("Connected to %s %d %d\n", address, mainPort, irqPort);
	struct renode_packet req;
	struct renode_packet res;

	float x[] = { 0.0, 0.0 };
	float u[] = { 0.0 };
	float r[] = { 10.0 };
	float y[] = { 0.0 };

	// Setup SDL
	// (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
	// d updating to latest version of SDL is recommended!)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
							 SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window *window = SDL_CreateWindow("Controller example", SDL_WINDOWPOS_CENTERED,
					      SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool done = false;
	while (!done) {
		if (renode_wait_request(renode, &req) != 0) {
			fprintf(stderr, "Failed to receive packet\n");
			break;
		}

		res.type = MSG_TYPE_ERROR;

		switch (req.type) {
		case MSG_TYPE_HANDSHAKE:
			res.type = MSG_TYPE_HANDSHAKE;
			break;
		case MSG_TYPE_WRITE:
			if (req.addr == PLANT_REG_U) {
				memcpy(&u[0], &req.value, sizeof(u[0]));
				res.type = MSG_TYPE_OK;
			} else if (req.addr == PLANT_REG_COMPUTE) {
				float Ax[] = { A[0] * x[0] + A[1] * x[1],
					       A[2] * x[0] + A[3] * x[1] };
				float Bu[] = { B[0] * u[0], B[1] * u[0] };
				float Cx[] = { C[0] * x[0] + C[1] * x[1] };

				x[0] = Ax[0] + Bu[0];
				x[1] = Ax[1] + Bu[1];
				y[0] = Cx[0];
				res.type = MSG_TYPE_OK;
			}
			break;
		case MSG_TYPE_READ:
			if (req.addr == PLANT_REG_Y) {
				memcpy(&res.value, &y[0], sizeof(res.value));
				res.type = MSG_TYPE_OK;
			} else if (req.addr == PLANT_REG_R) {
				memcpy(&res.value, &r[0], sizeof(res.value));
				res.type = MSG_TYPE_OK;
			}
			break;
		case MSG_TYPE_DISCONNECT:
			exit(0);
		}

		if (renode_send_response(renode, &res) != 0) {
			fprintf(stderr, "Failed to send packet");
			break;
		}

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT &&
			    event.window.event == SDL_WINDOWEVENT_CLOSE &&
			    event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Plant", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Text("This is the plant controlled by firmware");
		ImGui::PushItemWidth(400);
		ImGui::SliderFloat("Plant output (y)", &y[0], -25.0f, 25.0f);
		ImGui::SliderFloat("Plant reference (r)", &r[0], -25.0f, 25.0f);

		ImGui::End();

		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
			     clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	renode_disconnect(renode);
	renode_free(&renode);
}
