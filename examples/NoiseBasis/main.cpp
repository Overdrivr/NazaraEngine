#include <Nazara/Core/File.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Noise/Perlin2D.hpp>
#include <iostream>
#include <limits>

int main()
{
	NzInitializer<NzUtility> utility;
	if (!utility)
	{
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// In this example, let's fill an image with perlin noise and save it to the disk

    // Create the perlin2d generator with default parameter and an RGB8 512*512 pixels image
	NzPerlin2D perlin2d;
	NzImage image(nzImageType_2D,nzPixelFormat_RGB8,512,512);
	NzColor color;

    // Now, for each pixel of the image,
    for(unsigned int x(0) ; x < image.GetWidth() ; x++)
        for(unsigned int y(0) ; y < image.GetWidth() ; y++)
        {
            // Calculate the noise value in that point. First,
            // set the point coordinates in the generator
            perlin2d.Set(x,y);

            // Then get the noise value, comprised in [-1.0;+1.0]
            float noiseValue = perlin2d.Get();

            // Convert this noise value to the range [0;255]
            int pixelValue = static_cast<int>((noiseValue + 1.f) * 255.f / 2.f);

            // Finally, give this value to all three components RBG of the pixel color
            color.r = pixelValue;
            color.g = pixelValue;
            color.b = pixelValue;

            // And write the pixel color to the image at pixel coordinates (x,y)
            image.SetPixelColor(color, x, y);
        }

    // Save the image to disk

	return EXIT_SUCCESS;
}
