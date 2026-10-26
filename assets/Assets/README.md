# Project Assets

This is the project-facing asset root shown in the editor. Engine-owned shader
files remain outside this folder and are hidden from the asset browser.

The project media files are intentionally ignored by Git. The public repository
contains engine code and shaders, but no model or texture assets.

To run the grass demonstration locally, place these files here:

```text
assets/Assets/Models/grassNew.obj
assets/Assets/Textures/GRASS.PNG
assets/Assets/Textures/grassMask.png
assets/Assets/Textures/CLOUD.PNG
assets/Assets/Textures/defaultTexture.jpg
```

Then double-click `assets/Assets/Scenes/GrassDemo.scene` in the Content Browser.
The editor starts with an empty scene and loads the grass field from this preset.

Only use assets that you created or are permitted to use in local recordings.

