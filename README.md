
### Directx11 on Windows Phone 8

This is a POC project inspired by the really nice dx11 tutorials of rastertek.com.

This project is an adaptation of the Desktop DX11 programming tutorial with some changes to fit correctly on the Windows Phone 8 native runtime, plus some small bug/memleaks fixes.

As the title suggests, this project uses features of the DX11 SDK on Windows Phone (8 and 8.1) devices, using C++ and targeting ARM devices (or x86 for emulators).

----

### Why Windows Phone?

As Android and Apple devices are kind of eating the whole smartphone market, Windows Phone devices seems in trouble (source: IDC, May 2015, http://www.idc.com/prodserv/smartphone-os-market-share.jsp)

However, I rather trust what I have in hands, a decent Lumia 620, worth 150€ back when I bought it, which survived both Windows 8.0 and Windows 8.1 updates, and still runs fast and smoothly, with a 60fps interface, which is what I like on smartphones.

Next I look at the content, and yes, the Windows Phone Store is desperately empty, with a very few choices if you obviously compare with the Play Store.

Nonetheless, Windows Phone is a very skilled OS, with a wide variety of functionnalities easy to use despite the poor quality of the MSDN doc.

----

### Why C++?

Because 3D.

----

### Why 3D?

3D is a nice way to embelish your device. You get a wonderful world right behind your tiny screen. 3D is everywhere, even if 2D interfaces (which are faked 3D animations, you should see that on Windows Phone since version 8.0, "metro style", have a look at their buttons when you click on them).

----

### So why C++, again?

Things goes fast in 3D, here are some reasons why C(++) is suited to do the job:

- **Memory management is paramount.** When someone start saying "yeah but with today huge RAM, we can afford coding with our feet" you can safely ignore them. Tt's true that **desktops** or even **laptops** are well equipped with more and more memory, but that's not (yet) true for smartphones. My Lumia has a timid 256MB chipset, you can trust me that every byte counts when you try to load 100 models, each one with it's 3MB texture.

- **Speed is paramount.** You can do C# if you want, but be aware that this has a non-negligible cost on phone devices. If you reject C++ because of it's complexity, you may have chosen the wrong way to start 3D development. C++ is not complex, pointers are not your ennemies, it's okay to use libs that handles complexity for you. It's even more okay to use C++11 smart pointer to avoid memory leaks...

- **DirectX11 is only available for C++.** Some limited C# bindings exists though, but this requires additional dependencies. Also even using a managed language like C#, you won't be freed from COM objects management which are the base of the DirectX API.
