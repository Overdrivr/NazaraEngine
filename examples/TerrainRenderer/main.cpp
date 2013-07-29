#include <Nazara/Graphics.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/TerrainRenderer/TerrainRenderer.hpp>
#include <Nazara/TerrainRenderer/TerrainChunk.hpp>
#include <iostream>
#include <sstream>

using namespace std;

int main()
{
	NzInitializer<NzGraphics> nazara;
	if (!nazara)
	{
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	NzInitializer<NzTerrainRenderer> nzTerrainRender;
	if (!nzTerrainRender)
	{
		std::cout << "Failed to initialize Terrain Renderer, see NazaraLog.log for further informations" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}


    NzScene scene;

    /// On crée un chunk de terrain manuellement
    NzTerrainChunk chunk;
    NzTerrainNodeID id;//L'identifiant unique du maillage uploadé, on s'en passe ici
    NzBoundingBoxf box;//La bounding box du maillage, on s'en passe ici

    std::array<float,150> data;

    for(int x(0) ; x < 5 ; ++x)
        for(int y(0) ; y < 5 ; ++y)
        {
            //On génère une grille
            data.at(((x + 5 * y))*6) = x * 1000.f;
            data.at(((x + 5 * y))*6 + 1) = 0.f;
            data.at(((x + 5 * y))*6 + 2) = y * 1000.f;
            //Normales vers le haut
            data.at(((x + 5 * y))*6 + 3) = 0.f;
            data.at(((x + 5 * y))*6 + 4) = 1.f;
            data.at(((x + 5 * y))*6 + 5) = 0.f;
        }

    chunk.AddMesh(data,box,id);

    ///Code classique pour ouvrir une fenêtre avec Nazara
    NzString windowTitle("DynaTerrain example");
	NzRenderWindow window(NzVideoMode(800,600,32),windowTitle,nzWindowStyle_Default);
	window.SetFramerateLimit(100);
	window.EnableVerticalSync(false);
	window.SetCursor(nzWindowCursor_None);
    NzRenderer::SetClearColor(25, 25, 25);

	/// Caméra
	NzVector3f camPos(-2000.f, 1800.f, 2000.f);
	//NzVector3f camPos(7241.f, 12618.f, 3130.f);
	NzEulerAnglesf camRot(-30.f, -45.f, 0.f);
	NzCamera camera;
	camera.SetPosition(camPos);
	camera.SetRotation(camRot);
	camera.SetFOV(70.f);
	camera.SetZFar(100000.f);
	camera.SetZNear(10.f);
	camera.SetTarget(window);
	camera.SetParent(scene);

    /// Lampe
	NzLight spotLight(nzLightType_Spot);
	spotLight.SetParent(camera);

    ///Gestion du temps
    NzClock secondClock, updateClock; // Des horloges pour gérer le temps
	unsigned int fps = 0; // Compteur de FPS
	float camSpeed = 50.f;
	float sensitivity = 0.2f;

	///Skybox

	NzTexture* texture = new NzTexture;
	if (texture->LoadCubemapFromFile("resources/skyboxsun5deg2.png"))
	{
		texture->SetPersistent(false);
		NzSkyboxBackground* background = new NzSkyboxBackground(texture);
		scene.SetBackground(background);
		std::cout<<"Skybox loaded successfully."<<endl;
	}
	else
	{
		delete texture;
		std::cout << "Failed to load skybox." << std::endl;
	}

	// Quelques variables
	bool camMode = true;
    bool drawWireframe = false;
    bool terrainUpdate = true;

    std::cout<<"Starting main loop"<<std::endl;

	while (window.IsOpen())
	{
		NzEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case nzEventType_Quit:
					window.Close();
					break;

				case nzEventType_MouseMoved:
				{
					if (!camMode)
						break;

					camRot.yaw = NzNormalizeAngle(camRot.yaw - event.mouseMove.deltaX*sensitivity);
					camRot.pitch = NzClamp(camRot.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);
					camera.SetRotation(camRot);
					NzMouse::SetPosition(window.GetWidth()/2, window.GetHeight()/2, window);
					break;
				}

				case nzEventType_MouseButtonPressed:
					if (event.mouseButton.button == NzMouse::Left)
					{
						// L'utilisateur vient d'appuyer sur le bouton left de la souris
						// Nous allons inverser le mode caméra et montrer/cacher le curseur en conséquence
						if (camMode)
						{
							camMode = false;
							window.SetCursor(nzWindowCursor_Default);
						}
						else
						{
							camMode = true;
							window.SetCursor(nzWindowCursor_None);
						}
					}
                    break;

				case nzEventType_KeyPressed: // Une touche du clavier vient d'être enfoncée
				{
					switch (event.key.code)
					{
						case NzKeyboard::Escape:
							window.Close();
							break;

						default:
							break;
					}

					break;
				}
				default:
					break;
			}
		}

		// Mise à jour de la partie logique
		if (updateClock.GetMilliseconds() >= 1000/60.f)
		{
			float elapsedTime = updateClock.GetSeconds();

			// Déplacement de la caméra
			static const NzVector3f forward(NzVector3f::Forward());
			static const NzVector3f left(NzVector3f::Left());
			static const NzVector3f up(NzVector3f::Up());

            float speed2 = (NzKeyboard::IsKeyPressed(NzKeyboard::Key::LShift)) ? camSpeed*40: camSpeed;
            NzVector3f speed(speed2,speed2,speed2);

            if (NzKeyboard::IsKeyPressed(NzKeyboard::Z))
                camera.Move(forward * speed * elapsedTime);

            if (NzKeyboard::IsKeyPressed(NzKeyboard::S))
                camera.Move(-forward * speed * elapsedTime);

            if (NzKeyboard::IsKeyPressed(NzKeyboard::Q))
                camera.Move(left * speed * elapsedTime);

            if (NzKeyboard::IsKeyPressed(NzKeyboard::D))
                camera.Move(-left * speed * elapsedTime);

            // En revanche, ici la hauteur est toujours la même, peu importe notre orientation
            if (NzKeyboard::IsKeyPressed(NzKeyboard::Space))
                camera.Move(up * speed * elapsedTime, nzCoordSys_Global);

            if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl))
                camera.Move(up * speed * elapsedTime, nzCoordSys_Global);

            //terrain.SetPosition(terrainPos);

			updateClock.Restart();
		}

        camera.Activate();
		scene.Update();
		scene.Cull();
		scene.UpdateVisible();

		// Dessin du chunk
		NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
        NzRenderer::SetIndexBuffer(&(NzTerrainRenderer::GetIndexBuffer()));
        NzTerrainRenderer::DrawTerrainChunk(chunk);

        scene.Draw();
		// Nous mettons à jour l'écran
		window.Display();

		fps++;

		// Toutes les secondes
		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(windowTitle + " (FPS: " + NzString::Number(fps) + ')' + "( Camera in : " + camera.GetPosition().ToString() + ")");// (Updated Nodes : " + NzString::Number(quad.GetSubdivisionsAmount()) + "/s)");
			fps = 0;
			secondClock.Restart();
		}
	}

    return 0;
}
