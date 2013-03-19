#include <Nazara/3D.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/DynaTerrain/DynamicTerrain.hpp>
#include <Nazara/DynaTerrain/DynamicPlanet.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <iostream>
#include <sstream>
#include "MyHeightSource2D.hpp"
#include "MyHeightSource3D.hpp"

using namespace std;

int main()
{
	NzContextParameters::defaultCompatibilityProfile = true;
	NzInitializer<Nz3D> nazara;
	if (!nazara)
	{
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}
	NzDebugDrawer::Initialize();

    ///Initialisation du terrain
    // On instancie notre source de hauteur personnalisée, définissant la hauteur du terrain en tout point
    MyHeightSource2D source2;
    MyHeightSource3D source3;
    // La source peut charger des données manuelles grâce à cette méthode
    //source.LoadTerrainFile("resources/terrain.hsd");

    /// On créé la configuration du terrain
    NzTerrainConfiguration myConfig;

        ///Les paramètres de base
    myConfig.center = NzVector3f(0.f,0.f,0.f);//Le centre du terrain
    myConfig.terrainOrientation = NzEulerAnglesf(0.f, 0.f, 0.f);//L'orientation du terrain
    myConfig.maxHeight = 800.f;//La hauteur maximale du terrain
    myConfig.minPrecision = 2;//La précision minimale du terrain
    myConfig.fragmentShader = "resources/terrain_shader.frag";
    myConfig.vertexShader = "resources/terrain_shader.vert";
    myConfig.groundTextures = "resources/debug_grid2.png";//"resources/debug_grid2.png";//"resources/debug_texture2.png";//"resources/debug_texture_flat.png";"resources/dt_tiles.jpg"

        ///Le paramètre lié à la précision des pentes
    myConfig.maxSlopePrecision = 2;//La précision maximale en cas de très forte pente

        ///Les paramètres liés à la précision autour de la caméra
    myConfig.higherCameraPrecision = 12;//La précision maximale engendrée par la caméra
    myConfig.cameraRadiusAmount = 10;//Le nombre max de rayons de précision autour de la caméra
    myConfig.higherCameraPrecisionRadius = 5.f;//Le rayon du cercle le plus précis (à garder très petit si la précision est importante)
    myConfig.radiusSizeIncrement = 2.0f;//L'incrément en taille entre deux rayons consécutifs

    //Si la configuration n'est pas bonne, elle sera réparée au plus proche automatiquement ( TODO !)
    if(!myConfig.IsValid())
        std::cout<<"Terrain configuration not valid..."<<std::endl;

    /*NzPlanetConfiguration myConfig;

        ///Les paramètres de base
    myConfig.center = NzVector3f(0.f,0.f,0.f);//Le centre du terrain
    myConfig.planetRadius = 2000.f;
    myConfig.maxHeight = 1000.f;//La hauteur maximale du terrain
    myConfig.minPrecision = 2;//La précision minimale du terrain
    myConfig.fragmentShader = "resources/terrain_shader.frag";
    myConfig.vertexShader = "resources/terrain_shader.vert";
    myConfig.groundTextures = "resources/debug_grid2.png";//"resources/debug_grid2.png";//"resources/debug_texture2.png";//"resources/debug_texture_flat.png";"resources/dt_tiles.jpg"

        ///Le paramètre lié à la précision des pentes
    myConfig.maxSlopePrecision = 2;//La précision maximale en cas de très forte pente

        ///Les paramètres liés à la précision autour de la caméra
    myConfig.higherCameraPrecision = 10;//La précision maximale engendrée par la caméra
    myConfig.cameraRadiusAmount = 8;//Le nombre max de rayons de précision autour de la caméra
    myConfig.higherCameraPrecisionRadius = 10.f;//Le rayon du cercle le plus précis (à garder très petit si la précision est importante)
    myConfig.radiusSizeIncrement = 2.5f;//L'incrément en taille entre deux rayons consécutifs
*/
    //Si la configuration n'est pas bonne, elle sera réparée au plus proche automatiquement ( TODO !)
    if(!myConfig.IsValid())
        std::cout<<"Terrain configuration not valid..."<<std::endl;

    //Le terrain en lui-même, aka le quadtree
    NzDynamicTerrain terrain(myConfig,&source2);
    //NzDynamicPlanet planet(myConfig,&source3);

    cout<<"Initializing terrain, please wait..."<<endl;

    //On initialise le terrain
    terrain.Initialize();
    //planet.Initialize();

    //cout<<"Terrain initialized successfully !"<<endl;
    cout<<"Planet initialized successfully !"<<endl;
    //cout<<"Nombre de feuilles  : "<<quad.GetLeafNodesAmount()<<endl;
    //cout<<"Nombre de triangles : "<<quad.GetLeafNodesAmount()*32<<endl;
    cout<<"---------------------------------------------------------------"<<endl;


    ///Code classique pour ouvrir une fenêtre avec Nazara
    NzString windowTitle("DynaTerrain example");
	NzRenderWindow window(NzVideoMode(800,600,32),windowTitle,nzWindowStyle_Default);
	window.SetFramerateLimit(100);
	window.EnableVerticalSync(false);
	NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(window.GetWidth())/window.GetHeight(), 1.f, 100000.f));

	NzClock secondClock, updateClock; // Des horloges pour gérer le temps
	unsigned int fps = 0; // Compteur de FPS
    NzMatrix4f matrix;
    matrix.MakeIdentity();
    NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(NzVector3f(0.f,0.f,0.f), NzVector3f::Forward()));

	// Notre caméra
	NzVector3f camPos(-2000.f, 1800.f, 2000.f);
	NzEulerAnglesf camRot(-30.f, -45.f, 0.f);

	NzNode camera;
	camera.SetTranslation(camPos);
	camera.SetRotation(camRot);

	float camSpeed = 80.f;
	float sensitivity = 0.2f;

	// Quelques variables
	bool camMode = true;
	window.SetCursor(nzWindowCursor_None);
	bool windowOpen = true;
    bool drawWireframe = false;
    bool terrainUpdate = true;


	// On peut commencer la boucle du programme
	while (windowOpen)
	{
	// Ici nous gérons les évènements
		NzEvent event;
		while (window.PollEvent(&event)) // Avons-nous un évènement dans la file ?
		{
			// Nous avons un évènement !

			switch (event.type) // De quel type est cet évènement ?
			{
				case nzEventType_Quit: // L'utilisateur/L'OS nous a demandé de terminer notre exécution
					windowOpen = false; // Nous terminons alors la boucle
					break;

				case nzEventType_MouseMoved: // La souris a bougé
				{
					// Si nous ne sommes pas en mode free-fly, on ne traite pas l'évènement
					if (!camMode)
						break;

					// On modifie l'angle de la caméra grâce au déplacement relatif de la souris
					camRot.yaw = NzNormalizeAngle(camRot.yaw - event.mouseMove.deltaX*sensitivity);

					// Pour éviter les loopings mais surtout les problèmes de calculation de la matrice de vue, on restreint les angles
					camRot.pitch = NzClamp(camRot.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					// La matrice vue représente les transformations effectuées par la caméra
					// On recalcule la matrice de la caméra et on l'envoie au renderer
					camera.SetRotation(camRot); // Conversion des angles d'euler en quaternion

					// Pour éviter que le curseur ne sorte de l'écran, nous le renvoyons au centre de la fenêtre
					NzMouse::SetPosition(window.GetWidth()/2, window.GetHeight()/2, window);
					break;
				}

				case nzEventType_MouseButtonPressed: // L'utilisateur (ou son chat) vient de cliquer sur la souris
					if (event.mouseButton.button == NzMouse::Left) // Est-ce le clic gauche ?
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
				case nzEventType_Resized: // L'utilisateur a changé la taille de la fenêtre, le coquin !
					NzRenderer::SetViewport(NzRectui(0, 0, event.size.width, event.size.height)); // Adaptons l'affichage

					// Il nous faut aussi mettre à jour notre matrice de projection
					NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(event.size.width)/event.size.height, 1.f, 10000.f));
					break;

				case nzEventType_KeyPressed: // Une touche du clavier vient d'être enfoncée
				{
					switch (event.key.code)
					{
						case NzKeyboard::Escape:
							windowOpen = false;
							break;

						case NzKeyboard::F1:
							if (drawWireframe)
							{
								drawWireframe = false;
								NzRenderer::SetFaceFilling(nzFaceFilling_Fill);
							}
							else
							{
								drawWireframe = true;
								NzRenderer::SetFaceFilling(nzFaceFilling_Line);
							}
							break;

                        case NzKeyboard::F2:
                            terrainUpdate = !terrainUpdate;
                            break;

						default:
							break;
					}

					break;
				}

				default: // Les autres évènements, on s'en fiche
					break;
			}
		}

		// Mise à jour de la partie logique
		if (updateClock.GetMilliseconds() >= 1000/60) // 60 fois par seconde
		{
			float elapsedTime = updateClock.GetSeconds(); // Le temps depuis la dernière mise à jour

			// Déplacement de la caméra
			static const NzVector3f forward(NzVector3f::Forward());
			static const NzVector3f left(NzVector3f::Left());
			static const NzVector3f up(NzVector3f::Up());

			// Notre rotation sous forme de quaternion nous permet de tourner un vecteur
			// Par exemple ici, quaternion * forward nous permet de récupérer le vecteur de la direction "avant"


				// Sinon, c'est la caméra qui se déplace (en fonction des mêmes touches)

				// Un boost en maintenant le shift gauche
				float speed2 = (NzKeyboard::IsKeyPressed(NzKeyboard::Key::LShift)) ? camSpeed*7: camSpeed;
                NzVector3f speed(speed2,speed2,speed2);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::Z))
					camera.Translate(forward * speed * elapsedTime);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::S))
					camera.Translate(-forward * speed * elapsedTime);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::Q))
					camera.Translate(left * speed * elapsedTime);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::D))
					camera.Translate(-left * speed * elapsedTime);

				// En revanche, ici la hauteur est toujours la même, peu importe notre orientation
				if (NzKeyboard::IsKeyPressed(NzKeyboard::Space))
					camera.Translate(up * speed * elapsedTime, nzCoordSys_Global);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl))
					camera.Translate(up * speed * elapsedTime, nzCoordSys_Global);

			updateClock.Restart();
		}

        NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(camera.GetDerivedTranslation(), camera.GetDerivedTranslation() + camera.GetDerivedRotation() * NzVector3f::Forward()));

		// Notre scène 3D requiert un test de profondeur
		NzRenderer::Enable(nzRendererParameter_DepthTest, true);

		// Nous voulons avoir un fond noir
		NzRenderer::SetClearColor(25, 25, 25);

		// Et nous effaçons les buffers de couleur et de profondeur
		NzRenderer::Clear(nzRendererClear_Color | nzRendererClear_Depth);

        // La matrice world est celle qui représente les transformations du modèle
        NzRenderer::SetMatrix(nzMatrixType_World, matrix);

        //On met à jour le terrain
        if(terrainUpdate)
        {
            //planet.Update(camera.GetTranslation());
            terrain.Update(camera.GetTranslation());
        }

        //On dessine le terrain
        terrain.Render();
        //planet.Render();

		// Nous mettons à jour l'écran
		window.Display();

		fps++;

		// Toutes les secondes
		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(windowTitle + " (FPS: " + NzString::Number(fps) + ')' + "( Camera in : " + camera.GetTranslation() + ")");// (Updated Nodes : " + NzString::Number(quad.GetSubdivisionsAmount()) + "/s)");
			fps = 0;
			secondClock.Restart();
		}
	}

    return 0;
}
