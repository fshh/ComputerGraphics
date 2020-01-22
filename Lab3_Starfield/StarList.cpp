#include "StarList.h"
#include <QtCore/QtMath>

StarList::StarList(unsigned int numStars, float spread, float speed) : spread_(spread), speed_(speed)
{
    for (int i = 0; i < numStars; ++i) {
        stars_.push_back({ 0, 0, 0, 0, QColor(255, 255, 255) });
        initStar(i);
    }
}

StarList::~StarList()
{}

void StarList::initStar(unsigned int idx)
{
	if (idx >= stars_.size()) {
		return;
	}

	// randomGen.generateDouble will generate a random number [0,1)
    // Generate positions: (-1, 1)
	stars_[idx].x = 2.0f * (randomGen_.generateDouble() - 0.5f) * spread_;
	stars_[idx].y = 2.0f * (randomGen_.generateDouble() - 0.5f) * spread_;
	stars_[idx].z = (randomGen_.generateDouble() + 0.0001f) * spread_;
	stars_[idx].speed = speed_;
	stars_[idx].color = QColor(255, 255, 255);
}

void StarList::updateAndRender(QImage& image, float delta, const QSize& windowSize)
{
    // We compute half width and half height, because we are
    // only working with half of the screen.     
    float halfWidth = 800 / 2.0f;
    float halfHeight = 600 / 2.0f;

    // Note the conversion to radians
    float tanHalfFOV = qTan(qDegreesToRadians((float)70 / 2));

    // Iterate through all of your stars 
    for (int i = 0; i < stars_.size(); i++) {
        stars_[i].z -= delta * speed_;

        if (stars_[i].z <= 0) {
            initStar(i);
            continue;
        }

        // Perspective achieved by dividing x and y by (half the tangent of our FOV * z)
        float givePerspective = tanHalfFOV * stars_[i].z;

        // Apply our perspective
        int x = (int)((stars_[i].x / (givePerspective)) * halfWidth + halfWidth);
        int y = (int)((stars_[i].y / (givePerspective)) * halfHeight + halfHeight);
		
		// spiral stars around center
		float cos = qCos(stars_[i].z);
		float sin = qSin(stars_[i].z);
		// translate point back to origin
		x -= halfWidth;
		y -= halfHeight;
		// rotate point
		int x1 = x * cos - y * sin;
		int y1 = x * sin + y * cos;
		// translate point back
		x = x1 + halfWidth;
		y = y1 + halfHeight;
		
        // Reinitialize a star
        if (x <0 || x >= windowSize.width()) {
            initStar(i);
            continue;
        }
        if (y <0 || y >= windowSize.height()) {
            initStar(i);
            continue;
        }

        // Draw a pixel to the renderer.
        image.setPixelColor(x, y, stars_[i].color);
    }
}
