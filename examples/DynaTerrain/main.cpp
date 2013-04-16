#include <Nazara/Graphics.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/DynaTerrain/DynamicTerrain.hpp>
#include <Nazara/DynaTerrain/DynamicPlanet.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include "MyHeightSource2D.hpp"
#include "MyHeightSource3D.hpp"
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

	NzScene scene;

    ///Initialisation du terrain
    // On instancie notre source de hauteur personnalis�e, d�finissant la hauteur du terrain en tout point
    MyHeightSource2D source2;
    // On cr�� la configuration du terrain
    NzTerrainConfiguration myConfig;
    //Les param�tres de base
    //myConfig.center = NzVector3f(0.f,0.f,0.f);//Le centre du terrain
    myConfig.terrainOrientation = NzEulerAnglesf(0.f, 0.f, 0.f);//L'orientation du terrain
    myConfig.terrainSize = 4000.f;
    myConfig.maxHeight = 1000.f;//La hauteur maximale du terrain
    myConfig.minPrecision = 2;//La pr�cision minimale du terrain
    myConfig.fragmentShader = "resources/terrain_shader.frag";
    myConfig.vertexShader = "resources/terrain_shader.vert";
    myConfig.groundTextures = "resources/debug_grid2.png";//"resources/debug_grid2.png";//"resources/debug_texture2.png";//"resources/debug_texture_flat.png";"resources/dt_tiles.jpg"
    //Le param�tre li� � la pr�cision des pentes
    myConfig.maxSlopePrecision = 2;//La pr�cision maximale en cas de tr�s forte pente
    //Les param�tres li�s � la pr�cision autour de la cam�ra
    myConfig.higherCameraPrecision = 9;//La pr�cision maximale engendr�e par la cam�ra
    myConfig.cameraRadiusAmount = 7;//Le nombre max de rayons de pr�cision autour de la cam�ra
    myConfig.higherCameraPrecisionRadius = 100.f;//Le rayon du cercle le plus pr�cis (� garder tr�s petit si la pr�cision est importante)
    myConfig.radiusSizeIncrement = 2.5f;//L'incr�ment en taille entre deux rayons cons�cutifs

    //Si la configuration n'est pas bonne, elle sera r�par�e au plus proche automatiquement ( TODO !)
    if(!myConfig.IsValid())
        std::cout<<"Terrain configuration not valid..."<<std::endl;

    NzDynamicTerrain terrain(myConfig,&source2);

    cout<<"Initializing terrain, please wait..."<<endl;
    terrain.Initialize();
    cout<<"Terrain initialized successfully !"<<endl;

    terrain.SetParent(scene);
    NzVector3f terrainPos(100.f,100.f,100.f);
    terrain.SetPosition(terrainPos);

    /*
    MyHeightSource3D source3;
    NzPlanetConfiguration myConfig;

    ///Les param�tres de base
    myConfig.center = NzVector3f(0.f,0.f,0.f);//Le centre du terrain
    myConfig.planetRadius = 6378.f;
    myConfig.maxHeight = 884.8;//La hauteur maximale du terrain
    myConfig.minPrecision = 2;//La pr�cision minimale du terrain
    myConfig.fragmentShader = "resources/terrain_shader.frag";
    myConfig.vertexShader = "resources/terrain_shader.vert";
    myConfig.groundTextures = "resources/debug_grid2.png";//"resources/debug_grid2.png";//"resources/debug_texture2.png";//"resources/debug_texture_flat.png";"resources/dt_tiles.jpg"
    ///Le param�tre li� � la pr�cision des pentes
    myConfig.maxSlopePrecision = 2;//La pr�cision maximale en cas de tr�s forte pente
    ///Les param�tres li�s � la pr�cision autour de la cam�ra
    myConfig.higherCameraPrecision = 12;//La pr�cision maximale engendr�e par la cam�ra
    myConfig.cameraRadiusAmount = 9;//Le nombre max de rayons de pr�cision autour de la cam�ra
    myConfig.higherCameraPrecisionRadius = 10.f;//Le rayon du cercle le plus pr�cis (� garder tr�s petit si la pr�cision est importante)
    myConfig.radiusSizeIncrement = 2.5f;//L'incr�ment en taille entre deux rayons cons�cutifs
    //Si la configuration n'est pas bonne, elle sera r�par�e au plus proche automatiquement ( TODO !)
    if(!myConfig.IsValid())
        std::cout<<"Terrain configuration not valid..."<<std::endl;

    NzDynamicPlanet planet(myConfig,&source3);
    //planet.Initialize();
    cout<<"Planet initialized successfully !"<<endl;
*/



    //cout<<"Nombre de feuilles  : "<<quad.GetLeafNodesAmount()<<endl;
    //cout<<"Nombre de triangles : "<<quad.GetLeafNodesAmount()*32<<endl;
    cout<<"---------------------------------------------------------------"<<endl;




    ///Code classique pour ouvrir une fen�tre avec Nazara
    NzString windowTitle("DynaTerrain example");
	NzRenderWindow window(NzVideoMode(800,600,32),windowTitle,nzWindowStyle_Default);
	window.SetFramerateLimit(100);
	window.EnableVerticalSync(false);
	window.SetCursor(nzWindowCursor_None);
    NzRenderer::SetClearColor(25, 25, 25);

	/// Cam�ra
	NzVector3f camPos(-2000.f, 1800.f, 2000.f);
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
    NzClock secondClock, updateClock; // Des horloges pour g�rer le temps
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
	}
	else
	{
		delete texture;
		std::cout << "Failed to load skybox" << std::endl;
	}

	// Quelques variables
	bool camMode = true;
    bool drawWireframe = false;
    bool terrainUpdate = true;

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
						// Nous allons inverser le mode cam�ra et montrer/cacher le curseur en cons�quence
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

				case nzEventType_KeyPressed: // Une touche du clavier vient d'�tre enfonc�e
				{
					switch (event.key.code)
					{
						case NzKeyboard::Escape:
							window.Close();
							break;

						case NzKeyboard::F1:
							if (drawWireframe)
							{
								drawWireframe = false;
								NzRenderer::SetFaceFilling(nzFaceFilling_Fill);//FIX ME
							}
							else
							{
								drawWireframe = true;
								NzRenderer::SetFaceFilling(nzFaceFilling_Line);//FIX ME
							}
							break;

                        case NzKeyboard::F2:
                            terrainUpdate = !terrainUpdate;
                            break;

                        case NzKeyboard::Add:
                            terrainPos.x += 50.f;
                            break;

                        case NzKeyboard::Subtract:
                            terrainPos.x -= 50.f;
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

		// Mise � jour de la partie logique
		if (updateClock.GetMilliseconds() >= 1000/60.f)
		{
			float elapsedTime = updateClock.GetSeconds();

			// D�placement de la cam�ra
			static const NzVector3f forward(NzVector3f::Forward());
			static const NzVector3f left(NzVector3f::Left());
			static const NzVector3f up(NzVector3f::Up());

            float speed2 = (NzKeyboard::IsKeyPressed(NzKeyboard::Key::LShift)) ? camSpeed*20: camSpeed;
            NzVector3f speed(speed2,speed2,speed2);

            if (NzKeyboard::IsKeyPressed(NzKeyboard::Z))
                camera.Move(forward * speed * elapsedTime);

            if (NzKeyboard::IsKeyPressed(NzKeyboard::S))
                camera.Move(-forward * speed * elapsedTime);

            if (NzKeyboard::IsKeyPressed(NzKeyboard::Q))
                camera.Move(left * speed * elapsedTime);

            if (NzKeyboard::IsKeyPressed(NzKeyboard::D))
                camera.Move(-left * speed * elapsedTime);

            // En revanche, ici la hauteur est toujours la m�me, peu importe notre orientation
            if (NzKeyboard::IsKeyPressed(NzKeyboard::Space))
                camera.Move(up * speed * elapsedTime, nzCoordSys_Global);

            if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl))
                camera.Move(up * speed * elapsedTime, nzCoordSys_Global);

            terrain.SetPosition(terrainPos);

			updateClock.Restart();
		}

        camera.Activate();
		scene.Update();
		scene.Cull();
		scene.UpdateVisible();

		///------------------
        //On met � jour le terrain
        if(terrainUpdate)
        {
            //planet.Update(camera.GetPosition());
            terrain.Update(camera.GetPosition());
        }
        ///-----------------

        scene.Draw();///Le terrain est dessin� ici !
		// Nous mettons � jour l'�cran
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
