# Normal map demo

Demo showcasing the usage of normal maps on a Sega Megadrive. Works by assigning palette entries to normal directions and modifying the colors
according to the angle between a directional light and the normals. By default the light direction spins randomly around the torus.
It's possible to manually control the direction and toggle the texture and light diffuse and specular components. Random spinning will resume automaticaly
after 3 seconds of inactivity.

[<p align="center"><img src="readmeresources/SampleImage.png" width="636" height="476"/></p>](https://www.youtube.com/embed/Zn50HsH6ntU)

## Tech Stack:
* [SGDK 2.11](https://github.com/Stephane-D/SGDK/releases/tag/v2.11) for handling all Sega Megadrive low-level management
* Visual Studio Code 
* Genesis Code extension for Visual Studio Code
* [Gens KMod](http://gendev.spritesmind.net/page-gensK.html) and [Blastem-mdp](https://github.com/Tails8521/blastem/releases/tag/1.0.0) for testing and debugging
* [md-profiler](https://github.com/Tails8521/md-profiler) for profiling
* Aseprite for pixel art edition and image conversion debugging
* Gimp for additional image editing
* Blender for sample torus modeling + normal and albdedo textures rendering
* A custom conversion tool that's not yet ready for public usage

## Special thanks
I'd like to thank the following people for making this project possible
* Stephane Dallongeville for the creation of SGDK
* Kaze Emanuar for suggesting encoding normal map in palletized image in [this video](https://www.youtube.com/watch?v=xIUkoUEMf_g&t=462s)

## Additional credits
This demo uses the following 3rd party resources:
* [**Arial font**](https://learn.microsoft.com/en-us/typography/font-list/arial)
* [**Ink Free font**](https://learn.microsoft.com/da-dk/typography/font-list/ink-free)
* **203344__klemmy__123-taiko-open.wav**: Taiko beat sound.
    * Obtained from freesounds.org, unmodified
    * Attibution text: [123-Taiko-Open.wav](https://freesound.org/people/klemmy/sounds/203344/) by [klemmy](https://freesound.org/people/klemmy/) | License: [Attribution 3.0](http://creativecommons.org/licenses/by/3.0/)
* **506450__kbeezy88__hhfunk1.wav**: Hi hat sound
    * Obtained from freesounds.org, unmodified
    * Attribution text: [HHFunk1.wav](https://freesound.org/people/kbeezy88/sounds/506450/) by [kbeezy88](https://freesound.org/people/kbeezy88/) | License: [Creative Commons 0](http://creativecommons.org/publicdomain/zero/1.0/)
