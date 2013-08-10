#include <Nazara/Graphics.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/TerrainRenderer/TerrainRenderer.hpp>
#include <Nazara/TerrainRenderer/TerrainChunk.hpp>
#include <Nazara/Math/Matrix4.hpp>
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
/*
    NzInitializer<NzDebugDrawer> nzDebugRender;
	if (!nzDebugRender)
	{
		std::cout << "Failed to initialize Debug Renderer, see NazaraLog.log for further informations" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}*/

	NzDebugDrawer::Initialize();

    NzScene scene;

    NzTexture* texture = new NzTexture;
	if (texture->LoadCubemapFromFile("resources/skybox-space.png"))
	{
		// Si la création du cubemap a fonctionné

		// Nous indiquons que la texture est "non-persistente", autrement dit elle sera libérée automatiquement par le moteur
		// à l'instant précis où elle ne sera plus utilisée, dans ce cas-ci, ce sera à la libération de l'objet skybox,
		// ceci arrivant lorsqu'un autre background est affecté à la scène, ou lorsque la scène sera libérée
		texture->SetPersistent(false);

		// Nous créons le background en lui affectant la texture
		NzSkyboxBackground* background = new NzSkyboxBackground(texture);

		// Nous pouvons en profiter pour paramétrer le background.
		// Cependant, nous n'avons rien de spécial à faire ici, nous pouvons donc l'envoyer à la scène.
		scene.SetBackground(background);

		// Comme indiqué plus haut, la scène s'occupera automatiquement de la libération de notre background
	}
	else
	{
		delete texture; // Le chargement a échoué, nous libérons la texture
		std::cout << "Failed to load skybox" << std::endl;
	}

    /// On crée un chunk de terrain manuellement
    NzTerrainChunk chunk;
    NzTerrainNodeID id;//L'identifiant unique du maillage uploadé, on s'en passe ici
    NzBoundingVolumef box(0.f,0.f,0.f,1000.f,1000.f,1000.f);//La bounding box du maillage, on s'en passe ici

    std::array<float,150> data;
    srand(1234567891);
    int index = 0;
    for(int y(0) ; y < 5 ; ++y)
        for(int x(0) ; x < 5 ; ++x)
        {
            index = (x + 5 * y)*6;
            //On génère une grille
            data.at(index) = x * 1000.f;
            data.at(index + 1) = (rand() % 10 - 5) * 100.f;
            data.at(index + 2) = y * 1000.f;
            //Normales vers le haut
            data.at(index + 3) = 0.f;
            data.at(index + 4) = 1.f;
            data.at(index + 5) = 0.f;
        }

    chunk.AddMesh(data,box,id);

    ///Code classique pour ouvrir une fenêtre avec Nazara
    NzString windowTitle("Terrain Renderer example");
	NzRenderWindow window(NzVideoMode(800,600,32),windowTitle,nzWindowStyle_Default);
	window.SetFramerateLimit(100);
	window.EnableVerticalSync(false);
	window.SetCursor(nzWindowCursor_None);

	/// Caméra
	//NzVector3f camPos(-2000.f, 1800.f, 2000.f);
	//NzVector3f camPos(7241.f, 12618.f, 3130.f);
	NzVector3f camPos(-1000.f, 2300.f, 340.f);
	NzEulerAnglesf camRot(-30.f, -90.f, 0.f);
	NzCamera camera;
	camera.SetPosition(camPos);
	camera.SetRotation(camRot);
	camera.SetFOV(70.f);
	camera.SetZFar(100000.f);
	camera.SetZNear(10.f);
	camera.SetParent(scene);
	camera.SetTarget(window);

    /// Gestion du temps
    NzClock secondClock, updateClock; // Des horloges pour gérer le temps
	unsigned int fps = 0; // Compteur de FPS
	float camSpeed = 50.f;
	float sensitivity = 0.2f;

	/// Quelques variables
	bool camMode = true;
    bool drawWireframe = false;
    bool terrainUpdate = true;

    NzDebugDrawer::SetPointSize(30.f);

    //std::cout<<NzDebugDrawer::GetLineWidth()<<std::endl;
    //std::cout<<NzDebugDrawer::GetPointSize()<<std::endl;
    //std::cout<<NzDebugDrawer::GetPrimaryColor()<<std::endl;
    //NzDebugDrawer::EnableDepthBuffer(true);

    NzRenderStates renderStates;
    renderStates.parameters[nzRendererParameter_DepthBuffer] = true;
    renderStates.parameters[nzRendererParameter_DepthWrite] = true;

    //std::cout<<"Starting main loop"<<std::endl;

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

        //NzRenderer::SetClearColor(25, 25, 25);
        //NzRenderer::Clear(nzRendererClear_Color | nzRendererClear_Depth);
		scene.Update();
		scene.Cull();
		scene.UpdateVisible();
		scene.Draw();

		NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Identity());
		//NzBoundingVolumef box(0.f,0.f,0.f,1000.f,1000.f,1000.f);
		box.Update(NzMatrix4f::Identity());
		NzDebugDrawer::Draw(box);

		// Dessin du chunk
		NzRenderer::SetRenderStates(renderStates);
		NzRenderer::SetShaderProgram(&(NzTerrainRenderer::GetShader()));
        NzRenderer::SetFaceFilling(nzFaceFilling_Line);
        NzRenderer::SetIndexBuffer(&(NzTerrainRenderer::GetIndexBuffer()));
        NzTerrainRenderer::DrawTerrainChunk(chunk);

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
