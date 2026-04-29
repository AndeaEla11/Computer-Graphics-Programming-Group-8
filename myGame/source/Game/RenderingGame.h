#pragma once


#include "Game.h"

using namespace Library;

namespace Library
{
    class FirstPersonCamera;
	class RenderStateHelper;

	class Keyboard;
	class Mouse;
	class FpsComponent;

}

namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}

namespace Rendering
{
    class TriangleDemo;
	class ModelFromFile;
	class ObjectDiffuseLight;

    class RenderingGame : public Game
    {
    public:
        RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand);
        ~RenderingGame();

        virtual void Initialize() override;		
        virtual void Update(const GameTime& gameTime) override;
        virtual void Draw(const GameTime& gameTime) override;
		virtual void ResetGame();

	protected:
        virtual void Shutdown() override;

    private:
		enum class GameState
		{
			Menu,
			Playing,
			Paused,
			Quit
		};
		GameState mCurrentState = GameState::Menu;
		static const XMFLOAT4 BackgroundColor;
        FirstPersonCamera * mCamera;
        TriangleDemo* mDemo;

		//Define member variables for Keyboard and mouse
		LPDIRECTINPUT8 mDirectInput;

		Keyboard* mKeyboard;
		Mouse* mMouse;
		ModelFromFile* mModel2;
		ModelFromFile* mModel3;
		ModelFromFile* mModel4;
		ModelFromFile* mModel5;
		ModelFromFile* mModel7;
		ModelFromFile* mModel8;
		ID3D11ShaderResourceView* mMainMenuTexture;

		std::vector<ModelFromFile*> mModels;

		int mScore;
		SpriteBatch* mSpriteBatch;
		SpriteFont* mSpriteFont;

		FpsComponent* mFpsComponent;
		RenderStateHelper* mRenderStateHelper;

		ObjectDiffuseLight* mObjectDiffuseLight;

		void Pick(int sx, int sy, ModelFromFile*);

		float mChickenSpeed;
		bool mchickenAutoMove;
		float mChangeDirectionTimer;
		float mChangeDirectionInterval;

		float mChickenAreaMinX;
		float mChickenAreaMaxX;
		float mChickenAreaMinZ;
		float mChickenAreaMaxZ;

		std::vector<ModelFromFile*> mChickens;
		std::vector<XMFLOAT3> mChickenPositions;
		std::vector<XMFLOAT3> mChickenDirections;

		int mWinScore = 100;
		bool mGameWon = false;
    };
}