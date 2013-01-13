#include <iostream>
#include <Nazara/3D.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Utility/Utility.hpp>
#include "MyHeightSource.hpp"
#include "TerrainQuadTree.hpp"
#include <Nazara/Math/Circle.hpp>
#include <Nazara/Math/Rect.hpp>


using namespace std;

//TODO : rajouter des constructeurs par défaut à chacun des bruits
//TODO : Les bruits ne peuvent pas être instanciés par pointeurs ??

//TODO : Remplacer calcul variation moyenne de pente par variation max
//TODO2 : Un patch pourrait transmettre à son fils certaines valeurs de hauteur communes
int main()
{
    /*NzCirclef circle(-325,970,200);
    NzRectf rect(-1000,-1000,2000,2000);
    cout<<"intersects "<<circle.Intersect(rect)<<endl;
    cout<<"contains "<<circle.Contains(rect)<<endl;*/
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

    ///Partie réservée au terrain
    MyHeightSource source;
    source.LoadTerrainFile("terrain.hsd");

    NzTerrainQuadTreeConfiguration myConfig;
    myConfig.slopeMaxDepth = 6;
    myConfig.minimumDepth = 2;
    myConfig.terrainHeight = 500.f;
    std::cout<<"is valid"<<myConfig.IsValid()<<std::endl;

    NzTerrainQuadTree quad(myConfig,NzVector2f(0.f,0.f),&source);

    cout<<"--------------Preparation terrain----------------------------"<<endl;

    quad.Initialize(NzVector2f(0.f,0.f));

    cout<<"------Nombre feuilles après préparation : "<<quad.GetLeavesList().size()<<endl;
    cout<<"------Nombre de triangles : "<<quad.GetLeavesList().size()*32<<std::endl;


    ///creation du shader
	NzShader shader;
	if (!shader.Create(nzShaderLanguage_GLSL))
	{
		std::cout << "Failed to load shader" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Le fragment shader traite la couleur de nos pixels
	if (!shader.LoadFromFile(nzShaderType_Fragment, "slope_shader.frag"))
	{
		std::cout << "Failed to load fragment shader from file" << std::endl;
		// À la différence des autres ressources, le shader possède un log qui peut indiquer les erreurs en cas d'échec
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Le vertex shader (Transformation des vertices de l'espace 3D vers l'espace écran)
	if (!shader.LoadFromFile(nzShaderType_Vertex, "slope_shader.vert"))
	{
		std::cout << "Failed to load vertex shader from file" << std::endl;
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Une fois les codes sources de notre shader chargé, nous pouvons le compiler, afin de le rendre utilisable
	if (!shader.Compile())
	{
		std::cout << "Failed to compile shader" << std::endl;
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}


    NzString windowTitle("DynaTerrain slope optimization example");

	// Nous pouvons créer notre fenêtre ! (Via le constructeur directement ou par la méthode Create)
	NzRenderWindow window(NzVideoMode(1000,600,32),windowTitle,nzWindowStyle_Default);

	// Nous limitons les FPS à 100
	window.SetFramerateLimit(100);

	// La matrice de projection définit la transformation du vertice 3D à un point 2D
	NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(window.GetWidth())/window.GetHeight(), 1.f, 10000.f));

	// Notre fenêtre est créée, cependant il faut s'occuper d'elle, pour le rendu et les évènements
	NzClock secondClock, updateClock; // Des horloges pour gérer le temps
	unsigned int fps = 0; // Compteur de FPS

    NzMatrix4f matrix;
    matrix.MakeIdentity();
    NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(NzVector3f(0.f,0.f,0.f), NzVector3f::Forward()));

	// Quelques variables

	// Notre caméra
	//NzVector3f camPos(-6.f, 750.f, 2080.f);
	NzVector3f camPos(-600.f, 900.f, 1200.f);
	//NzVector3f camPos(22.f, 16.f, 65.f);
	NzEulerAnglesf camRot(0.f, 0.f, 0.f); // Les angles d'eulers sont bien plus facile à utiliser

	NzNode camera;
	camera.SetTranslation(camPos);
	camera.SetRotation(camRot);

	float camSpeed = 50.f;
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
				float speed2 = (NzKeyboard::IsKeyPressed(NzKeyboard::Key::LShift)) ? camSpeed*5 : camSpeed;
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

        // On active le shader et paramètrons le rendu
		NzRenderer::SetShader(&shader);

		// Notre scène 3D requiert un test de profondeur
		NzRenderer::Enable(nzRendererParameter_DepthTest, true);

		// Nous voulons avoir un fond noir
		NzRenderer::SetClearColor(25, 25, 25);

		// Et nous effaçons les buffers de couleur et de profondeur
		NzRenderer::Clear(nzRendererClear_Color | nzRendererClear_Depth);

		NzRenderer::SetFaceFilling(nzFaceFilling_Fill);

        // La matrice world est celle qui représente les transformations du modèle
        NzRenderer::SetMatrix(nzMatrixType_World, matrix);

        //On met à jour le terrain
        quad.Update(camera.GetTranslation());
        //On dessine le terrain
        quad.Render();
        //dispatcher.DrawAll();
		// Nous mettons à jour l'écran
		window.Display();

		fps++;

		// Toutes les secondes
		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(windowTitle + " (FPS: " + NzString::Number(fps) + ')' + "( Camera in : " + camera.GetTranslation() + ") (Updated Nodes : " + NzString::Number(quad.GetUpdatedNodeAmountPerFrame()) + ')');
			fps = 0;
			secondClock.Restart();
		}
	}

    return 0;
}
