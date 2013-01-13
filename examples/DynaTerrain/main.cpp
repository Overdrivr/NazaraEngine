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

//TODO : rajouter des constructeurs par d�faut � chacun des bruits
//TODO : Les bruits ne peuvent pas �tre instanci�s par pointeurs ??

//TODO : Remplacer calcul variation moyenne de pente par variation max
//TODO2 : Un patch pourrait transmettre � son fils certaines valeurs de hauteur communes
int main()
{
    /*NzCirclef circle(-325,970,200);
    NzRectf rect(-1000,-1000,2000,2000);
    cout<<"intersects "<<circle.Intersect(rect)<<endl;
    cout<<"contains "<<circle.Contains(rect)<<endl;*/
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

    ///Partie r�serv�e au terrain
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

    cout<<"------Nombre feuilles apr�s pr�paration : "<<quad.GetLeavesList().size()<<endl;
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
		// � la diff�rence des autres ressources, le shader poss�de un log qui peut indiquer les erreurs en cas d'�chec
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Le vertex shader (Transformation des vertices de l'espace 3D vers l'espace �cran)
	if (!shader.LoadFromFile(nzShaderType_Vertex, "slope_shader.vert"))
	{
		std::cout << "Failed to load vertex shader from file" << std::endl;
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Une fois les codes sources de notre shader charg�, nous pouvons le compiler, afin de le rendre utilisable
	if (!shader.Compile())
	{
		std::cout << "Failed to compile shader" << std::endl;
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}


    NzString windowTitle("DynaTerrain slope optimization example");

	// Nous pouvons cr�er notre fen�tre ! (Via le constructeur directement ou par la m�thode Create)
	NzRenderWindow window(NzVideoMode(1000,600,32),windowTitle,nzWindowStyle_Default);

	// Nous limitons les FPS � 100
	window.SetFramerateLimit(100);

	// La matrice de projection d�finit la transformation du vertice 3D � un point 2D
	NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(window.GetWidth())/window.GetHeight(), 1.f, 10000.f));

	// Notre fen�tre est cr��e, cependant il faut s'occuper d'elle, pour le rendu et les �v�nements
	NzClock secondClock, updateClock; // Des horloges pour g�rer le temps
	unsigned int fps = 0; // Compteur de FPS

    NzMatrix4f matrix;
    matrix.MakeIdentity();
    NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(NzVector3f(0.f,0.f,0.f), NzVector3f::Forward()));

	// Quelques variables

	// Notre cam�ra
	//NzVector3f camPos(-6.f, 750.f, 2080.f);
	NzVector3f camPos(-600.f, 900.f, 1200.f);
	//NzVector3f camPos(22.f, 16.f, 65.f);
	NzEulerAnglesf camRot(0.f, 0.f, 0.f); // Les angles d'eulers sont bien plus facile � utiliser

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

				// En revanche, ici la hauteur est toujours la m�me, peu importe notre orientation
				if (NzKeyboard::IsKeyPressed(NzKeyboard::Space))
					camera.Translate(up * speed * elapsedTime, nzCoordSys_Global);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl))
					camera.Translate(up * speed * elapsedTime, nzCoordSys_Global);

			updateClock.Restart();
		}

        NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(camera.GetDerivedTranslation(), camera.GetDerivedTranslation() + camera.GetDerivedRotation() * NzVector3f::Forward()));

        // On active le shader et param�trons le rendu
		NzRenderer::SetShader(&shader);

		// Notre sc�ne 3D requiert un test de profondeur
		NzRenderer::Enable(nzRendererParameter_DepthTest, true);

		// Nous voulons avoir un fond noir
		NzRenderer::SetClearColor(25, 25, 25);

		// Et nous effa�ons les buffers de couleur et de profondeur
		NzRenderer::Clear(nzRendererClear_Color | nzRendererClear_Depth);

		NzRenderer::SetFaceFilling(nzFaceFilling_Fill);

        // La matrice world est celle qui repr�sente les transformations du mod�le
        NzRenderer::SetMatrix(nzMatrixType_World, matrix);

        //On met � jour le terrain
        quad.Update(camera.GetTranslation());
        //On dessine le terrain
        quad.Render();
        //dispatcher.DrawAll();
		// Nous mettons � jour l'�cran
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
