#include <Nazara/3D.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTreeConfiguration.hpp>
#include <iostream>
#include "MyHeightSource.hpp"

using namespace std;

int main()
{
    // Cette ligne active le mode de compatibilité d'OpenGL lors de l'initialisation de Nazara (Nécessaire pour le shader)
	NzContextParameters::defaultCompatibilityProfile = true;
	NzInitializer<NzRenderer> renderer;
	if (!renderer)
	{
		// Ça n'a pas fonctionné, le pourquoi se trouve dans le fichier NazaraLog.log
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur
		return EXIT_FAILURE;
	}

    ///Initialisation du terrain
    // On instancie notre source de hauteur personnalisée, définissant la hauteur du terrain en tout point
    MyHeightSource source;
    // La source peut charger des données manuelles grâce à cette méthode
    source.LoadTerrainFile("resources/terrain.hsd");

    // On créé la configuration du terrain
    NzTerrainQuadTreeConfiguration myConfig;

    myConfig.slopeMaxDepth = 6;//La précision maximale en cas de très forte pente
    myConfig.minimumDepth = 3;//La précision minimale du terrain
    myConfig.terrainHeight = 1000.f;//La hauteur maximale du terrain

    //Configurer correctement un terrain est complexe pour l'instant
    //Si la configuration n'est pas bonne, pas de problème on utilise une configuration par défaut
    if(!myConfig.IsValid())
        std::cout<<"Terrain configuration not valid, falling back to default."<<std::endl;

    //Le terrain en lui-même, aka le quadtree
    NzTerrainQuadTree quad(myConfig,NzVector2f(0.f,0.f),&source);
    cout<<"Initializing terrain, please wait..."<<endl;
    //On initialise le terrain, en lui indiquant les chemins vers les shaders
    quad.Initialize("resources/slope_shader.vert","resources/slope_shader.frag","resources/dt_tiles.jpg");

    cout<<"Nombre de feuilles  : "<<quad.GetLeavesList().size()<<endl;
    cout<<"Nombre de triangles : "<<quad.GetLeavesList().size()*32<<endl;
    cout<<"---------------------------------------------------------------"<<endl;


    ///Code classique pour ouvrir une fenêtre avec Nazara

    NzString windowTitle("DynaTerrain example");
	NzRenderWindow window(NzVideoMode(800,600,32),windowTitle,nzWindowStyle_Default);
	window.SetFramerateLimit(100);
	NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(window.GetWidth())/window.GetHeight(), 1.f, 10000.f));

	NzClock secondClock, updateClock; // Des horloges pour gérer le temps
	unsigned int fps = 0; // Compteur de FPS
    NzMatrix4f matrix;
    matrix.MakeIdentity();
    NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(NzVector3f(0.f,0.f,0.f), NzVector3f::Forward()));

	// Notre caméra
	NzVector3f camPos(-600.f, 900.f, 1200.f);
	NzEulerAnglesf camRot(0.f, 0.f, 0.f);

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
        quad.Update(camera.GetTranslation());

        //On dessine le terrain
        quad.Render();
		// Nous mettons à jour l'écran
		window.Display();

		fps++;

		// Toutes les secondes
		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(windowTitle + " (FPS: " + NzString::Number(fps) + ')' + "( Camera in : " + camera.GetTranslation() + ") (Updated Nodes : " + NzString::Number(quad.GetSubdivisionsAmount()) + "/s)");
			fps = 0;
			secondClock.Restart();
		}
	}

    return 0;
}
