//#include "IncidentManager.h"
//
//void IncidentManager::addIncident(std::unique_ptr<Incident> incident)
//{
//	mIncidents.push_back(std::move(incident));
//
//}
//
//void IncidentManager::update(sf::Time deltaTime)
//{
//	for (auto it = mIncidents.begin(); it != mIncidents.end();) {
//		(*it)->update(deltaTime);
//		if ((*it)->isCompleted()) {
//			it = mIncidents.erase(it);
//		}
//		else {
//			++it;
//		}
//	}
//}
//
//void IncidentManager::render(sf::RenderTarget& target) const
//{
//	for (auto& incident : mIncidents) {
//		incident->render(target);
//	}
//}
