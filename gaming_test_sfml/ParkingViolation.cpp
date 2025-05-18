#include "ParkingViolation.h"
#include <cmath>
#include <SFML/System/Time.hpp>    // ��� sf::Time
#include <vector>                  // ��� std::vector
ParkingViolation::ParkingViolation(const sf::Vector2f &pos, const sf::Texture& carTex, const sf::Texture& questionTex, 
	unsigned frameCount,
	sf::Vector2u frameSize,
	float frameDuration):
	mCar(carTex), mQuestion(questionTex), mFrameCount(frameCount), mFrameDuration(frameDuration)
{
	mCar.setScale(1.5f, 1.5f);
	mCar.setPosition(pos);
	for (unsigned i = 0; i < mFrameCount; ++i) {
		mFrames.emplace_back(
			i * frameSize.x, 0,
			frameSize.x, frameSize.y
		);
	}
	// ��������� ������ ����
	mQuestion.setTextureRect(mFrames[0]);

	// ��������� ����� �������� ������� ������� � ��� ��������
	mQuestion.setOrigin(frameSize.x / 2.f, frameSize.y / 2.f);

	// �������� �?� ���� ���� �����
	mQuestion.setScale(1.5f, 1.5f);
	auto carBounds = mCar.getGlobalBounds();
	auto questionBounds = mQuestion.getLocalBounds();

	// ����������� �?� �� ������ ����� ������
	float qx = carBounds.left + carBounds.width * 0.5f;
	float qy = carBounds.top - questionBounds.height*0.25f - 14.f;

	mQuestion.setPosition(qx, qy);
	
}

void ParkingViolation::update(const sf::Vector2f& playerPos, sf::Time dt)
{
	if (mResolved) return; // ��� ������
	float dx = playerPos.x - mCar.getPosition().x;
	float dy = playerPos.y - mCar.getPosition().y;
	mPlayerNear = (std::sqrt(dx * dx + dy * dy) < mTriggerDist);

	// ���� ����� ������ � ��������� ������
	//if (mPlayerNear) {
		mElapsedTime += dt.asSeconds();
		if (mElapsedTime >= mFrameDuration) {
			mElapsedTime -= mFrameDuration;
			mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
			mQuestion.setTextureRect(mFrames[mCurrentFrame]);
		}
	//}
}


void ParkingViolation::render(sf::RenderTarget& target)
{
	if (mResolved) return;
	target.draw(mCar);
	/*if (mPlayerNear)
		*/target.draw(mQuestion);
}
