#pragma once
#include <vector>
#include <memory>
#include "Incident.h"

class IncidentManager {
public:
	void addIncident(std::unique_ptr<Incident> incident);
	void update(sf::Time deltaTime);
	void render(sf::RenderWindow& window);

private:
	std::vector<std::unique_ptr<Incident>> mIncidents;
};