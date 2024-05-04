#include "Game.h"
#include "Box2DHelper.h"
#include <iostream>

// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo)
{
    // Inicializaci�n de la ventana y configuraci�n de propiedades
    wnd = new RenderWindow(VideoMode(ancho, alto), titulo);
    wnd->setVisible(true);
    fps = 60;
    wnd->setFramerateLimit(fps);
    frameTime = 1.0f / fps;
    SetZoom(); // Configuraci�n de la vista del juego
    InitPhysics(); // Inicializaci�n del motor de f�sica
}

// Bucle principal del juego
void Game::Loop()
{
    while (wnd->isOpen())
    {
        wnd->clear(clearColor); // Limpiar la ventana
        DoEvents(); // Procesar eventos de entrada
        CheckCollitions(); // Comprobar colisiones
        UpdatePhysics(); // Actualizar la simulaci�n f�sica
        DrawGame(); // Dibujar el juego
        wnd->display(); // Mostrar la ventana
    }
}

// Actualizaci�n de la simulaci�n f�sica
void Game::UpdatePhysics()
{
    phyWorld->Step(frameTime, 8, 8); // Simular el mundo f�sico
    phyWorld->ClearForces(); // Limpiar las fuerzas aplicadas a los cuerpos
    phyWorld->DebugDraw(); // Dibujar el mundo f�sico para depuraci�n
}

// Inicializaci�n del motor de f�sica y los cuerpos del mundo f�sico
void Game::InitPhysics()
{
    // Inicializar el mundo f�sico con la gravedad por defecto
    phyWorld = new b2World(b2Vec2(0.0f, 9.8f));

    // Crear un renderer de debug para visualizar el mundo f�sico
    debugRender = new SFMLRenderer(wnd);
    debugRender->SetFlags(UINT_MAX);
    phyWorld->SetDebugDraw(debugRender);

    // Crear el suelo y el techo
    b2Body* groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    groundBody->SetTransform(b2Vec2(50.0f, 90.0f), 0.0f); // Bajar el suelo un poco para evitar la colisi�n con el techo

    b2Body* roofBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    roofBody->SetTransform(b2Vec2(50.0f, 10.0f), 0.0f); // Posicionar el techo m�s arriba

    // Crear las paredes
    b2Body* leftWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    leftWallBody->SetTransform(b2Vec2(5.0f, 50.0f), 0.0f); // Ajustar la posici�n X del muro izquierdo

    b2Body* rightWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    rightWallBody->SetTransform(b2Vec2(95.0f, 50.0f), 0.0f); // Ajustar la posici�n X del muro derecho

    // Crear un c�rculo que se controlar� con el teclado
    controlBody = Box2DHelper::CreateCircularDynamicBody(phyWorld, 5, 1.0f, 0.5, 0.1f);
    controlBody->SetTransform(b2Vec2(50.0f, 50.0f), 0.0f);

    // Asignar un coeficiente de restituci�n al cuerpo de la pelota
    b2Fixture* fixture = controlBody->GetFixtureList();
    b2FixtureDef fixtureDef;
    fixtureDef.restitution = 1.5f;
    fixture->SetRestitution(fixtureDef.restitution);

    // Crear obst�culos est�ticos
    obstacle1 = Box2DHelper::CreateCircularStaticBody(phyWorld, 10);
    obstacle1->SetTransform(b2Vec2(30.0f, 70.0f), 0.0f);

    obstacle2 = Box2DHelper::CreateCircularStaticBody(phyWorld, 10);
    obstacle2->SetTransform(b2Vec2(70.0f, 50.0f), 0.0f);
}

// Dibujo de los elementos del juego
void Game::DrawGame()
{
    // Dibujar el suelo
    sf::RectangleShape groundShape(sf::Vector2f(500, 5));
    groundShape.setFillColor(sf::Color::Red);
    groundShape.setPosition(0, 95);
    wnd->draw(groundShape);

    // Dibujar el techo
    sf::RectangleShape roofShape(sf::Vector2f(500, 5)); // Cambiar el nombre de la variable a roofShape
    roofShape.setFillColor(sf::Color::Blue); // Cambiar el color a azul para representar un techo
    roofShape.setPosition(0, 0); // Cambiar la posici�n del techo
    wnd->draw(roofShape); // Dibujar el techo en la ventana

// Dibujar las paredes
sf::RectangleShape leftWallShape(sf::Vector2f(5, alto)); // Alto de la ventana
leftWallShape.setFillColor(sf::Color::Blue);
leftWallShape.setPosition(0, 0); // Cambiar la posici�n X a 0 para que comience desde el borde izquierdo
wnd->draw(leftWallShape);

sf::RectangleShape rightWallShape(sf::Vector2f(10, alto)); // Alto de la ventana
rightWallShape.setFillColor(sf::Color::Cyan);
rightWallShape.setPosition(ancho - 10, 0); // Posicionar la pared derecha en el borde derecho de la ventana
wnd->draw(rightWallShape);

    // Dibujar el cuerpo de control (c�rculo)
    sf::CircleShape controlShape(5);
    controlShape.setFillColor(sf::Color::Magenta);
    controlShape.setPosition(controlBody->GetPosition().x - 5, controlBody->GetPosition().y - 5);
    wnd->draw(controlShape);
}

// Procesamiento de eventos de entrada
void Game::DoEvents()
{
    Event evt;
    while (wnd->pollEvent(evt))
    {
        switch (evt.type)
        {
        case Event::Closed:
            wnd->close(); // Cerrar la ventana si se presiona el bot�n de cerrar
            break;
        }
    }

    // Controlar el movimiento del cuerpo de control con el teclado
    // Segun la numeracion usada, cuando mas cerca de cero mas 
    // lento es el desplazamiento sobre ese eje
    controlBody->SetAwake(true);
    if (Keyboard::isKeyPressed(Keyboard::Left))
        controlBody->SetLinearVelocity(b2Vec2(-30.0f, 0.0f));
    if (Keyboard::isKeyPressed(Keyboard::Right))
        controlBody->SetLinearVelocity(b2Vec2(30.0f, 0.0f));
    if (Keyboard::isKeyPressed(Keyboard::Down))
        controlBody->SetLinearVelocity(b2Vec2(0.0f, 30.0f));
    if (Keyboard::isKeyPressed(Keyboard::Up))
        controlBody->SetLinearVelocity(b2Vec2(0.0f, -30.0f));
}

// Comprobaci�n de colisiones (a implementar m�s adelante)
void Game::CheckCollitions()
{
    // Implementaci�n de la comprobaci�n de colisiones
}

// Configuraci�n de la vista del juego
void Game::SetZoom()
{
    View camara;
    // Posicionamiento y tama�o de la vista
    camara.setSize(100.0f, 100.0f);
    camara.setCenter(50.0f, 50.0f);
    wnd->setView(camara); // Asignar la vista a la ventana
}



// Destructor de la clase
Game::~Game(void)
{ }