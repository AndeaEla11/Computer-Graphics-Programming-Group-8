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

namespace Rendering
{;

	const XMFLOAT4 RenderingGame::BackgroundColor = { 0.1f, 0.1f, 1.0f, 1.0f };

    RenderingGame::RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand)
        :  Game(instance, windowClass, windowTitle, showCommand),
        mDemo(nullptr),
		mDirectInput(nullptr),
		mKeyboard(nullptr),
		mMouse(nullptr),
		mModel(nullptr),
		mFpsComponent(nullptr),
		mRenderStateHelper(nullptr),
		mObjectDiffuseLight(nullptr)

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
    
        mDemo = new TriangleDemo(*this, *mCamera);
        mComponents.push_back(mDemo);

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

        mModel = new ModelFromFile(*this, *mCamera, "Content\\Models\\chicken_root.fbx");
        mModel->SetPosition(0.0f, -3.14f, 0.0f, 0.001f, 0.0f, 0.35f, 0.0f);
        mComponents.push_back(mModel);

        mFpsComponent = new FpsComponent(*this);
        mFpsComponent->Initialize();
        mRenderStateHelper = new RenderStateHelper(*this);

        mObjectDiffuseLight = new ObjectDiffuseLight(*this, *mCamera);
        mObjectDiffuseLight->SetPosition(-1.57f, 0.0f, 0.0f, 0.01, -1.0f, 1.7f, -2.5f);
        mComponents.push_back(mObjectDiffuseLight);
        RasterizerStates::Initialize(mDirect3DDevice);
        SamplerStates::Initialize(mDirect3DDevice);

        Game::Initialize();

		mCamera->SetPosition(0.0f, 0.0f, 5.0f);
    }

    void RenderingGame::Shutdown()
    {
		DeleteObject(mDemo);
        DeleteObject(mCamera);
        Game::Shutdown();
		DeleteObject(mMouse);
        DeleteObject(mKeyboard);
		ReleaseObject(mDirectInput);
		DeleteObject(mModel);
		DeleteObject(mObjectDiffuseLight);
    }

    void RenderingGame::Update(const GameTime &gameTime)
    {
        mFpsComponent->Update(gameTime);
        Game::Update(gameTime);

        if (mKeyboard->WasKeyPressedThisFrame(DIK_ESCAPE))
        {
            Exit();
        }
    }

    void RenderingGame::Draw(const GameTime &gameTime)
    {
        mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&BackgroundColor));
        mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        
        mRenderStateHelper->SaveAll();
        mFpsComponent->Draw(gameTime);
        mRenderStateHelper->RestoreAll();

        Game::Draw(gameTime);
       
        HRESULT hr = mSwapChain->Present(0, 0);
        if (FAILED(hr))
        {
            throw GameException("IDXGISwapChain::Present() failed.", hr);
        }
    }
}