#include "RenderingGame.h"
#include "GameException.h"
#include "FirstPersonCamera.h"
#include "TriangleDemo.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "ModelFromFile.h"
#include "FpsComponent.h"
#include "RenderStateHelper.h"
#include "ObjectDiffuseLight.h"
#include "SamplerStates.h"
#include "RasterizerStates.h"
#include <algorithm>
#include "Model.h"
#include <WICTextureLoader.h>
#define NOMINMAX

//display score
#include <SpriteFont.h>
#include <sstream>

#include <DirectXMath.h>
using namespace DirectX;
typedef DirectX::XMFLOAT3 Vector3;

namespace Rendering
{
	;

	const XMFLOAT4 RenderingGame::BackgroundColor = { 0.75f, 0.75f, 0.75f, 1.0f };

	RenderingGame::RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand)
		: Game(instance, windowClass, windowTitle, showCommand),
		mDemo(nullptr), mDirectInput(nullptr), mKeyboard(nullptr), mMouse(nullptr), mModel1(nullptr),
		mFpsComponent(nullptr), mRenderStateHelper(nullptr), mObjectDiffuseLight(nullptr),
		mModel2(nullptr), mModel3(nullptr), mModel4(nullptr), mModel5(nullptr), mModel7(nullptr),
		mModel8(nullptr), mScore(0), mSpriteBatch(nullptr), mSpriteFont(nullptr), mMainMenuTexture(nullptr),

		mChickenPosition(XMFLOAT3(0.0f, 0.4f, 0.0f)),
		mChickenDirection(XMFLOAT3(1.0f, 0.0f, 0.0f)),
		mChickenSpeed(5.0f),
		mchickenAutoMove(true),
		mChangeDirectionTimer(0.0f),
		mChangeDirectionInterval(1.0f),
		mChickenAreaMinX(-20.0f),
		mChickenAreaMaxX(20.0f),
		mChickenAreaMinZ(-20.0f),
		mChickenAreaMaxZ(20.0f)

	{
		mDepthStencilBufferEnabled = true;
		mMultiSamplingEnabled = true;
	}

	RenderingGame::~RenderingGame()
	{
	}

	void RenderingGame::Initialize()
	{

		mCamera = new FirstPersonCamera(*this);
		mComponents.push_back(mCamera);
		mServices.AddService(Camera::TypeIdClass(), mCamera);
		ShowCursor(TRUE);

		mDemo = new TriangleDemo(*this, *mCamera);
		mComponents.push_back(mDemo);
		HRESULT hr = DirectX::CreateWICTextureFromFile(
			mDirect3DDevice,
			mDirect3DDeviceContext,
			L"Content\\Textures\\MainMenu.png",
			nullptr,
			&mMainMenuTexture
		);

		if (FAILED(hr))
		{
			throw GameException("Could not load MainMenu.png", hr);
		}

		//Remember that the component is a management class for all objects in the D3D rendering engine. 
		//It provides a centralized place to create and release objects. 
		//NB: In C++ and other similar languages, to instantiate a class is to create an object.
		if (FAILED(DirectInput8Create(mInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&mDirectInput, nullptr)))
		{
			throw GameException("DirectInput8Create() failed");
		}

		mKeyboard = new Keyboard(*this, mDirectInput);
		mComponents.push_back(mKeyboard);
		mServices.AddService(Keyboard::TypeIdClass(), mKeyboard);

		mMouse = new Mouse(*this, mDirectInput);
		mComponents.push_back(mMouse);
		mServices.AddService(Mouse::TypeIdClass(), mMouse);

		//SetPosition(rotateX, rotateY, rotateZ, scale, translateX, translateY, translateZ);

		mModel1 = new ModelFromFile(*this, *mCamera, "Content\\Models\\chicken_root.fbx", L"A Chicken", 20, L"Content\\Textures\\chicken_color.jpg");
		mModel1->SetPosition(0.0f, 2.7f, 0.0f, 0.001f, 0.0f, 0.4f, 0.0f);
		mComponents.push_back(mModel1);

		ModelFromFile* model = new ModelFromFile(*this, *mCamera, "Content\\Models\\bench.3ds", L"A Bench", 10, L"Content\\Textures\\bench.jpg");
		model->SetPosition(-1.57f, 3.13f, 0.0f, 0.007f, 12.0f, 0.6f, -3.9f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\bench.3ds", L"A Bench", 10, L"Content\\Textures\\bench.jpg");
		model->SetPosition(-1.57f, 3.13f, 0.0f, 0.007f, 6.0f, 0.6f, -3.9f);
		mComponents.push_back(model);

		mModel3 = new ModelFromFile(*this, *mCamera, "Content\\Models\\Barn.fbx", L"A Barn", 10, L"Content\\Textures\\barn.jpg");
		mModel3->SetPosition(-1.57f, 3.13f, 0.0f, 0.7f, -10.0f, 2.1f, 9.0f);
		mComponents.push_back(mModel3);

		mModel5 = new ModelFromFile(*this, *mCamera, "Content\\Models\\silo.fbx", L"A silo", 10, L"Content\\Textures\\silos.jpg");
		mModel5->SetPosition(-1.57f, 9.4f, 0.0f, 0.5f, -15.0f, 4.5f, 15.0f);
		mComponents.push_back(mModel5);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Haystack.fbx", L"A haystack", 10, L"Content\\Textures\\haystack.jpg");
		model->SetPosition(-1.57f, 9.4f, 0.0f, 1.0f, -15.0f, 1.0f, 5.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Haystack.fbx", L"A haystack", 10, L"Content\\Textures\\haystack.jpg");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 2.0f, -15.0f, 2.5f, -15.0f);
		mComponents.push_back(model);

		mModel7 = new ModelFromFile(*this, *mCamera, "Content\\Models\\Baleofhay.fbx", L"A Baleofhay", 10, L"Content\\Textures\\Baleofhay.jpg");
		mModel7->SetPosition(-1.57f, 9.4f, 0.0f, 1.0f, -6.0f, 1.0f, 6.0f);
		mComponents.push_back(mModel7);

		mModel8 = new ModelFromFile(*this, *mCamera, "Content\\Models\\Baleofhay2.fbx", L"A Baleofhay2", 10, L"Content\\Textures\\Baleofhay2.jpg");
		mModel8->SetPosition(-1.57f, 5.4f, 0.0f, 2.5f, -9.0f, 0.3f, -13.0f);
		mComponents.push_back(mModel8);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\dirt1.fbx", L"A dirt", 10, L"Content\\Textures\\dirt1.png");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 2.0f, 12.0f, 0.0f, 12.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\dirt1.fbx", L"A dirt", 10, L"Content\\Textures\\dirt1.png");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 2.0f, 12.0f, 0.0f, 6.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\dirt1.fbx", L"A dirt", 10, L"Content\\Textures\\dirt1.png");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 2.0f, 6.0f, 0.0f, 6.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\dirt1.fbx", L"A dirt", 10, L"Content\\Textures\\dirt1.png");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 2.0f, 6.0f, 0.0f, 12.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\dirt1.fbx", L"A dirt", 10, L"Content\\Textures\\dirt1.png");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 2.0f, 6.0f, 0.0f, 0.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\dirt1.fbx", L"A dirt", 10, L"Content\\Textures\\dirt1.png");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 2.0f, 12.0f, 0.0f, 0.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 1.58f, 0.0f, 0.7f, 15.0f, 0.0f, -3.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 1.58f, 0.0f, 0.7f, 13.65f, 0.0f, -3.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 1.58f, 0.0f, 0.7f, 12.3f, 0.0f, -3.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 1.58f, 0.0f, 0.7f, 10.95f, 0.0f, -3.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 1.58f, 0.0f, 0.7f, 9.6f, 0.0f, -3.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 1.58f, 0.0f, 0.7f, 8.25, 0.0f, -3.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 1.58f, 0.0f, 0.7f, 6.9f, 0.0f, -3.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 1.58f, 0.0f, 0.7f, 5.5f, 0.0f, -3.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 1.58f, 0.0f, 0.7f, 4.2f, 0.0f, -3.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 3.15f, 0.0f, 0.7f, 2.9f, 0.0f, -3.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, -1.8f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, -0.51f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 0.78f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 2.07f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 3.36f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 4.65f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 5.94f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 7.23f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 8.52f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 9.81f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 11.1f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 12.39f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\fence.fbx", L"A Fence", 10, L"Content\\Textures\\fence.jpg");
		model->SetPosition(-1.57f, 0.0f, 0.0f, 0.7f, 15.0f, 0.0f, 13.68);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, -13.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, -16.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, -19.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, -4.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, -7.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, -10.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, -1.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, 2.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, 5.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, 8.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, 11.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, 14.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, 17.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -17.0f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -14.1f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -11.2f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -8.3f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -5.4f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -2.5f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 0.4f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 3.3f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 6.2f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 9.1f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 12.0f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 14.9f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 17.8f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, 20.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, 17.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, 14.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, 11.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, -13.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, -16.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, -19.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, -4.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, -7.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, -10.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, -1.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, 2.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, 5.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, 8.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -20.0f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -17.0f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -14.1f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -11.2f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -8.3f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -5.4f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, -2.5f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 0.4f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 3.3f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 6.2f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 9.1f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 12.0f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 14.9f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 17.8f, -4.0f, -22.0f);
		mComponents.push_back(model);

		model = new ModelFromFile(*this, *mCamera, "Content\\Models\\Tree_temp_climate_004.fbx", L"A tree", 10, L"Content\\Textures\\T_Trees_temp_climate.png");
		model->SetPosition(-1.57f, 5.4f, 0.0f, 0.02f, 20.7f, -4.0f, -22.0f);
		mComponents.push_back(model);


		//house object with diffuse lighting effect:
		mObjectDiffuseLight = new ObjectDiffuseLight(*this, *mCamera);
		mObjectDiffuseLight->SetPosition(-1.57f, 0.0f, 0.0f, 0.03f, 9.0f, 5.5f, -14.0f);
		mComponents.push_back(mObjectDiffuseLight);
		RasterizerStates::Initialize(mDirect3DDevice);
		SamplerStates::Initialize(mDirect3DDevice);

		mFpsComponent = new FpsComponent(*this);
		mFpsComponent->Initialize();
		mRenderStateHelper = new RenderStateHelper(*this);

		Game::Initialize();

		mSpriteBatch = new SpriteBatch(mDirect3DDeviceContext);
		mSpriteFont = new SpriteFont(mDirect3DDevice, L"Content\\Fonts\\Arial_14_Regular.spritefont");

		mCamera->SetPosition(0.0f, 1.0f, 10.0f);
	}

	void RenderingGame::Shutdown()
	{

		DeleteObject(mDemo);
		DeleteObject(mCamera);

		DeleteObject(mKeyboard);
		DeleteObject(mMouse);
		ReleaseObject(mDirectInput);

		DeleteObject(mModel1);
		DeleteObject(mModel2);
		DeleteObject(mModel3);
		DeleteObject(mModel4);
		DeleteObject(mModel5);
		DeleteObject(mModel7);
		DeleteObject(mModel8);
		ReleaseObject(mMainMenuTexture);

		DeleteObject(mFpsComponent);
		DeleteObject(mRenderStateHelper);

		DeleteObject(mObjectDiffuseLight);

		DeleteObject(mSpriteFont);
		DeleteObject(mSpriteBatch);

		Game::Shutdown();
	}

	void RenderingGame::Update(const GameTime& gameTime)
	{
		mKeyboard->Update(gameTime);
		mFpsComponent->Update(gameTime);

		bool escPressed = mKeyboard->WasKeyPressedThisFrame(DIK_ESCAPE);
		bool pPressed = mKeyboard->WasKeyPressedThisFrame(DIK_P);
		bool enterPressed = mKeyboard->WasKeyPressedThisFrame(DIK_RETURN);
		bool spacePressed = mKeyboard->WasKeyPressedThisFrame(DIK_SPACE);

		if (mCurrentState == GameState::Playing)
		{
			if (pPressed)
			{
				mCurrentState = GameState::Paused;
				return;
			}
			else if (escPressed)
			{
				Exit();
				return;
			}

			Game::Update(gameTime);

			float dt = static_cast<float>(gameTime.ElapsedGameTime());

			if (mchickenAutoMove && mModel1 != nullptr)
			{
				mChangeDirectionTimer += dt;

				if (mChangeDirectionTimer >= mChangeDirectionInterval)
				{
					const float twoPi = XM_2PI;
					float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
					float angle = t * twoPi;

					mChickenDirection.x = std::cos(angle);
					mChickenDirection.y = 0.0f;
					mChickenDirection.z = std::sin(angle);

					mChangeDirectionTimer = 0.0f;

					float jitter = (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 0.8f;
					mChangeDirectionInterval = (std::max)(0.5f, 2.0f + jitter);
				}

				mChickenPosition.x += mChickenDirection.x * mChickenSpeed * dt;
				mChickenPosition.z += mChickenDirection.z * mChickenSpeed * dt;

				if (mChickenPosition.z < mChickenAreaMinZ)
				{
					mChickenPosition.z = mChickenAreaMinZ;
					mChickenDirection.z = -mChickenDirection.z;
				}
				else if (mChickenPosition.z > mChickenAreaMaxZ)
				{
					mChickenPosition.z = mChickenAreaMaxZ;
					mChickenDirection.z = -mChickenDirection.z;
				}

				if (mChickenPosition.x < mChickenAreaMinX)
				{
					mChickenPosition.x = mChickenAreaMinX;
					mChickenDirection.x = -mChickenDirection.x;
				}
				else if (mChickenPosition.x > mChickenAreaMaxX)
				{
					mChickenPosition.x = mChickenAreaMaxX;
					mChickenDirection.x = -mChickenDirection.x;
				}

				XMMATRIX W = XMLoadFloat4x4(mModel1->WorldMatrix());
				W.r[3] = XMVectorSet(mChickenPosition.x, mChickenPosition.y, mChickenPosition.z, 1.0f);
				XMStoreFloat4x4(mModel1->WorldMatrix(), W);
			}

			if (Game::toPick)
			{
				if (mModel1->Visible())
					Pick(Game::screenX, Game::screenY, mModel1);

				Game::toPick = false;
			}
		}
		else if (mCurrentState == GameState::Paused)
		{
			if (pPressed)
			{
				mCurrentState = GameState::Playing;
				return;
			}
			else if (escPressed)
			{
				mCurrentState = GameState::Menu;
				return;
			}
		}
		else if (mCurrentState == GameState::Menu)
		{
			if (enterPressed || spacePressed)
			{
				mCurrentState = GameState::Playing;
				return;
			}
			else if (escPressed)
			{
				Exit();
				return;
			}
		}
	}






	// do the picking here

	void RenderingGame::Pick(int sx, int sy, ModelFromFile* model)
	{
		XMFLOAT4X4 P;
		XMStoreFloat4x4(&P, mCamera->ProjectionMatrix());

		float vx = (+2.0f * sx / Game::DefaultScreenWidth - 1.0f) / P._11;
		float vy = (-2.0f * sy / Game::DefaultScreenHeight + 1.0f) / P._22;

		XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR rayDir = XMVectorSet(vx, vy, -1.0f, 0.0f);

		XMMATRIX V = mCamera->ViewMatrix();
		XMVECTOR viewDeterminant = XMMatrixDeterminant(V);
		XMMATRIX invView = XMMatrixInverse(&viewDeterminant, V);

		XMMATRIX W = XMLoadFloat4x4(model->WorldMatrix());
		XMVECTOR worldDeterminant = XMMatrixDeterminant(W);
		XMMATRIX invWorld = XMMatrixInverse(&worldDeterminant, W);

		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
		rayDir = XMVector3TransformNormal(rayDir, toLocal);
		rayDir = XMVector3Normalize(rayDir);

		float tmin = 0.0f;

		if (model->mBoundingBox.Intersects(rayOrigin, rayDir, tmin))
		{
			std::wostringstream pickupString;
			pickupString << L"Do you want to pick up: "
				<< model->GetModelDes()
				<< L"\n\t+" << model->ModelValue() << L" points";

			int result = MessageBox(0, pickupString.str().c_str(),
				L"Object Found", MB_ICONASTERISK | MB_YESNO);

			if (result == IDYES)
			{
				model->SetVisible(false);
				mScore += model->ModelValue();
			}
		}
	}

	void RenderingGame::Draw(const GameTime& gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		if (mCurrentState == GameState::Menu)
		{
			mRenderStateHelper->SaveAll();
			mSpriteBatch->Begin();

			if (mMainMenuTexture != nullptr)
			{
				RECT destRect = { 0, 0, static_cast<LONG>(mScreenWidth), static_cast<LONG>(mScreenHeight) };
				mSpriteBatch->Draw(mMainMenuTexture, destRect);
			}

			float w = static_cast<float>(mScreenWidth);
			float h = static_cast<float>(mScreenHeight);

			mSpriteFont->DrawString(mSpriteBatch, L"CHICKEN ADVENTURE",
				XMFLOAT2(w / 2 - 150.0f, h / 2 - 80.0f), Colors::White);

			mSpriteFont->DrawString(mSpriteBatch, L"Press ENTER to Start",
				XMFLOAT2(w / 2 - 120.0f, h / 2), Colors::Yellow);

			mSpriteFont->DrawString(mSpriteBatch, L"Press ESC to Quit",
				XMFLOAT2(w / 2 - 120.0f, h / 2 + 60.0f), Colors::Red);

			mSpriteBatch->End();
			mRenderStateHelper->RestoreAll();
		}
		else
		{
			Game::Draw(gameTime);

			mRenderStateHelper->SaveAll();
			mFpsComponent->Draw(gameTime);

			mSpriteBatch->Begin();

			std::wostringstream scoreLabel;
			scoreLabel << L"Your current score: " << mScore;

			mSpriteFont->DrawString(mSpriteBatch, scoreLabel.str().c_str(),
				XMFLOAT2(0.0f, 120.0f), Colors::Red);

			if (mCurrentState == GameState::Paused)
			{
				float cx = mScreenWidth / 2.0f;
				float cy = mScreenHeight / 2.0f;

				mSpriteFont->DrawString(mSpriteBatch, L"PAUSED",
					XMFLOAT2(cx - 40.0f, cy - 60.0f), Colors::White);

				mSpriteFont->DrawString(mSpriteBatch, L"Press 'P' to Resume",
					XMFLOAT2(cx - 100.0f, cy), Colors::Yellow);

				mSpriteFont->DrawString(mSpriteBatch, L"Press 'ESC' for Main Menu",
					XMFLOAT2(cx - 130.0f, cy + 60.0f), Colors::Red);
			}

			mSpriteBatch->End();
			mRenderStateHelper->RestoreAll();
		}

		HRESULT hr = mSwapChain->Present(0, 0);
		if (FAILED(hr))
		{
			throw GameException("IDXGISwapChain::Present() failed.", hr);
		}
	}
}