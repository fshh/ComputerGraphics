#pragma once

#include "Renderable.h"
#include "PointLight.h"
#include <memory>

class UnitQuad : public Renderable
{
protected:
	QList<std::shared_ptr<PointLight>> lights_;
	float sign_;
public:
	UnitQuad();
	virtual ~UnitQuad();

	// Our init method is much easier now.  We only need a texture!
	virtual void init(const QString& textureFile, QList<std::shared_ptr<PointLight>>& lights);
	virtual void update(const qint64 msSinceLastFrame) override;

private:

};
