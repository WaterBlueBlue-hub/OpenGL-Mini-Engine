# OpenGL Mini Engine

A compact OpenGL rendering demo with an instanced grass field, wind animation,
a simple cloud color layer, day preset lighting, camera controls, an ImGui
control panel, and an offscreen framebuffer presentation pass.

This is a focused portfolio demo built on a small educational rendering
framework. It is not presented as a complete game engine.

## Current Features

- Editor-style toolbar, scene hierarchy, viewport, inspector, and content browser
- Unity-style Scene/Game tabs with independent cameras
- Draggable floating views, dock-back action, and a foldable viewport
- Unity-style camera orientation gizmo in the upper-right of the Scene View
- Selected-object transform gizmo for Move, Rotate, and Scale tools
- Unity-style Play, Pause, and single-frame Step controls
- Chinese interface by default, with a Chinese/English toggle
- Selectable scene nodes with object-specific property panels
- Live asset tree loaded from the runtime asset directories
- Create Empty, 2D Quad, 3D Cube, Sphere, and Plane objects from the hierarchy
- Lit gray default material and orange selection outlines in Scene View
- Unity-style inspector with object header, collapsible sections, aligned
  properties, axis-colored transform fields, and material preview
- Unity-style Assets browser with search, breadcrumbs, list/grid modes,
  Project folder sidebar, typed file icons, selection details, and zoom control
- OpenGL 3.3 core renderer
- Procedural default skybox with horizon gradient and sun glow
- Empty startup scene with a reusable `Assets/Scenes/GrassDemo.scene` preset
- Scene hierarchy and camera controls
- GPU-instanced grass rendering
- Wind and cloud color animation
- Morning, noon, dusk, and night lighting presets
- Automatic day-cycle transition
- Perspective and orthographic camera modes
- Offscreen render target resized with the window
- Runtime control panel

## Controls

| Input | Action |
| --- | --- |
| `Alt` + left mouse drag | Orbit the Scene View camera |
| Middle mouse drag | Pan the Scene View camera |
| Mouse wheel | Zoom the Scene View camera |
| Left click | Select an object in the Scene View |
| `W` / `E` / `R` | Move / rotate / scale tool |
| `F` | Focus the Scene View on the selected object |
| Play button | Start or resume simulation time |
| Pause button | Freeze simulation time |
| Step button | Advance exactly one `1/60 s` simulation frame |
| Top UI buttons | Change projection, lighting, and language |

Drag the `Scene` or `Game` tab away from the tab strip to create a floating
window. Floating views can be moved by their title bar, collapsed with the
window arrow, or returned with the `Dock` button.

## Build

Requirements:

- Windows 10 or newer
- Visual Studio 2022 or newer with the Desktop C++ workload
- CMake 3.20 or newer

```powershell
cmake --preset vs2026
cmake --build --preset vs2026-release --target MiniEngine --parallel
```

The executable is generated under `out/build/vs2026/Release/`.

Opening the repository folder directly in Visual Studio is also supported.

## Project Structure

- `OpenGL.cpp`: demo composition, simulation updates, scene setup, startup, and main loop
- `editor/`: editor context, layout/toolbar, hierarchy, viewport, inspector, and asset browser
- `application/`: GLFW window, input plumbing, and cameras
- `glframework/`: renderer, scene graph, geometry, materials, textures, and lights

The entry file is intentionally kept below roughly 1,000 lines. New editor
panels belong in `editor/` rather than being added back to `OpenGL.cpp`.

Chinese text uses an installed Windows CJK font (`Microsoft YaHei`, SimHei,
SimSun, or DengXian). No font file is distributed with the repository.

## Project Assets

Media assets are not distributed with this repository. To run the complete
grass scene, follow [assets/Assets/README.md](assets/Assets/README.md).
Open the grass demo by double-clicking `Assets/Scenes/GrassDemo.scene` in the
Content Browser.

Shaders remain in `assets/shaders/` because they are source code, not
third-party media.

## Development Note

This project was designed, integrated, and validated by its author. Codex with
DeepSeek V4 Flash was used as an assisted programming tool for implementation,
refactoring, and debugging.

The scene hierarchy, component separation, and lifecycle ideas are inspired by
Unity's public architecture documentation. Unity source code is not included.

See [REFERENCES.md](REFERENCES.md) for references and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for dependencies.

## Status

MVP portfolio demo. The next release step is manual visual QA, recording a
short demonstration video, and publishing a technical article.

