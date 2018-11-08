#include "Camera.h"

namespace Camera {

	void Camera::Init(const glm::vec3& targetPos,float rotate, float distance)
	{
		m_cameraInfo.target = targetPos;
		m_distance = distance;
		m_rotate = rotate;
		CalcPosition(rotate, m_distance);
	}

	void Camera::setTarget(const glm::vec3 & targetPos)
	{
		m_cameraInfo.target = targetPos;
	}

	void Camera::CalcPosition(float rotate, float distance)
	{
		float x = cos(rotate) * distance;
		float z = sin(rotate) * distance;

		m_cameraInfo.position = glm::vec3(m_targetPos.x + x, m_targetPos.y + 20, m_targetPos.z + z);
	}

	void Camera::Update()
	{
		this->setTarget(m_cameraInfo.target);
		this->CalcPosition(m_cameraZ, m_distance);
	}
}