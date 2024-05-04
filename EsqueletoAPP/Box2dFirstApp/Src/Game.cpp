#include "Game.h"
#include "Box2DHelper.h"
#include <iostream>

// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo)
{
    // Inicialización de la ventana y configuración de propiedades
    wnd = new RenderWindow(VideoMode(ancho, alto), titulo);
    wnd->setVisible(true);
    fps = 60;
    wnd->setFramerateLimit(fps);
    frameTime = 1.0f / fps;
    SetZoom(); // Configuración de la vista del juego
    InitPhysics(); // Inicialización del motor de física
}

// Bucle principal del juego
void Game::Loop()
{
    while (wnd->isOpen())
    {
        wnd->clear(clearColor); // Limpiar la ventana
        DoEvents(); // Procesar eventos de entrada
        CheckCollitions(); // Comprobar colisiones
        UpdatePhysics(); // Actualizar la simulación física
        DrawGame(); // Dibujar el juego
        wnd->display(); // Mostrar la ventana
    }
}

// Actualización de la simulación física
void Game::UpdatePhysics()
{
    phyWorld->Step(frameTime, 8, 8); // Simular el mundo físico
    phyWorld->ClearForces(); // Limpiar las fuerzas aplicadas a los cuerpos
    phyWorld->DebugDraw(); // Dibujar el mundo físico para depuración
}

// Inicialización del motor de física y los cuerpos del mundo físico
void Game::InitPhysics()
{
    // Inicializar el mundo físico con la gravedad por defecto
    phyWorld = new b2World(b2Vec2(0.0f, 9.8f));

    // Crear un renderer de debug para visualizar el mundo físico
    debugRender = new SFMLRenderer(wnd);
    debugRender->SetFlags(UINT_MAX);
    phyWorld->SetDebugDraw(debugRender);

    // Crear el suelo y el techo
    b2Body* groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    groundBody->SetTransform(b2Vec2(50.0f, 90.0f), 0.0f); // Bajar el suelo un poco para evitar la colisión con el techo

    b2Body* roofBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    roofBody->SetTransform(b2Vec2(50.0f, 10.0f), 0.0f); // Posicionar el techo más arriba

    // Crear las paredes
    b2Body* leftWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    leftWallBody->SetTransform(b2Vec2(5.0f, 50.0f), 0.0f); // Ajustar la posición X del muro izquierdo

    b2Body* rightWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    rightWallBody->SetTransform(b2Vec2(95.0f, 50.0f), 0.0f); // Ajustar la posición X del muro derecho

    // Crear un círculo que se controlará con el teclado
    controlBody = Box2DHelper::CreateCircularDynamicBody(phyWorld, 5, 1.0f, 0.5, 0.1f);
    controlBody->SetTransform(b2Vec2(50.0f, 50.0f), 0.0f);

    // Asignar un coeficiente de restitución al cuerpo de la pelota
    b2Fixture* fixture = controlBody->GetFixtureList();
    b2FixtureDef fixtureDef;
    fixtureDef.restitution = 1.5f;
    fixture->SetRestitution(fixtureDef.restitution);

    // Crear obstáculos estáticos
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
    roofShape.setPosition(0, 0); // Cambiar la posición del techo
    wnd->draw(roofShape); // Dibujar el techo en la ventana

// Dibujar las paredes
sf::RectangleShape leftWallShape(sf::Vector2f(5, alto)); // Alto de la ventana
leftWallShape.setFillColor(sf::Color::Blue);
leftWallShape.setPosition(0, 0); // Cambiar la posición X a 0 para que comience desde el borde izquierdo
wnd->draw(leftWallShape);

sf::RectangleShape rightWallShape(sf::Vector2f(10, alto)); // Alto de la ventana
rightWallShape.setFillColor(sf::Color::Cyan);
rightWallShape.setPosition(ancho - 10, 0); // Posicionar la pared derecha en el borde derecho de la ventana
wnd->draw(rightWallShape);

    // Dibujar el cuerpo de control (círculo)
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
            wnd->close(); // Cerrar la ventana si se presiona el botón de cerrar
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

// Comprobación de colisiones (a implementar más adelante)
void Game::CheckCollitions()
{
    // Implementación de la comprobación de colisiones
}

// Configuración de la vista del juego
void Game::SetZoom()
{
    View camara;
    // Posicionamiento y tamaño de la vista
    camara.setSize(100.0f, 100.0f);
    camara.setCenter(50.0f, 50.0f);
    wnd->setView(camara); // Asignar la vista a la ventana
}



// Destructor de la clase
Game::~Game(void)
{ }