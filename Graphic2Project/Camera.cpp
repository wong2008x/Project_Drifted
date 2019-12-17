#include "Camera.h"


Camera::Camera()
{
	camPosition = XMVectorSet(0.0f, 10.0f, -20.0f, 0.0f);
	camTarget = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
}

void Camera::UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);


	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	camPosition += moveLeftRight * camRight;
	camPosition += moveBackForward * camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;
}
void Camera::Reset()
{
	camPosition = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
	camTarget = XMVectorSet(-10.0f, 1.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
}
void Camera::LoadViewMatrix(XMFLOAT4X4 &_view)
{
	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(camPosition, camTarget, camUp));
}
Camera::~Camera()
{
}

