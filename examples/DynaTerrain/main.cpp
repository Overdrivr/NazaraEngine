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
    // Cette ligne active le mode de compatibilit� d'OpenGL lors de l'initialisation de Nazara (N�cessaire pour le shader)
	NzContextParameters::defaultCompatibilityProfile = true;
	NzInitializer<NzRenderer> renderer;
	if (!renderer)
	{
		// �a n'a pas fonctionn�, le pourquoi se trouve dans le fichier NazaraLog.log
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur
		return EXIT_FAILURE;
	}

    ///Initialisation du terrain
    // On instancie notre source de hauteur personnalis�e, d�finissant la hauteur du terrain en tout point
    MyHeightSource source;
    // La source peut charger des donn�es manuelles gr�ce � cette m�thode
    source.LoadTerrainFile("resources/terrain.hsd");

    // On cr�� la configuration du terrain
    NzTerrainQuadTreeConfiguration myConfig;

    myConfig.slopeMaxDepth = 6;//La pr�cision maximale en cas de tr�s forte pente
    myConfig.minimumDepth = 3;//La pr�cision minimale du terrain
    myConfig.terrainHeight = 1000.f;//La hauteur maximale du terrain

    //Configurer correctement un terrain est complexe pour l'instant
    //Si la configuration n'est pas bonne, pas de probl�me on utilise une configuration par d�faut
    if(!myConfig.IsValid())
        std::cout<<"Terrain configuration not valid, falling back to default."<<std::endl;

    //Le terrain en lui-m�me, aka le quadtree
    NzTerrainQuadTree quad(myConfig,NzVector2f(0.f,0.f),&source);
    cout<<"Initializing terrain, please wait..."<<endl;
    //On initialise le terrain, en lui indiquant les chemins vers les shaders
    quad.Initialize("resources/slope_shader.vert","resources/slope_shader.frag","resources/dt_tiles.jpg");

    cout<<"Nombre de feuilles  : "<<quad.GetLeavesList().size()<<endl;
    cout<<"Nombre de triangles : "<<quad.GetLeavesList().size()*32<<endl;
    cout<<"---------------------------------------------------------------"<<endl;


    ///Code classique pour ouvrir une fen�tre avec Nazara

    NzString windowTitle("DynaTerrain example");
	NzRenderWindow window(NzVideoMode(800,600,32),windowTitle,nzWindowStyle_Default);
	window.SetFramerateLimit(100);
	NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(window.GetWidth())/window.GetHeight(), 1.f, 10000.f));

	NzClock secondClock, updateClock; // Des horloges pour g�rer le temps
	unsigned int fps = 0; // Compteur de FPS
    NzMatrix4f matrix;
    matrix.MakeIdentity();
    NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(NzVector3f(0.f,0.f,0.f), NzVector3f::Forward()));

	// Notre cam�ra
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
	// Ici nous g�rons les �v�nements
		NzEvent event;
		while (window.PollEvent(&event)) // Avons-nous un �v�nement dans la file ?
		{
			// Nous avons un �v�nement !

			switch (event.type) // De quel type est cet �v�nement ?
			{
				case nzEventType_Quit: // L'utilisateur/L'OS nous a demand� de terminer notre ex�cution
					windowOpen = false; // Nous terminons alors la boucle
					break;

				case nzEventType_MouseMoved: // La souris a boug�
				{
					// Si nous ne sommes pas en mode free-fly, on ne traite pas l'�v�nement
					if (!camMode)
						break;

					// On modifie l'angle de la cam�ra gr�ce au d�placement relatif de la souris
					camRot.yaw = NzNormalizeAngle(camRot.yaw - event.mouseMove.deltaX*sensitivity);

					// Pour �viter les loopings mais surtout les probl�mes de calculation de la matrice de vue, on restreint les angles
					camRot.pitch = NzClamp(camRot.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					// La matrice vue repr�sente les transformations effectu�es par la cam�ra
					// On recalcule la matrice de la cam�ra et on l'envoie au renderer
					camera.SetRotation(camRot); // Conversion des angles d'euler en quaternion

					// Pour �viter que le curseur ne sorte de l'�cran, nous le renvoyons au centre de la fen�tre
					NzMouse::SetPosition(window.GetWidth()/2, window.GetHeight()/2, window);
					break;
				}

				case nzEventType_MouseButtonPressed: // L'utilisateur (ou son chat) vient de cliquer sur la souris
					if (event.mouseButton.button == NzMouse::Left) // Est-ce le clic gauche ?
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
				case nzEventType_Resized: // L'utilisateur a chang� la taille de la fen�tre, le coquin !
					NzRenderer::SetViewport(NzRectui(0, 0, event.size.width, event.size.height)); // Adaptons l'affichage

					// Il nous faut aussi mettre � jour notre matrice de projection
					NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(event.size.width)/event.size.height, 1.f, 10000.f));
					break;

				case nzEventType_KeyPressed: // Une touche du clavier vient d'�tre enfonc�e
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

				default: // Les autres �v�nements, on s'en fiche
					break;
			}
		}

		// Mise � jour de la partie logique
		if (updateClock.GetMilliseconds() >= 1000/60) // 60 fois par seconde
		{
			float elapsedTime = updateClock.GetSeconds(); // Le temps depuis la derni�re mise � jour

			// D�placement de la cam�ra
			static const NzVector3f forward(NzVector3f::Forward());
			static const NzVector3f left(NzVector3f::Left());
			static const NzVector3f up(NzVector3f::Up());

			// Notre rotation sous forme de quaternion nous permet de tourner un vecteur
			// Par exemple ici, quaternion * forward nous permet de r�cup�rer le vecteur de la direction "avant"


				// Sinon, c'est la cam�ra qui se d�place (en fonction des m�mes touches)

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

				// En revanche, ici la hauteur est toujours la m�me, peu importe notre orientation
				if (NzKeyboard::IsKeyPressed(NzKeyboard::Space))
					camera.Translate(up * speed * elapsedTime, nzCoordSys_Global);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl))
					camera.Translate(up * speed * elapsedTime, nzCoordSys_Global);

			updateClock.Restart();
		}

        NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(camera.GetDerivedTranslation(), camera.GetDerivedTranslation() + camera.GetDerivedRotation() * NzVector3f::Forward()));

		// Notre sc�ne 3D requiert un test de profondeur
		NzRenderer::Enable(nzRendererParameter_DepthTest, true);

		// Nous voulons avoir un fond noir
		NzRenderer::SetClearColor(25, 25, 25);

		// Et nous effa�ons les buffers de couleur et de profondeur
		NzRenderer::Clear(nzRendererClear_Color | nzRendererClear_Depth);

        // La matrice world est celle qui repr�sente les transformations du mod�le
        NzRenderer::SetMatrix(nzMatrixType_World, matrix);

        //On met � jour le terrain
        quad.Update(camera.GetTranslation());

        //On dessine le terrain
        quad.Render();
		// Nous mettons � jour l'�cran
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
