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
		
		// ------------------------------------------------------------------------------------------------------------------
		// POST PROCESSING CONTROLS
		// ------------------------------------------------------------------------------------------------------------------
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD7) || (Window::GetKeyboard()->KeyTriggered(KEYBOARD_7))) {
			renderer.ToggleSobel();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD8) || (Window::GetKeyboard()->KeyTriggered(KEYBOARD_8))) {
			renderer.Togglecontrast();
		}

		// ------------------------------------------------------------------------------------------------------------------
		// SCENE MANAGEMENT CONTROLS
		// ------------------------------------------------------------------------------------------------------------------

		// Toggle splitscreen
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_TAB)) {
			renderer.ResetScenes();
			renderer.ToggleSplitScreen();
		}

		// Toggle automatic scene switching
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_PAUSE)) {
			renderer.TogglePause();
		}

		// Go to previous scene
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT)) {
			renderer.PreviousScene();
		}
		
		// Go to next scene
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT)) {
			renderer.NextScene();
		}

		// Control all scenes
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD0) || (Window::GetKeyboard()->KeyTriggered(KEYBOARD_0))) {
			renderer.SetControlledScene(99);
		}

		// Control/Switch to Scene 1
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD1) || (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1))) {
			renderer.SetControlledScene(0);
			renderer.SetActiveScene(0);
		}

		// Control/Switch to Scene 2
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD2) || (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2))) {
			renderer.SetControlledScene(1);
			renderer.SetActiveScene(1);
		}

		// Control/Switch to Scene 3
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD3) || (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3))) {
			renderer.SetControlledScene(2);
			renderer.SetActiveScene(2);
		}

		// Reset Scene
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R)) {
			renderer.ResetCurrent();
		}

		// ------------------------------------------------------------------------------------------------------------------
		// HUD CONTROLS
		// ------------------------------------------------------------------------------------------------------------------

		// Show Controls on Screen
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C)) {
			renderer.ToggleControls();
		}

		// Show Scene Features on Screen
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F)) {
			renderer.ToggleFeatures();
		}
	}
	return 0;
}