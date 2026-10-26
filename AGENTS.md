# OpenGL Mini Engine Instructions

## Scene View camera orbit

The Scene View camera orbits around `sceneFocusPoint` with three state values:

- `sceneOrbitYaw`
- `sceneOrbitPitch`
- `sceneOrbitDistance`

The camera position is calculated as:

```cpp
offset = vec3(
    cos(pitch) * sin(yaw),
    sin(pitch),
    cos(pitch) * cos(yaw)
);
cameraPosition = sceneFocusPoint + offset * sceneOrbitDistance;
```

Input semantics are part of the editor UX and must stay consistent:

- `Alt + left mouse` orbits the Scene View camera.
- Horizontal drag: `yaw -= MouseDelta.x * 0.35f`.
- Vertical drag: `pitch += MouseDelta.y * 0.3f`.
- `MouseDelta.y` is positive when the pointer moves down and negative when it
  moves up.
- Pitch is clamped to `[-85.0f, 85.0f]` degrees.

Do not flip the sign of either axis without an explicit user request. The
horizontal and vertical directions were adjusted independently based on manual
interaction feedback, so changing one must not silently change the other.

## Coordinate systems

- World axis colors match Unity conventions: X red, Y green, Z blue.
- The global orientation gizmo belongs in the upper-right corner of Scene View.
- The selected object uses a separate transform gizmo at its world position.
- `W`, `E`, and `R` select Move, Rotate, and Scale respectively.

## Default skybox

- The renderer uses a procedural sky pass driven by the inverse
  view-projection matrix.
- Scene View and Game View share the same gradient sky and sun glow.
- Do not replace it with a flat clear color or require an external skybox
  texture for the default project.

## Default scene and presets

- Startup creates an empty scene with only the Scene root, Main Camera,
  Directional Light, and Ambient Light.
- The grass field must be loaded from `assets/Assets/Scenes/GrassDemo.scene`,
  not created during renderer initialization.
- Scene assets open by double-clicking the `.scene` file in the Content Browser.
- Keep the Scene hierarchy free of a Grass Field node until that preset has been
  loaded.

## Object creation

- The Scene Hierarchy provides a `+ Create` button and a right-click context
  menu.
- Supported creation types are Empty, 2D Quad, 3D Cube, Sphere, and Plane.
- Newly created objects must be added to the scene, hierarchy, selection model,
  and inspector. Do not make menu entries that only change UI state.
- Use English default object names (`GameObject`, `Quad`, `Cube`, `Sphere`,
  `Plane`) to match Unity conventions.
- Newly created primitives use `WhiteMaterial` with a gray base color and
  directional lighting, not the checkerboard or default-texture material.
- Selected Mesh objects are drawn with an orange silhouette outline in Scene
  View only. Game View must not include the editor selection outline.

## Editor views

- Scene and Game are docked tabs.
- Dragging a tab outward creates a floating view.
- Floating views can be moved by their title bar and returned with Dock.
- Default scene object names `Main Camera`, `Directional Light`, and
  `Ambient Light` remain in English in both UI languages.

## Inspector layout

- The Inspector starts with a colored object icon, object name, and type.
- Properties are grouped into collapsible sections with aligned two-column
  tables.
- Vector fields use the X/Y/Z colors red, green, and blue.
- Transform sections must always expose Position, Rotation, and Scale. Do not
  reduce editable object transforms to position-only or Y-rotation-only rows.
- Mesh-producing objects include a gray Lit material preview.
- Keep property labels muted and controls visually dominant, following the
  Unity Inspector hierarchy rather than using a flat list of widgets.

## Asset browser

- Keep the `ASSETS` header, search field, refresh control, and list/grid switch
  in the top toolbar.
- The lower browser uses a Unity-style left sidebar with the Project folder
  tree and a Packages section. Do not add a Favorites section.
- The project asset root is `assets/Assets` with `Scenes`, `Materials`, `Models`,
  and `Textures` folders; breadcrumbs start at `Assets`.
- Engine-owned resources such as `assets/shaders` stay outside the project asset
  root and must not appear in the asset browser.
- Folder and file rows use distinct generated icons; do not rely on the
  operating system file-association icons.
- The footer must show asset count, selected asset path, and the zoom control.

## Unity source policy

- Unity Editor and engine source are not open source. Do not copy code from
  `UnityCsReference` or other Unity source-available repositories.
- Reimplement editor behavior independently from public documentation,
  screenshots, and observed user workflows.

## Module boundaries

- Keep `OpenGL.cpp` focused on demo composition, runtime state, scene setup,
  simulation updates, initialization, and the main loop.
- Put editor UI in `editor/`: layout, hierarchy, viewport, inspector, and asset
  browser each have separate implementation files.
- Shared editor-facing state and cross-module declarations live in
  `editor/EditorShared.h`. Do not add unrelated global declarations there.
- Avoid moving UI code back into `OpenGL.cpp` merely because it needs existing
  editor state; extend the shared interface instead.
- Do not reintroduce disabled `#if 0` implementations. Delete superseded code.

