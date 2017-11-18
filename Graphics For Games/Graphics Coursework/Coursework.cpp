#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "Renderer.h"
int main() {
	Window w("Graphics Coursework", 1208, 720, false); //This is all boring win32 window creation stuff!
	if (!w.HasInitialised()) {
		return -1;
	}

	srand((unsigned int)w.GetTimer()->GetMS() * 1000.0f);

	Renderer renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD7)) {
			renderer.ToggleSobel();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD8)) {
			renderer.Togglecontrast();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_TAB)) {
			renderer.ResetScenes();
			renderer.ToggleSplitScreen();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_PAUSE)) {
			renderer.TogglePause();
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT)) {
			renderer.PreviousScene();
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT)) {
			renderer.NextScene();
		}

		if (renderer.IsSplitScreen()) {
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD0)) {
				renderer.SetControlledScene(99);
			}
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD1)) {
				renderer.SetControlledScene(0);
			}
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD2)) {
				renderer.SetControlledScene(1);
			}
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD3)) {
				renderer.SetControlledScene(2);
			}
		}
	}
	return 0;
}