#pragma once
#include "Mesh.h"
#include "Transform.h"
#include <memory>

class GameEntity
{
	private:
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Transform> transform;

	public:
		GameEntity(std::shared_ptr<Mesh> mesh);

		std::shared_ptr<Mesh> GetMesh();
		std::shared_ptr<Transform> GetTransform();

		void SetMesh(std::shared_ptr<Mesh> mesh);


};

