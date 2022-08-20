// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
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
#include <pthread.h>

#include <renode/renode.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "virtual_device.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

struct device_state {
	pthread_mutex_t lock;
	struct virtual_device data;
};

struct application {
	struct renode *renode;
	struct device_state state;
	bool done;
};

void *_communication_thread(void *data)
{
	struct application *self = (struct application *)data;
	while (true) {
		struct renode *renode = self->renode;
		struct renode_packet req;
		struct renode_packet res;

		if (renode_wait_request(renode, &req) != 0) {
			fprintf(stderr, "Failed to receive packet\n");
			break;
		}

		pthread_mutex_lock(&self->state.lock);

		uint8_t *data = (uint8_t *)&self->state.data;
		res.type = MSG_TYPE_ERROR;

		switch (req.type) {
		case MSG_TYPE_HANDSHAKE:
			res.type = MSG_TYPE_HANDSHAKE;
			break;
		case MSG_TYPE_WRITE:
			memcpy(data + req.addr, &req.value, sizeof(uint32_t));
			res.type = MSG_TYPE_OK;
			break;
		case MSG_TYPE_READ:
			memcpy(&res.value, data + req.addr, sizeof(uint32_t));
			res.type = MSG_TYPE_OK;
			// interrupt flags are reset when they are read
			if (req.addr == offsetof(struct virtual_device, INTF)) {
				self->state.data.INTF = 0;
			}
			break;
		case MSG_TYPE_DISCONNECT:
			exit(0);
			//pthread_mutex_unlock(&self->state.lock);
			//self->done = true;
			return NULL;
		}

		pthread_mutex_unlock(&self->state.lock);

		if (renode_send_response(renode, &res) != 0) {
			fprintf(stderr, "Failed to send packet");
			break;
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{
	if (argc != 4) {
		printf("Usage: %s <mainPort> <irqPort> <address>\n", argv[0]);
		return 1;
	}
	int mainPort = atoi(argv[1]);
	int irqPort = atoi(argv[2]);
	const char *address = argv[3];
	struct application app;
	struct application *self = &app;

	memset(&app, 0, sizeof(app));

	pthread_mutex_init(&self->state.lock, NULL);
	self->renode = renode_new();

	printf("Connecting to %s %d %d\n", address, mainPort, irqPort);

	if (renode_connect(self->renode, mainPort, irqPort, address) != 0) {
		fprintf(stderr, "Could not connect to renode (IP: %s)\n", address);
		return -1;
	}

	printf("Connected to %s %d %d\n", address, mainPort, irqPort);
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
	ImPlot::CreateContext();
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

	pthread_t thread;
	pthread_create(&thread, NULL, _communication_thread, self);
	while (!self->done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				self->done = true;
			if (event.type == SDL_WINDOWEVENT &&
			    event.window.event == SDL_WINDOWEVENT_CLOSE &&
			    event.window.windowID == SDL_GetWindowID(window))
				self->done = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		pthread_mutex_lock(&self->state.lock);

		ImGui::Begin("Plant", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Text("This is the plant controlled by firmware");

		if (ImGui::Button("Interrupt Button 1")) {
			self->state.data.INTF |= (1 << 0);
			renode_irq_notify(self->renode);
		}

		if (ImGui::Button("Interrupt Button 2")) {
			self->state.data.INTF |= (1 << 1);
			renode_irq_notify(self->renode);
		}

		ImGui::PushItemWidth(400);
		ImGui::PopItemWidth();

		ImGui::End();

		pthread_mutex_unlock(&self->state.lock);

		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
			     clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	renode_disconnect(self->renode);
	renode_free(&self->renode);

	ImPlot::DestroyContext();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
