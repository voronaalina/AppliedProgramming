#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <optional>
#include <vector>

// 1. ФІЗИЧНІ КОНСТАНТИ
const double G = 6.674e-11;
const double ME = 5.972e24;
const double MM = 7.342e22;
const double RE = 6.371e6;
const double RM = 3.844e8;
const double PI = 3.14159265358979323846;

const int satCount = 3;
const double defaultOrbitRadius = 1.0e8; // 100,000 км
double currentOrbitRadius = defaultOrbitRadius;

const double realSatMass = 1000.0;
const double demoSatMass = 1.0e18; // збільшена маса для експ.2
double currentSatMass = realSatMass;

// Експеримент 1
const double speedValues[] = { 0.2, 0.5, 0.8, 1.0, 1.2 };
const int speedValueCount = 5;
int speedIndex = 3;
double speedFactor = 1.0;

// Експеримент 2
bool closeSatellites = false;
double satelliteAngleGap = 0.05;
const double gapValues[] = { 0.01, 0.05, 0.15, 0.30 };
const int gapValueCount = 4;
int gapIndex = 1;

struct Vec2 { double x = 0.0; double y = 0.0; };

struct Body
{
    double m = 0.0;
    Vec2 r, v, a;
    bool crashed = false;
};

struct Options
{
    bool moonGravity = true;
    bool satInteraction = true;
};

// 2. Початкові умови та фізична модель

Body makeSatellite(const Body& earth, double phi)
{
    const double v = speedFactor * std::sqrt(G * earth.m / currentOrbitRadius);

    Body satellite;
    satellite.m = currentSatMass;
    satellite.r = {
        earth.r.x + currentOrbitRadius * std::cos(phi),
        earth.r.y + currentOrbitRadius * std::sin(phi)
    };
    satellite.v = {
        earth.v.x - v * std::sin(phi),
        earth.v.y + v * std::cos(phi)
    };
    return satellite;
}

std::vector<Body> initBodies(int experiment)
{
    const double moonSpeed = std::sqrt(G * (ME + MM) / RM);

    Body earth, moon;
    earth.m = ME;
    earth.r = { -RM * MM / (ME + MM), 0.0 };
    earth.v = { 0.0, -moonSpeed * MM / (ME + MM) };

    moon.m = MM;
    moon.r = { RM * ME / (ME + MM), 0.0 };
    moon.v = { 0.0, moonSpeed * ME / (ME + MM) };

    std::vector<Body> bodies = { earth, moon };

    for (int i = 0; i < satCount; ++i)
    {
        double phi = 0.0;
        if (experiment == 3 && closeSatellites)
        {
            phi = satelliteAngleGap * i;
        }
        else
        {
            phi = 2.0 * PI * i / satCount;
        }
        bodies.push_back(makeSatellite(earth, phi));
    }

    return bodies;
}

bool interacts(const std::vector<Body>& bodies, size_t i, size_t j, const Options& options)
{
    if (bodies[i].crashed || bodies[j].crashed)
        return false;

    const bool iSatellite = i >= 2;
    const bool jSatellite = j >= 2;

    if (!options.moonGravity && ((i == 1 && jSatellite) || (j == 1 && iSatellite)))
        return false;

    if (!options.satInteraction && iSatellite && jSatellite)
        return false;

    return true;
}

void computeAccelerations(std::vector<Body>& bodies, const Options& options)
{
    for (auto& body : bodies)
        body.a = { 0.0, 0.0 };

    for (size_t i = 0; i < bodies.size(); ++i)
    {
        for (size_t j = i + 1; j < bodies.size(); ++j)
        {
            if (!interacts(bodies, i, j, options))
                continue;

            const double dx = bodies[j].r.x - bodies[i].r.x;
            const double dy = bodies[j].r.y - bodies[i].r.y;
            const double distance2 = dx * dx + dy * dy;
            if (distance2 < 1.0)
                continue;

            const double distance3 = distance2 * std::sqrt(distance2);

            bodies[i].a.x += G * bodies[j].m * dx / distance3;
            bodies[i].a.y += G * bodies[j].m * dy / distance3;
            bodies[j].a.x -= G * bodies[i].m * dx / distance3;
            bodies[j].a.y -= G * bodies[i].m * dy / distance3;
        }
    }
}

void updatePhysics(std::vector<Body>& bodies, double dt, const Options& options)
{
    computeAccelerations(bodies, options);

    for (auto& body : bodies)
    {
        if (body.crashed)
            continue;

        body.v.x += body.a.x * dt;
        body.v.y += body.a.y * dt;
        body.r.x += body.v.x * dt;
        body.r.y += body.v.y * dt;
    }

    for (size_t i = 2; i < bodies.size(); ++i)
    {
        const double dx = bodies[i].r.x - bodies[0].r.x;
        const double dy = bodies[i].r.y - bodies[0].r.y;
        if (std::sqrt(dx * dx + dy * dy) < RE)
        {
            bodies[i].crashed = true;
            bodies[i].v = { 0.0, 0.0 };
        }
    }
}

double totalEnergy(const std::vector<Body>& bodies, const Options& options)
{
    double energy = 0.0;

    for (size_t i = 0; i < bodies.size(); ++i)
    {
        if (bodies[i].crashed)
            continue;

        energy += 0.5 * bodies[i].m *
            (bodies[i].v.x * bodies[i].v.x + bodies[i].v.y * bodies[i].v.y);

        for (size_t j = i + 1; j < bodies.size(); ++j)
        {
            if (!interacts(bodies, i, j, options))
                continue;

            const double dx = bodies[j].r.x - bodies[i].r.x;
            const double dy = bodies[j].r.y - bodies[i].r.y;
            const double distance = std::sqrt(dx * dx + dy * dy);
            if (distance > 0.0)
                energy -= G * bodies[i].m * bodies[j].m / distance;
        }
    }
    return energy;
}

double satelliteDistance(const std::vector<Body>& bodies, size_t index)
{
    const double dx = bodies[index].r.x - bodies[0].r.x;
    const double dy = bodies[index].r.y - bodies[0].r.y;
    return std::sqrt(dx * dx + dy * dy);
}

// 3. Візуалізація

int main()
{
    const float W = 1000.f;
    const float H = 700.f;

    sf::RenderWindow window(sf::VideoMode({ 1000, 700 }),
        "Research: Earth-Moon-Satellites Orbital Mechanics");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("../resources/fonts/arial.ttf"))
        return 1;

    sf::Text infoText(font, "", 15);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition({ 15.f, 12.f });

    sf::Text helpText(font,
        "Space: pause | R: restart | Up/Down: time speed | Z/X: dt down/up\n"
        "1: Baseline | 2: Speed Impact (Left/Right) | 3: Spacing (G: change gap)\n"
        "4: High Orbit / Moon | M/I: toggle gravity | Wheel: zoom | Esc: exit",
        12);
    helpText.setFillColor(sf::Color(180, 180, 180));
    helpText.setPosition({ 15.f, H - 58.f });

    sf::CircleShape earthShape(2.f, 100);
    earthShape.setFillColor(sf::Color(60, 120, 220));

    sf::CircleShape moonShape(6.f);
    moonShape.setOrigin({ 6.f, 6.f });
    moonShape.setFillColor(sf::Color(210, 210, 210));

    sf::CircleShape satelliteShape(4.f);
    satelliteShape.setOrigin({ 4.f, 4.f });

    Options options;
    std::vector<Body> bodies;
    std::vector<std::vector<Vec2>> trails;

    double simTime = 0.0;
    double accumulator = 0.0;
    double dt = 10.0;
    double timeScale = 15000.0;
    double scale = 5.0e-7;
    double initialEnergy = 0.0;
    double minDistance = defaultOrbitRadius;
    double maxDistance = defaultOrbitRadius;
    bool paused = false;
    int experiment = 1;
    sf::Clock clock;

    auto restart = [&]()
        {
            bodies = initBodies(experiment);
            trails.assign(bodies.size(), std::vector<Vec2>());
            simTime = 0.0;
            accumulator = 0.0;
            initialEnergy = totalEnergy(bodies, options);
            minDistance = satelliteDistance(bodies, 2);
            maxDistance = minDistance;
        };

    auto setExperiment = [&](int number)
        {
            experiment = number;
            options.moonGravity = true;
            options.satInteraction = true;
            speedIndex = 3;
            speedFactor = speedValues[speedIndex];
            currentOrbitRadius = defaultOrbitRadius;
            closeSatellites = false;
            currentSatMass = realSatMass;
            scale = 5.0e-7;

            if (experiment == 1)
            {
                currentOrbitRadius = 1.0e8;
            }
            else if (experiment == 2)
            {
                currentOrbitRadius = 1.0e8;
            }
            else if (experiment == 3)
            {
                closeSatellites = true;
                currentSatMass = demoSatMass;
                satelliteAngleGap = gapValues[gapIndex];
            }
            else if (experiment == 4)
            {
                currentOrbitRadius = 3.2e8; // вища орбіта взаємодії з Місяцем
                scale = 1.5e-7;
            }

            restart();
        };

    restart();

    auto toScreen = [&](const Vec2& p)
        {
            return sf::Vector2f(float(W / 2 + p.x * scale), float(H / 2 - p.y * scale));
        };

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
                else if (key->code == sf::Keyboard::Key::Space)
                    paused = !paused;
                else if (key->code == sf::Keyboard::Key::R)
                    restart();
                else if (key->code == sf::Keyboard::Key::Up)
                    timeScale *= 2.0;
                else if (key->code == sf::Keyboard::Key::Down)
                    timeScale /= 2.0;
                else if (key->code == sf::Keyboard::Key::X)
                {
                    dt *= 2.0;
                    restart();
                }
                else if (key->code == sf::Keyboard::Key::Z)
                {
                    dt /= 2.0;
                    restart();
                }
                else if (key->code == sf::Keyboard::Key::Left || key->code == sf::Keyboard::Key::Right)
                {
                    if (experiment == 2)
                    {
                        if (key->code == sf::Keyboard::Key::Left)
                            speedIndex = std::max(0, speedIndex - 1);
                        else
                            speedIndex = std::min(speedValueCount - 1, speedIndex + 1);

                        speedFactor = speedValues[speedIndex];
                        restart();
                    }
                }
                else if (key->code == sf::Keyboard::Key::G && experiment == 3)
                {
                    gapIndex = (gapIndex + 1) % gapValueCount;
                    satelliteAngleGap = gapValues[gapIndex];
                    restart();
                }
                else if (key->code == sf::Keyboard::Key::M)
                {
                    options.moonGravity = !options.moonGravity;
                    restart();
                }
                else if (key->code == sf::Keyboard::Key::I)
                {
                    options.satInteraction = !options.satInteraction;
                    restart();
                }
                else if (key->code == sf::Keyboard::Key::Num1)
                    setExperiment(1);
                else if (key->code == sf::Keyboard::Key::Num2)
                    setExperiment(2);
                else if (key->code == sf::Keyboard::Key::Num3)
                    setExperiment(3);
                else if (key->code == sf::Keyboard::Key::Num4)
                    setExperiment(4);

                timeScale = std::clamp(timeScale, 10.0, 100000.0);
                dt = std::clamp(dt, 1.0, 1000.0);
            }

            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                scale *= wheel->delta > 0 ? 1.2 : 1.0 / 1.2;
                scale = std::clamp(scale, 5.0e-8, 2.0e-5);
            }
        }

        const double realDt = clock.restart().asSeconds();
        if (!paused)
        {
            accumulator += realDt * timeScale;
            int steps = 0;
            while (accumulator >= dt && steps < 5000)
            {
                updatePhysics(bodies, dt, options);
                accumulator -= dt;
                simTime += dt;
                ++steps;

                const double r = satelliteDistance(bodies, 2);
                minDistance = std::min(minDistance, r);
                maxDistance = std::max(maxDistance, r);
            }
            if (steps == 5000)
                accumulator = 0.0;

            for (size_t i = 2; i < bodies.size(); ++i)
            {
                trails[i].push_back({
                    bodies[i].r.x - bodies[0].r.x,
                    bodies[i].r.y - bodies[0].r.y
                    });
                if (trails[i].size() > 700)
                    trails[i].erase(trails[i].begin());
            }
        }

        window.clear(sf::Color::Black);

        for (size_t i = 2; i < bodies.size(); ++i)
        {
            sf::VertexArray line(sf::PrimitiveType::LineStrip);
            for (const auto& point : trails[i])
                line.append(sf::Vertex{ toScreen(point), sf::Color(100, 100, 100) });
            window.draw(line);
        }

        const float earthRadiusPixels = std::max(float(RE * scale), 2.f);
        earthShape.setRadius(earthRadiusPixels);
        earthShape.setOrigin({ earthRadiusPixels, earthRadiusPixels });
        earthShape.setPosition(toScreen({ 0.0, 0.0 }));
        window.draw(earthShape);

        moonShape.setPosition(toScreen({
            bodies[1].r.x - bodies[0].r.x,
            bodies[1].r.y - bodies[0].r.y
            }));
        window.draw(moonShape);

        for (size_t i = 2; i < bodies.size(); ++i)
        {
            sf::Color color = (i == 2) ? sf::Color::Green : sf::Color::White;
            if (bodies[i].crashed)
                color = sf::Color::Red;
            satelliteShape.setFillColor(color);
            satelliteShape.setPosition(toScreen({
                bodies[i].r.x - bodies[0].r.x,
                bodies[i].r.y - bodies[0].r.y
                }));
            window.draw(satelliteShape);
        }

        int crashedCount = 0;
        for (size_t i = 2; i < bodies.size(); ++i)
            if (bodies[i].crashed)
                ++crashedCount;

        const double currentDistance = satelliteDistance(bodies, 2);
        const double energy = totalEnergy(bodies, options);
        const double energyError = std::abs(energy - initialEnergy) /
            std::max(std::abs(initialEnergy), 1.0);

        const char* expDesc = "";
        if (experiment == 1) expDesc = "1: Baseline Model";
        if (experiment == 2) expDesc = "2: Initial Speed Impact (Left/Right changes speed)";
        if (experiment == 3) expDesc = "3: Satellite Spacing & Mutual Gravity (G changes gap)";
        if (experiment == 4) expDesc = "4: High Orbit / Moon Perturbations";

        char text[1024];
        std::snprintf(text, sizeof(text),
            "Research Experiment: %s \n"
            "Time: %.2f h    dt: %.1f s (Z/X)    time speed: x%.0f%s\n"
            "Speed Multiplier: %.2f    Orbit Radius: %.0f thousand km\n"
            "Satellites: %d    Crashed: %d\n"
            "Satellite 1 Distance from Earth: %.2f thousand km\n"
            "Min / Max Distance: %.2f / %.2f thousand km\n"
            "Relative Total-Energy Change: %.3e",
            expDesc, simTime / 3600.0, dt, timeScale,
            paused ? " [PAUSED]" : "",
            speedFactor, currentOrbitRadius / 1.0e3,
            satCount, crashedCount,
            currentDistance / 1.0e6,
            minDistance / 1.0e6, maxDistance / 1.0e6,
            energyError);

        infoText.setString(text);
        window.draw(infoText);
        window.draw(helpText);
        window.display();
    }

    return 0;
}