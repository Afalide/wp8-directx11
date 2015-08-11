
### Directx11 on Windows Phone 8

This is a POC project inspired by the really nice dx11 tutorials of [rastertek.com](http://www.rastertek.com).

This project is an adaptation of the Desktop DX11 programming tutorial with some changes to fit correctly on the Windows Phone 8 native runtime, plus some small bug/memleaks fixes.

As the title suggests, this project uses features of the DX11 SDK on Windows Phone (8 and 8.1) devices, using C++ and targeting ARM devices (or x86 for emulators).

----

### Why Windows Phone?

As Android and Apple devices are kind of eating the whole smartphone market, Windows Phone devices seems in trouble (source: IDC, May 2015, http://www.idc.com/prodserv/smartphone-os-market-share.jsp)

Nonetheless, Windows Phone is a very skilled OS, much more intuitive than it's desktop counterpart, which is why users should give it a go.

This project is indeed a tentative to demonstrate the bunch of 3D-related features these devices are able to provide. 

----

### Why C++?

Because 3D.

----

### Why 3D?

3D is a nice way to embellish your device. You get a wonderful world right behind your tiny screen. 3D is everywhere, even if 2D interfaces (which are faked 3D animations, you should see that on Windows Phone since version 8.0, "metro style", have a look at their buttons when you click on them).

----

### So why C++, again?

Things goes fast in 3D, here are some reasons why C(++) is suited to do the job:

- **Memory management is paramount.** When someone start saying "yeah but with today huge RAM, we can afford coding with our feet" you can safely ignore them. Tt's true that **desktops** or even **laptops** are well equipped with more and more memory, but that's not (yet) true for smartphones. A wide range of low-costs phones (which are targetted by this project) can't afford more than 512 MB of RAM, which is why every byte counts when you try to load a hundred of models worth thousands of vertices, each one with it's corresponding 3MB texture.

- **You need some speed.** You can do C# if you want, but be aware that this has a non-negligible cost on phone devices. If you reject C++ because of it's complexity, you may have chosen the wrong way to start 3D development. C++ is not complex, pointers are not your ennemies, it's okay to use libs that handles complexity for you. It's even more okay to use C++11 smart pointer to avoid memory leaks...

- **DirectX11 is only available for C++.** Some limited C# bindings exists though, but this requires additional dependencies. Also even using a managed language like C#, you won't be freed from COM objects management which are the base of the DirectX API.
