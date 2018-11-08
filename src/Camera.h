/*

*/
#pragma once
#include <glm/glm.hpp>

namespace Camera {

	/// カメラデータ.
	struct CameraData {
		glm::vec3 position;
		glm::vec3 target;
		glm::vec3 up = glm::vec3(0, 1, 0);
	};

	class Camera {
	public:
		Camera() = default;
		
		Camera& Instance() {
			static Camera instance;
			return instance;
		};

		void Init(const glm::vec3& targetPos, float rotate, float distance);
		void setTarget(const glm::vec3& targetPos);
		void CalcPosition(float rotate, float distance);
		void Update();
		
		float rotate() { return m_rotate; }

	private:
		CameraData m_cameraInfo;
		glm::vec3 m_targetPos;
		
		float m_cameraZ;
		float m_distance;
		float m_rotate;
	};
}