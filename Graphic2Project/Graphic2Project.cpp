// Graphic2Project.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Graphic2Project.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPHIC2PROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHIC2PROJECT));

    MSG msg;
	mTimer.Restart();
    // Main message loop:
    while (true)//GetMessage(&msg, nullptr, 0, 0))
    {
		
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		if (msg.message == WM_QUIT)
			break;
		
		mTimer.Signal();
		
		Update();
		Render();

		
    }
	CleanupDevice();

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHIC2PROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GRAPHIC2PROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

	mWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!mWindow)
   {
      return FALSE;
   }

   ShowWindow(mWindow, nCmdShow);
   UpdateWindow(mWindow);
   

   GetClientRect(mWindow, &mWinR);

   //my codes here   
   D3D_DRIVER_TYPE driverTypes[] =
   {
	   D3D_DRIVER_TYPE_HARDWARE,
	   D3D_DRIVER_TYPE_WARP,
	   D3D_DRIVER_TYPE_REFERENCE,
   };
   UINT numDriverTypes = ARRAYSIZE(driverTypes);

   D3D_FEATURE_LEVEL featureLevels[] =
   {
	   D3D_FEATURE_LEVEL_11_1,
	   D3D_FEATURE_LEVEL_11_0,
	   D3D_FEATURE_LEVEL_10_1,
	   D3D_FEATURE_LEVEL_10_0,
   };
   UINT numFeatureLevels = ARRAYSIZE(featureLevels);

   DXGI_SWAP_CHAIN_DESC swap;
   ZeroMemory(&swap, sizeof(DXGI_SWAP_CHAIN_DESC));
   swap.BufferCount = 1;
   swap.OutputWindow = mWindow;
   swap.Windowed = true;
   swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
   swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
   swap.BufferDesc.Width = mWinR.right - mWinR.left;
   swap.BufferDesc.Height = mWinR.bottom - mWinR.top;
   swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
   swap.SampleDesc.Count = 1;
   aspectRatio = swap.BufferDesc.Width / static_cast<float>(swap.BufferDesc.Height);
 
   HRESULT hr;
   // ----Debug Mode change later

   UINT createDeviceFlags = 0;
#ifdef _DEBUG
   createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

   hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, 
	   featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swap, &mSwap, &mDev, &dx11, &mContext);

   ///deferred Context
   mDev->CreateDeferredContext(0, &mDeferredContext);
   ID3D11Resource* backBuffer;
   hr = mSwap->GetBuffer(0, __uuidof(backBuffer), (void**)&backBuffer);
   hr = mDev->CreateRenderTargetView(backBuffer, NULL, &mRTV);

   backBuffer->Release();


   mPort.Height = swap.BufferDesc.Height;
   mPort.Width = swap.BufferDesc.Width;
   mPort.TopLeftX = mPort.TopLeftY = 0;
   mPort.MinDepth = 0;
   mPort.MaxDepth = 1;

	mFirPort.Height = swap.BufferDesc.Height;
	mFirPort.Width = swap.BufferDesc.Width/2; 
	mFirPort.TopLeftX = mFirPort.TopLeftY = 0;
	mFirPort.MinDepth = 0; 
	mFirPort.MaxDepth = 1; 

	mSecPort.Height = swap.BufferDesc.Height;
	mSecPort.Width = swap.BufferDesc.Width/2;
	mSecPort.TopLeftX = swap.BufferDesc.Width / 2;
	mSecPort.TopLeftY = 0;
	mSecPort.MinDepth = 0;
	mSecPort.MaxDepth = 1;

   static const SkyBox skyBoxVerts[] =
   {
	   { XMFLOAT3(-1.0f, -1.0f, -1.0f) },
	   { XMFLOAT3(-1.0f, -1.0f,  1.0f) },
	   { XMFLOAT3(-1.0f,  1.0f, -1.0f) },
	   { XMFLOAT3(-1.0f,  1.0f,  1.0f) },
	   { XMFLOAT3(1.0f, -1.0f, -1.0f) },
	   { XMFLOAT3(1.0f, -1.0f,  1.0f) },
	   { XMFLOAT3(1.0f,  1.0f, -1.0f) },
	   { XMFLOAT3(1.0f,  1.0f,  1.0f) },
   };
   static const unsigned int skyIndices[] =
   {
	   3,7,5,
	   5,1,3,

	   7,6,4,
	   4,5,7,

	   6,2,0,
	   0,4,6,

	   2,3,1,
	   1,0,2,

	   2,6,7,
	   7,3,2,

	   5,4,0,
	   0,1,5,
   };
   static const D3D11_INPUT_ELEMENT_DESC skyBoxInputDesc[] =
   {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	
   };
   hr = mDev->CreateInputLayout(skyBoxInputDesc, 3, SkyVertexShader, sizeof(SkyVertexShader), &skyLayout);
   D3D11_BUFFER_DESC bDesc;
   D3D11_SUBRESOURCE_DATA subData;
   ZeroMemory(&bDesc, sizeof(bDesc));
   ZeroMemory(&subData, sizeof(subData));

   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(SkyBox) * ARRAYSIZE(skyBoxVerts);
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;
   subData.pSysMem = skyBoxVerts;
   mDev->CreateBuffer(&bDesc, &subData, &vSkyBuff);

   bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bDesc.ByteWidth = sizeof(unsigned int)*ARRAYSIZE(skyIndices);
   subData.pSysMem = skyIndices;
   subData.SysMemPitch = 0;
   subData.SysMemSlicePitch = 0;
   mDev->CreateBuffer(&bDesc, &subData, &iSkyBuff);

   D3D11_SAMPLER_DESC sampleDesc;
   ZeroMemory(&sampleDesc, sizeof(sampleDesc));
   sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
   sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
   sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
   sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

   mDev->CreateSamplerState(&sampleDesc, &skyBoxSamplerState);
   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/SkyboxOcean.dds", NULL, &skyBoxTextureRV);

   // compile the shaders
   hr = mDev->CreateVertexShader(SkyVertexShader, sizeof(SkyVertexShader), nullptr, &vSkyShader);
   hr = mDev->CreatePixelShader(SkyPixelShader, sizeof(SkyPixelShader), nullptr, &pSkyShader);

   SimpleVertex tri[] = // NDC
   {
	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {0.25f, -0.25f, -0.25f, 1},  {1, 0, 1, 1} },
	   { {-0.25f, -0.25f, -0.25f, 1},  {1, 1, 1, 1} },

	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {0.25f, -0.25f, 0.25f, 1},  {1, 0, 1, 1} },
	   { {0.25f, -0.25f, -0.25f, 1},  {1, 1, 1, 1} },

	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {-0.25f, -0.25f, 0.25f, 1},  {1, 0, 1, 1} },
	   { {0.25f, -0.25f, 0.25f, 1},  {1, 1, 1, 1} },

	   { {0, 1.0f, 0 , 1},  {1, 1, 0 , 1} },
	   { {-0.25f, -0.25f, -0.25f, 1},  {1, 0, 1, 1} },
	   { {-0.25f, -0.25f, 0.25f, 1},  {1, 1, 1, 1} },
   };

   numVerts = ARRAYSIZE(tri);

   //LOAD THE TRI
   ZeroMemory(&bDesc, sizeof(bDesc));
   ZeroMemory(&subData, sizeof(subData));

   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(SimpleVertex)*numVerts;
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;
   
   subData.pSysMem = tri;

   mDev->CreateBuffer(&bDesc, &subData, &vBuff);
  
   // compile the shaders
   hr=mDev->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vShader);
   hr=mDev->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pShader);
   
   //Describe it to D3D11
   D3D11_INPUT_ELEMENT_DESC ieDesc[] =
   {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   };
   hr= mDev->CreateInputLayout(ieDesc, 2, VertexShader, sizeof(VertexShader), &vLayout);

   CreateSphere(15, 15);
   hr = mDev->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &spVShader);
   hr = mDev->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &spPShader);
   hr = mDev->CreateInputLayout(ieDesc, 2, VertexShader, sizeof(VertexShader), &spLayout);



   //Constant Buffer Section
   ZeroMemory(&bDesc, sizeof(bDesc));

   bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
   bDesc.ByteWidth = sizeof(WVP);
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_DYNAMIC;

   hr= mDev->CreateBuffer(&bDesc, nullptr, &cBuff);

   //Lighting Buffer
   bDesc.ByteWidth = sizeof(LightingConstant);
   hr = mDev->CreateBuffer(&bDesc, nullptr, &cLightBuff);

   bDesc.ByteWidth = sizeof(double)*2;
   hr = mDev->CreateBuffer(&bDesc, nullptr, &timerBuff);

   bDesc.ByteWidth = sizeof(CamConstant);
   hr = mDev->CreateBuffer(&bDesc, nullptr, &camBuff);




   D3D11_BLEND_DESC blendDESC;
   ZeroMemory(&blendDESC, sizeof(blendDESC));
   blendDESC.AlphaToCoverageEnable = true;
   blendDESC.RenderTarget[0].BlendEnable = true;
   blendDESC.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
   blendDESC.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
   blendDESC.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
   blendDESC.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
   blendDESC.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
   blendDESC.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
   blendDESC.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

   mDev->CreateBlendState(&blendDESC, &mBlendState);


   ////Loading Model and Texture
   ////Multithreading
   thread Loading = thread(LoadGameObject);
   Loading.join();

   ZeroMemory(&bDesc, sizeof(bDesc));
   ZeroMemory(&subData, sizeof(subData));

   loadObject("Assets/Rock.obj", rockVertex);

   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;
   bDesc.ByteWidth = sizeof(SimpleMesh)*rockVertex.size();
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   subData.pSysMem = rockVertex.data();
   hr=mDev->CreateBuffer(&bDesc, &subData, &vRockBuff);



   //Load New Mesh
   hr = mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &vRockShader);
   hr = mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &pRockShader);

   D3D11_INPUT_ELEMENT_DESC meshInputDesc[] =
   {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   };
   // Load the Texture
   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/Rock_Diffuse.dds", NULL, &rockTextureRV);
   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/Rock_SPecular.dds", NULL, &rockTextureRV1);
   // Create the sample state
   ZeroMemory(&sampleDesc, sizeof(sampleDesc));
   sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
   sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
   sampleDesc.MinLOD = 0;
   sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
   hr = mDev->CreateSamplerState(&sampleDesc, &rockSamplerState);
   hr = mDev->CreateInputLayout(meshInputDesc, 4, VertexMeshShader, sizeof(VertexMeshShader), &vRockLayout);




   //Load Flag
   loadObject("Assets/Flag.obj", flagVertex);

   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;
   bDesc.ByteWidth = sizeof(SimpleMesh)*flagVertex.size();
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   subData.pSysMem = flagVertex.data();
   hr=mDev->CreateBuffer(&bDesc, &subData, &vFlagBuff);



   //Load New Mesh
   hr = mDev->CreateVertexShader(VertexWaveShader, sizeof(VertexWaveShader), nullptr, &vFlagShader);
   hr = mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &pFlagShader);

   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/Flag.dds", (ID3D11Resource**)&flagTexture, &flagTextureRV);

   // Create the sample state
   ZeroMemory(&sampleDesc, sizeof(sampleDesc));
   sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
   sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
   sampleDesc.MinLOD = 0;
   sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
   hr = mDev->CreateSamplerState(&sampleDesc, &flagSamplerState);
   hr = mDev->CreateInputLayout(meshInputDesc, 4, VertexMeshShader, sizeof(VertexMeshShader), &vFlagLayout);

   //Load StoneHenge Mesh

   ZeroMemory(&bDesc, sizeof(bDesc));
   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(StoneHenge_data);
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;

   subData.pSysMem = StoneHenge_data;

   hr = mDev->CreateBuffer(&bDesc, &subData, &vStoneBuff);

   //Index Buffer mesh
   bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bDesc.ByteWidth = sizeof(StoneHenge_indicies);
   subData.pSysMem = StoneHenge_indicies;
   hr = mDev->CreateBuffer(&bDesc, &subData, &iStoneBuff);

   //Load New Mesh
   hr = mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &vStoneShader);
   hr = mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &pStoneShader);


   // Load the Texture
   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/StoneHenge.dds", (ID3D11Resource**)&stoneTexture, &stoneTextureRV);

   // Create the sample state
   ZeroMemory(&sampleDesc, sizeof(sampleDesc));
   sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
   sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
   sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
   sampleDesc.MinLOD = 0;
   sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
   hr = mDev->CreateSamplerState(&sampleDesc, &stoneSamplerState);
   hr = mDev->CreateInputLayout(meshInputDesc, 4, VertexMeshShader, sizeof(VertexMeshShader), &vStoneLayout);



   CD3D11_TEXTURE2D_DESC zDesc;
   ZeroMemory(&zDesc, sizeof(zDesc));
   zDesc.ArraySize = 1;
   zDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
   zDesc.Width = swap.BufferDesc.Width;
   zDesc.Height = swap.BufferDesc.Height;
   zDesc.Format = DXGI_FORMAT_D32_FLOAT;
   zDesc.Usage = D3D11_USAGE_DEFAULT;
   zDesc.MipLevels = 1;
   zDesc.SampleDesc.Count = 1;

   hr = mDev->CreateTexture2D(&zDesc, nullptr, &zBuffer);
   hr = mDev->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);

   D3D11_RASTERIZER_DESC cmdesc;

   ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
   cmdesc.FillMode = D3D11_FILL_SOLID;
   cmdesc.FrontCounterClockwise = false;
   ///////////////**************new**************////////////////////
   cmdesc.CullMode = D3D11_CULL_NONE;
   hr = mDev->CreateRasterizerState(&cmdesc, &RSCullNone);

   loadObject("Assets/palmTree1.obj",treeVertex);
   CreateDDSTextureFromFile(mDev, L"Assets/Textures/palm1_Diffuse.dds", NULL, &treeTextureRV);
   ////////////////////////////////////////////Instancing Trees////////////////////////////////////////////
   vector<InstanceData> inst(numTrees);
   XMVECTOR tempPos;
   srand(time(NULL));
   // We are just creating random positions for the trees, between the positions of (-100, 0, -100) to (100, 0, 100)
   // then storing the position in our instanceData array
   for (int i = 0; i < numTrees; i++)
   {
	   float randX = ((float)(rand() % 2000) / 10) - 100;
	   float randZ = ((float)(rand() % 2000) / 10) - 100;
	   tempPos = XMVectorSet(randX, 0.0f, randZ, 0.0f);

	   XMStoreFloat3(&inst[i].pos, tempPos);
   }
   D3D11_BUFFER_DESC instBuffDesc;
   ZeroMemory(&instBuffDesc, sizeof(instBuffDesc));

   instBuffDesc.Usage = D3D11_USAGE_DEFAULT;
   instBuffDesc.ByteWidth = sizeof(InstanceData) * numTrees;
   instBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   instBuffDesc.CPUAccessFlags = 0;
   instBuffDesc.MiscFlags = 0;

   D3D11_SUBRESOURCE_DATA instData;
   ZeroMemory(&instData, sizeof(instData));

   instData.pSysMem = &inst[0];
   hr = mDev->CreateBuffer(&instBuffDesc, &instData, &treeInstanceBuff);

   treeWorld = XMMatrixIdentity();

   D3D11_INPUT_ELEMENT_DESC layout[] =
   {
	   { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	   { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	   { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	   { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	   /************************************New Stuff****************************************************/
	   // Instance elements
	   // last parameter (InstanceDataStepRate) is one because we will "step" to the next instance element (INSTANCEPOS) after drawing 1 instance (tree)
	   { "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT,    1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	   /*************************************************************************************************/
   };
   mDev->CreateInputLayout(layout,5,Instance_VS,sizeof(Instance_VS), &treeInsLayout);
   hr = mDev->CreateVertexShader(Instance_VS, sizeof(Instance_VS), nullptr, &treeVShader);
   hr = mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &treePShader);




   //Directional Light
   myLighting.dLightClr = { 0.95f, 0.95f, 0.95f, 1.0f };
   XMStoreFloat4(&myLighting.dLightDir, dLight);
   //Point Light
   myLighting.pLightClr = { 0.8f,0.0f,0.3f,1.0f };
   XMStoreFloat4(&myLighting.pLightPos, pLightPos);
   XMStoreFloat(&myLighting.pLightRadius, pLightRadius);
   //Spot Light
   myLighting.sLightClr = { 0.8f,0.8f,0.8f,1.0f };
   XMStoreFloat4(&myLighting.sLightPos, sLightPos);
   XMStoreFloat4(&myLighting.sLightDir, sLightDir);
   XMStoreFloat(&myLighting.innerAngle, innerAngle);
   XMStoreFloat(&myLighting.outerAngle, outerAngle);

   myLighting.lightingMode = 1;


   mySecLighting.dLightClr = { 1.0f, 1.0f, 1.0f, 1.0f };
   XMStoreFloat4(&mySecLighting.dLightDir, dLight);
   //Point Light
   mySecLighting.pLightClr = { 1.0f,0.88f,0.6f,1.0f };
   mySecLighting.pLightPos = {-10,-100,100,1};
   mySecLighting.pLightRadius = 1000;
   //Spot Light
   mySecLighting.sLightClr = { 0.8f,0.8f,0.8f,1.0f };
   XMStoreFloat4(&mySecLighting.sLightPos, sLightPos);
   XMStoreFloat4(&mySecLighting.sLightDir, sLightDir);
   XMStoreFloat(&mySecLighting.innerAngle, innerAngle);
   XMStoreFloat(&mySecLighting.outerAngle, outerAngle);
   mySecLighting.lightingMode = 1;


   // Initialize the world matrices
   XMMATRIX temp = XMMatrixIdentity();
   XMStoreFloat4x4(&myMatricies.g_World,temp);
   XMStoreFloat4x4(&mySecWorld.g_World, temp);

   // Initialize the view matrix
	curCamera = &firstCam;
	curCamera->camPosition = XMVectorSet(0.0f, 10.0f, -40.0f, 0.0f);
	curCamera->camTarget = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	curCamera->camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
   temp = XMMatrixLookAtLH(curCamera->camPosition, curCamera->camTarget, curCamera->camUp);
   XMStoreFloat4x4(&myMatricies.g_View, temp);
   XMStoreFloat4x4(&mySecWorld.g_View, temp);

   if(multiviewPort)
   aspectRatio = swap.BufferDesc.Width / 2.0f / swap.BufferDesc.Height;
   temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
   XMStoreFloat4x4(&myMatricies.g_Projection, temp);
   XMStoreFloat4x4(&mySecWorld.g_Projection, temp);


   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_SIZE:
	{
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);
		WindowResize(width,height);
		break;
	}
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void Render()
{
	

	mContext->OMSetRenderTargets(1, &mRTV, zBufferView);
	mContext->ClearRenderTargetView(mRTV, DirectX::Colors::BurlyWood);
	mContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	// rasterizer


	if (multiviewPort)
	{
		mContext->RSSetViewports(1, &mFirPort);
		ThemeOne(myMatricies);
		mContext->RSSetViewports(1, &mSecPort);
		ThemeTwo(mySecWorld);
	}
	else
	{

		mContext->RSSetViewports(1, &mPort);
		if(SceneOne)
		{
		ThemeOne(myMatricies);
		}
		else
		{
		ThemeTwo(mySecWorld);
		}
	}
	mSwap->Present(1, 0);


}
void ThemeOne(WVP &myMatrix)
{

	D3D11_MAPPED_SUBRESOURCE gpuBuffer = {};
	HRESULT  hr;
	XMMATRIX temp = XMMatrixIdentity();
	totalTime[0] = mTimer.TotalTime();
	totalTime[1] += mTimer.Delta();
	D3D11_MAPPED_SUBRESOURCE mapTimeSubresource;
	ZeroMemory(&mapTimeSubresource, sizeof(mapTimeSubresource));
	mContext->Map(timerBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapTimeSubresource);
	memcpy(mapTimeSubresource.pData, totalTime, sizeof(double) * 2);
	mContext->Unmap(timerBuff, 0);

	D3D11_MAPPED_SUBRESOURCE camBuffer;
	XMStoreFloat4(&myfirCamCons.camPos,firstCam.camPosition);
	myfirCamCons.hasMultiTex = false;
	myfirCamCons.hasNormal = false;
	myfirCamCons.hasShadowMap = false;
	hr = mContext->Map(camBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &camBuffer);
	*((CamConstant*)(camBuffer.pData)) = myfirCamCons;
	mContext->Unmap(camBuff, 0);

	UINT skyStrides[] = { sizeof(SkyBox) };
	UINT skyoffsets[] = { 0 };
	mContext->PSSetShaderResources(0, 1, &skyBoxTextureRV);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mContext->IASetVertexBuffers(0, 1, &vSkyBuff, skyStrides, skyoffsets);
	mContext->IASetIndexBuffer(iSkyBuff, DXGI_FORMAT_R32_UINT, 0);
	mContext->VSSetShader(vSkyShader, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(pSkyShader, 0, 0);
	mContext->IASetInputLayout(skyLayout);
	mContext->PSSetSamplers(0, 1, &skyBoxSamplerState);
	
	temp = XMMatrixIdentity();

	//Define sphereWorld's world space matrix
	Scale = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	//Make sure the sphere is always centered around camera

	Translation = XMMatrixTranslation(XMVectorGetX(firstCam.camPosition), XMVectorGetY(firstCam.camPosition), XMVectorGetZ(firstCam.camPosition));
	XMStoreFloat4x4(&myMatrix.g_World, Scale * Translation);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->DrawIndexed(36, 0, 0);
	mContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);



	UINT strides[] = { sizeof(SimpleVertex) };
	UINT offsets[] = { 0 };
	mContext->IASetVertexBuffers(0, 1, &vBuff, strides, offsets);
	mContext->IASetInputLayout(vLayout);
	mContext->VSSetShader(vShader, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(pShader, 0, 0);
	temp = XMMatrixIdentity();
	temp = XMMatrixTranslation(0, 10, 4);
	XMMATRIX temp2 = XMMatrixRotationY(5*totalTime[1]);
	temp = XMMatrixMultiply(temp2, temp);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	trianglePos={ myMatrix.g_World._41, myMatrix.g_World._42, myMatrix.g_World._43, myMatrix.g_World._44 };
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(numVerts, 0);


	///Procedually draw, but no luck....
	//mContext->IASetVertexBuffers(0, 1, &spVertBuffer, strides, offsets);
	//mContext->IASetIndexBuffer(spIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//mContext->IASetInputLayout(spLayout);
	//mContext->VSSetShader(spVShader, 0, 0);
	//mContext->VSSetConstantBuffers(0, 1, &cBuff);
	//mContext->PSSetShader(spPShader, 0, 0);
	//mContext->RSSetState(RSCullNone);
	//temp = XMMatrixIdentity();
	//temp = XMMatrixScaling(5, 5, 5) * XMMatrixTranslation(0, 5, 0);
	//temp2 = XMMatrixIdentity();
	//temp2 = XMMatrixRotationY(5 * totalTime[1]);
	//temp = XMMatrixMultiply(temp2, temp);
	//XMStoreFloat4x4(&myMatrix.g_World, temp);
	//hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	//*((WVP*)(gpuBuffer.pData)) = myMatrix;
	//mContext->Unmap(cBuff, 0);
	//mContext->DrawIndexed(spVertexNum,0, 0);




	if (myLighting.lightingMode == 1)
	{
		//Directional Light
		dLight = XMVector4Transform(dLight, XMMatrixRotationY(XMConvertToRadians(10 * delta_time)));
		XMStoreFloat4(&myLighting.dLightDir, dLight);
		//Point Light
		//pLightPos = XMVector4Transform(pLightPos, XMMatrixRotationY(XMConvertToRadians(10 * delta_time)));
		//XMStoreFloat4(&myLighting.pLightPos, pLightPos);
		//Spot Light 
		//XMFLOAT4 slightTemp;

		//XMStoreFloat4(&slightTemp, sLightPos);
		//if (flag)
		//{
		//	sLightPos = XMVector4Transform(sLightPos, XMMatrixTranslation(0, 0, 10 * delta_time));
		//	if (slightTemp.z > 10)
		//		flag = false;
		//}
		//else
		//{
		//	sLightPos = XMVector4Transform(sLightPos, XMMatrixTranslation(0, 0, -10 * delta_time));
		//	if (slightTemp.z < -10)
		//		flag = true;
		//}
		//XMStoreFloat4(&myLighting.sLightPos, sLightPos);
		//sLightDir = XMVector4Transform(sLightDir, XMMatrixRotationY(XMConvertToRadians(10 * -delta_time)));
		//XMStoreFloat4(&myLighting.sLightDir, sLightDir);
	}

	D3D11_MAPPED_SUBRESOURCE LightingBuffer;
	hr = mContext->Map(cLightBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &LightingBuffer);
	*((LightingConstant*)(LightingBuffer.pData)) = myLighting;
	mContext->Unmap(cLightBuff, 0);

	
	//Draw Rock
	//Set Pipline

	UINT mesh_strides[] = { sizeof(SimpleMesh) };
	UINT mesh_offsets[] = { 0 };


	//Draw Ocean
	mContext->IASetVertexBuffers(0, 1, &ground.pGO_Vbuff, &ground.stride, mesh_offsets);
	mContext->VSSetShader(ground.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(ground.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(ground.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &ground.pGO_SRV_Texture);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp = XMMatrixScaling(100,1.5f,100)*XMMatrixTranslation(0, -10, 0);
	XMStoreFloat4(&ground.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(ground.GO_vertex.size(), 0);

	//Draw isLand
	//Set Pipline

	mContext->IASetVertexBuffers(0, 1, &island.pGO_Vbuff, &island.stride, mesh_offsets);
	mContext->VSSetShader(island.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(island.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(island.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &island.pGO_SRV_Texture);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp = XMMatrixScaling(2.0f,1,2.0f)*XMMatrixTranslation(0, -5, 10);
	XMStoreFloat4(&island.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(island.GO_vertex.size(), 0);

	//Draw player
	mContext->IASetVertexBuffers(0, 1, &player.pGO_Vbuff, &player.stride, mesh_offsets);
	mContext->VSSetShader(player.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(player.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(player.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &player.pGO_SRV_Texture);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp = XMMatrixRotationY(XMConvertToRadians(180))*XMMatrixTranslation(3, 2.5f, -5);
	XMStoreFloat4(&player.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(player.GO_vertex.size(), 0);

	//Draw fussy
	mContext->IASetVertexBuffers(0, 1, &fussy.pGO_Vbuff, &fussy.stride, mesh_offsets);
	mContext->VSSetShader(fussy.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(fussy.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(fussy.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &fussy.pGO_SRV_Texture);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp = XMMatrixScaling(3.5f,3.5f,3.5f)*XMMatrixTranslation(3, 3.5f, 10);
	XMStoreFloat4(&fussy.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(fussy.GO_vertex.size(), 0);

	//Draw PalmTree
	mContext->IASetVertexBuffers(0, 1, &palmTree.pGO_Vbuff, &palmTree.stride, mesh_offsets);
	mContext->VSSetShader(palmTree.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(palmTree.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(palmTree.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &palmTree.pGO_SRV_Texture);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp =XMMatrixRotationZ(XMConvertToRadians(-10))*XMMatrixTranslation(20, 0.0f, 10);
	XMStoreFloat4(&palmTree.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(palmTree.GO_vertex.size(), 0);

	mContext->IASetVertexBuffers(0, 1, &palmTree1.pGO_Vbuff, &palmTree1.stride, mesh_offsets);
	mContext->VSSetShader(palmTree1.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(palmTree1.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(palmTree1.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &palmTree1.pGO_SRV_Texture);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp = XMMatrixRotationZ(XMConvertToRadians(-10)) * XMMatrixTranslation(-10, 0.0f, 10);
	XMStoreFloat4(&palmTree1.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(palmTree1.GO_vertex.size(), 0);

	//Draw treasureChest
	mContext->IASetVertexBuffers(0, 1, &treasureChest.pGO_Vbuff, &treasureChest.stride, mesh_offsets);
	mContext->VSSetShader(treasureChest.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(treasureChest.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(treasureChest.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &treasureChest.pGO_SRV_Texture);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp = XMMatrixScaling(0.7f,0.7f,0.7f)*XMMatrixTranslation(-5, 3.5f, 15);
	XMStoreFloat4(&treasureChest.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(treasureChest.GO_vertex.size(), 0);


	// Store the vertex and instance buffers into an array
	////Create Palm Trees
	UINT treeStrides[] = { sizeof(InstanceData) };
	UINT treeOffsets[] = { 0};
	//Set the WVP matrix and send it to the constant buffer in effect file
	//mContext->IASetVertexBuffers(0, 1, &treeVertBuff, mesh_strides,treeOffsets);
	//mContext->IASetVertexBuffers(1, 1, &treeInstanceBuff, treeStrides, treeOffsets);
	//mContext->VSSetShader(treeVShader,0,0);
	//mContext->PSSetShader(treePShader, 0, 0);
	//mContext->IASetInputLayout(treeInsLayout);
	//mContext->VSSetConstantBuffers(0, 1, &cBuff);
	//mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	//mContext->PSSetConstantBuffers(1, 1, &camBuff);
	//mContext->PSSetShaderResources(0, 1, &treeTextureRV);
	//mContext->PSSetSamplers(0, 1, &rockSamplerState);

	//temp = XMMatrixIdentity();
	//temp = XMMatrixTranslation(-5, 3.5f, 15);
	//XMStoreFloat4x4(&myMatrix.g_World, temp);
	//hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	//*((WVP*)(gpuBuffer.pData)) = myMatrix;
	//mContext->Unmap(cBuff, 0);
	//mContext->DrawInstanced(treeVertex.size(), 20, 0, 0);




	////Draw Flag
	//mContext->IASetVertexBuffers(0, 1, &vFlagBuff, mesh_strides, mesh_offsets);
	//mContext->VSSetShader(vFlagShader, 0, 0);
	//mContext->VSSetConstantBuffers(0, 1, &cBuff);
	//mContext->VSSetConstantBuffers(1, 1, &timerBuff);
	//mContext->PSSetShader(pFlagShader, 0, 0);
	//mContext->PSSetShaderResources(0, 1, &flagTextureRV);
	//mContext->IASetInputLayout(vFlagLayout);
	//temp = XMMatrixIdentity();
	//temp = XMMatrixTranslation(10, 5, -2);
	//XMStoreFloat4x4(&myMatrix.g_World, temp);
	//flagPos = { myMatrix.g_World._41, myMatrix.g_World._42, myMatrix.g_World._43, myMatrix.g_World._44 };
	//hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	//*((WVP*)(gpuBuffer.pData)) = myMatrix;
	//mContext->Unmap(cBuff, 0);
	//mContext->Draw(flagVertex.size(), 0);

	//Draw Rock
	mContext->IASetVertexBuffers(0, 1, &vRockBuff, mesh_strides, mesh_offsets);
	mContext->VSSetShader(vRockShader, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(pRockShader, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(vRockLayout);
	mContext->PSSetShaderResources(0, 1, &rockTextureRV);
	mContext->PSSetShaderResources(1, 1, &rockTextureRV1);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp = XMMatrixTranslation(11, 1, 5);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	rockPos = { myMatrix.g_World._41,myMatrix.g_World._42,myMatrix.g_World._43,myMatrix.g_World._44 };
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);

	myfirCamCons.hasMultiTex = true;
	hr = mContext->Map(camBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &camBuffer);
	*((CamConstant*)(camBuffer.pData)) = myfirCamCons;
	mContext->Unmap(camBuff, 0);
	mContext->Draw(rockVertex.size(), 0);


}
void ThemeTwo(WVP& myMatrix)
{
	D3D11_MAPPED_SUBRESOURCE gpuBuffer = {};
	HRESULT  hr;
	XMMATRIX temp = XMMatrixIdentity();
	XMMATRIX temp2 = XMMatrixIdentity();

	totalTime[0] = mTimer.TotalTime();
	totalTime[1] += mTimer.Delta();
	D3D11_MAPPED_SUBRESOURCE mapTimeSubresource;
	ZeroMemory(&mapTimeSubresource, sizeof(mapTimeSubresource));
	mContext->Map(timerBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapTimeSubresource);
	memcpy(mapTimeSubresource.pData, totalTime, sizeof(double) * 2);
	mContext->Unmap(timerBuff, 0);

	D3D11_MAPPED_SUBRESOURCE LightingBuffer;

	mySecLighting.sLightPos = { XMVectorGetX(secCam.camPosition),XMVectorGetY(secCam.camPosition),XMVectorGetZ(secCam.camPosition),1.0f };
	mySecLighting.sLightDir = { XMVectorGetX(secCam.camTarget- secCam.camPosition),XMVectorGetY(secCam.camTarget- secCam.camPosition),XMVectorGetZ(secCam.camTarget- secCam.camPosition) ,1.0f };
	hr = mContext->Map(cLightBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &LightingBuffer);
	*((LightingConstant*)(LightingBuffer.pData)) = mySecLighting;
	mContext->Unmap(cLightBuff, 0);

	D3D11_MAPPED_SUBRESOURCE camBuffer;
	XMStoreFloat4(&mysecCamCons.camPos, secCam.camPosition);
	mysecCamCons.hasMultiTex = false;
	mysecCamCons.hasNormal = false;
	mysecCamCons.hasShadowMap = false;
	hr = mContext->Map(camBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &camBuffer);
	*((CamConstant*)(camBuffer.pData)) = mysecCamCons;
	mContext->Unmap(camBuff, 0);

	UINT skyoffsets[] = { 0 };
	mContext->PSSetShaderResources(0, 1, &spaceSkybox.pGO_SRV_Texture);
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mContext->IASetVertexBuffers(0, 1, &spaceSkybox.pGO_Vbuff, &spaceSkybox.stride, skyoffsets);
	mContext->VSSetShader(spaceSkybox.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(spaceSkybox.pGO_PS, 0, 0);
	mContext->IASetInputLayout(spaceSkybox.pGO_inputLayout);
	mContext->PSSetSamplers(0, 1, &skyBoxSamplerState);

	temp = XMMatrixIdentity();

	//Define sphereWorld's world space matrix
	Scale = XMMatrixScaling(10.0f, 10.0f, 10.0f);
	//Make sure the sphere is always centered around camera
	Translation = XMMatrixTranslation(XMVectorGetX(secCam.camPosition), XMVectorGetY(secCam.camPosition), XMVectorGetZ(secCam.camPosition));
	XMStoreFloat4x4(&myMatrix.g_World, Scale * Translation);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(spaceSkybox.GO_vertex.size(), 0);
	mContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	///Draw Sun
	UINT mesh_offsets[] = { 0 };
	mContext->IASetVertexBuffers(0, 1, &sun.pGO_Vbuff, &sun.stride, mesh_offsets);
	mContext->VSSetShader(sun.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(sun.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(sun.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &sun.pGO_SRV_Texture);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp = XMMatrixRotationY(XMConvertToRadians(-3* totalTime[1])) * XMMatrixTranslation(0, 0, 100);

	XMStoreFloat4(&sun.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(sun.GO_vertex.size(), 0);

	//Draw Earth
	mysecCamCons.hasMultiTex = false;
	hr = mContext->Map(camBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &camBuffer);
	*((CamConstant*)(camBuffer.pData)) = mysecCamCons;
	mContext->Unmap(camBuff, 0);
	mContext->IASetVertexBuffers(0, 1, &earth.pGO_Vbuff, &earth.stride, mesh_offsets);
	mContext->VSSetShader(earth.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(earth.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(earth.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &earth.pGO_SRV_Texture);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp = XMMatrixRotationY(XMConvertToRadians(10 * totalTime[1]))*XMMatrixTranslation(200, 0, 0);
	temp2 = XMMatrixIdentity();
	temp2=XMMatrixRotationY(XMConvertToRadians(3 * totalTime[1]))* XMMatrixTranslation(sun.objPos.x, sun.objPos.y, sun.objPos.z);
	temp = temp * temp2;
	XMStoreFloat4(&earth.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	rockPos = { myMatrix.g_World._41,myMatrix.g_World._42,myMatrix.g_World._43,myMatrix.g_World._44 };
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(earth.GO_vertex.size(), 0);

	//Draw Mars
	mContext->IASetVertexBuffers(0, 1, &mars.pGO_Vbuff, &mars.stride, mesh_offsets);
	mContext->VSSetShader(mars.pGO_VS, 0, 0);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	mContext->PSSetShader(mars.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->PSSetConstantBuffers(1, 1, &camBuff);
	mContext->IASetInputLayout(mars.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &mars.pGO_SRV_Texture);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	temp = XMMatrixIdentity();
	temp = XMMatrixRotationY(XMConvertToRadians(10 * totalTime[1])) * XMMatrixTranslation(-350, 10, 0);
	temp2 = XMMatrixIdentity();
	temp2 = XMMatrixRotationY(XMConvertToRadians(-3 * totalTime[1])) * XMMatrixTranslation(sun.objPos.x, sun.objPos.y, sun.objPos.z);
	temp = temp * temp2;
	XMStoreFloat4(&mars.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	rockPos = { myMatrix.g_World._41,myMatrix.g_World._42,myMatrix.g_World._43,myMatrix.g_World._44 };
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(mars.GO_vertex.size(), 0);


	//Draw SpaceShip
	mContext->IASetVertexBuffers(0, 1, &spaceShip.pGO_Vbuff, &spaceShip.stride, mesh_offsets);
	mContext->VSSetShader(spaceShip.pGO_VS, 0, 0);
	mContext->PSSetShader(spaceShip.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->IASetInputLayout(spaceShip.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &spaceShip.pGO_SRV_Texture);
	mContext->PSSetShaderResources(1, 1, &spaceShip.pGO_SRV_secTexture);
	mContext->PSSetShaderResources(2, 1, &spaceShip.pGO_SRV_NormTex);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	temp = XMMatrixIdentity();
	temp = XMMatrixScaling(3.0f, 3.0f, 3.0f) * XMMatrixTranslation(XMVectorGetX(secCam.camPosition), XMVectorGetY(secCam.camPosition)-5, XMVectorGetZ(secCam.camPosition)+10);
	temp = XMMatrixRotationY(XMConvertToRadians(180))* temp;
	XMStoreFloat4(&spaceShip.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	rockPos = { myMatrix.g_World._41,myMatrix.g_World._42,myMatrix.g_World._43,myMatrix.g_World._44 };
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);
	mContext->Draw(spaceShip.GO_vertex.size(), 0);

	//Draw Satellite
	mContext->IASetVertexBuffers(0, 1, &satellite.pGO_Vbuff, &satellite.stride, mesh_offsets);
	mContext->VSSetShader(satellite.pGO_VS, 0, 0);
	mContext->PSSetShader(satellite.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->IASetInputLayout(satellite.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &satellite.pGO_SRV_Texture);
	mContext->PSSetShaderResources(1, 1, &satellite.pGO_SRV_secTexture);
	mContext->PSSetShaderResources(2, 1, &satellite.pGO_SRV_NormTex);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	temp = XMMatrixIdentity();
	temp = XMMatrixScaling(0.3f, 0.3f, 0.3f)* XMMatrixTranslation(100, 0, 0);
	temp2 = XMMatrixIdentity();
	temp2 = (XMMatrixRotationY(XMConvertToRadians(10 * totalTime[1]))* XMMatrixRotationZ(XMConvertToRadians(5 * totalTime[1])))*XMMatrixTranslation(earth.objPos.x, earth.objPos.y, earth.objPos.z);
	temp = temp*temp2 ;
	
	XMStoreFloat4(&satellite.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	rockPos = { myMatrix.g_World._41,myMatrix.g_World._42,myMatrix.g_World._43,myMatrix.g_World._44 };
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);

	mysecCamCons.hasMultiTex = true;
	hr = mContext->Map(camBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &camBuffer);
	*((CamConstant*)(camBuffer.pData)) = mysecCamCons;
	mContext->Unmap(camBuff, 0);

	mContext->Draw(satellite.GO_vertex.size(), 0);

	//Draw Moon
	mContext->IASetVertexBuffers(0, 1, &moon.pGO_Vbuff, &moon.stride, mesh_offsets);
	mContext->VSSetShader(moon.pGO_VS, 0, 0);
	mContext->PSSetShader(moon.pGO_PS, 0, 0);
	mContext->PSSetConstantBuffers(0, 1, &cLightBuff);
	mContext->IASetInputLayout(moon.pGO_inputLayout);
	mContext->PSSetShaderResources(0, 1, &moon.pGO_SRV_Texture);
	mContext->PSSetShaderResources(1, 1, &moon.pGO_SRV_secTexture);
	//mContext->PSSetSamplers(0, 1, &rockSamplerState);
	mContext->VSSetConstantBuffers(0, 1, &cBuff);
	temp = XMMatrixIdentity();
	temp = XMMatrixScaling(0.8f, 0.8f, 0.8f) * XMMatrixTranslation(75, 0, 0);
	temp2 = XMMatrixIdentity();
	temp2 = (XMMatrixRotationY(XMConvertToRadians(10 * totalTime[1])) * XMMatrixRotationZ(XMConvertToRadians(15))) * XMMatrixTranslation(earth.objPos.x, earth.objPos.y, earth.objPos.z);
	temp = XMMatrixRotationX(XMConvertToRadians(10 * totalTime[1]))*temp * temp2;
	XMStoreFloat4(&moon.objPos, temp.r[3]);
	XMStoreFloat4x4(&myMatrix.g_World, temp);
	rockPos = { myMatrix.g_World._41,myMatrix.g_World._42,myMatrix.g_World._43,myMatrix.g_World._44 };
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatrix;
	mContext->Unmap(cBuff, 0);

	mContext->Draw(moon.GO_vertex.size(), 0);
}
void Update()
{
	
	delta_time=mTimer.Delta();

	if (GetAsyncKeyState('O'))
		curCamera = &firstCam;
	if (GetAsyncKeyState('P'))
		curCamera = &secCam;
	if (GetAsyncKeyState('I'))
		curCamera = nullptr;

	if (!curCamera)
		return;
	if (GetAsyncKeyState('W') )
	{
		curCamera->moveBackForward += cameraSpeed * 5*delta_time;
	}
	if (GetAsyncKeyState('S') )
	{
		curCamera->moveBackForward -= cameraSpeed * 5*delta_time;
	}
	if (GetAsyncKeyState('D') )
	{

		curCamera->moveLeftRight += cameraSpeed * 5*delta_time;
	}
	if (GetAsyncKeyState('A') )
	{
		curCamera->moveLeftRight -= cameraSpeed*5*delta_time;
	}

	if (GetAsyncKeyState(VK_NUMPAD7) )
	{
		fPlane+= 5*delta_time;

		XMMATRIX temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_NUMPAD1) )
	{
		fPlane -=  10*delta_time;
		XMMATRIX temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		if (fPlane <=10)
		{ 
			fPlane = 10.0f;
		temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		}
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	
	}
	if (GetAsyncKeyState(VK_NUMPAD8) )
	{
		XMMATRIX temp;
		FOV += 10*delta_time;
		if (FOV >= 120.0f)
		{
			FOV = 120.0f;
		}
		temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_NUMPAD2) )
	{
		XMMATRIX temp;
		FOV -= 10*delta_time;
		if (FOV <= 30.0f)
		{
			FOV = 30.0f;
		}
		temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_NUMPAD9))
	{
		nPlane +=  5*delta_time;

		XMMATRIX temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_NUMPAD3) )
	{
		XMMATRIX temp;
		nPlane -=  delta_time;
		
		if (nPlane <= 0)
		{
			nPlane = 0.01f;
		}
		temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);
	}
	if (GetAsyncKeyState(VK_NUMPAD0) & 0x1)
	{
		FOV = 75.0f;
		nPlane = 0.01f;
		fPlane = 200.0f;
		XMMATRIX temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane);
		XMStoreFloat4x4(&myMatricies.g_Projection, temp);

		curCamera->Reset();

	}
	if (GetAsyncKeyState(VK_F1) & 0x1)
	{
		GetClientRect(mWindow, &mWinR);

		multiviewPort = !multiviewPort;

		if(multiviewPort)
		{
			aspectRatio = (mWinR.right - mWinR.left)/2.0f / (mWinR.bottom - mWinR.top);
		}
		else
		{
			aspectRatio = (mWinR.right - mWinR.left) /(mWinR.bottom - mWinR.top)/1.0f;
		}
	
		XMStoreFloat4x4(&myMatricies.g_Projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane));
		XMStoreFloat4x4(&mySecWorld.g_Projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane));
	}
	if (GetAsyncKeyState(VK_F2) & 0x1)
	{
		if(!multiviewPort)
		{ 
		
		aspectRatio = (mWinR.right - mWinR.left) / (mWinR.bottom - mWinR.top) / 1.0f;
		if (SceneOne)
		{
			XMStoreFloat4x4(&myMatricies.g_Projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane));
		}
		
		else 
		{
		XMStoreFloat4x4(&mySecWorld.g_Projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane));
		}
		
		SceneOne = !SceneOne;
		}
	}
	if (GetAsyncKeyState('V') & 0x1)
	{
		lookAT = !lookAT;
		if (objPos.size() < 4)
		{
			objPos.push_back(trianglePos);
			objPos.push_back(rockPos);
			objPos.push_back(flagPos);
			objPos.push_back(stonePos);
		}
	}
	

	GetCursorPos(&CurPos);

	if (GetAsyncKeyState(VK_RBUTTON) && (lastPos.x != CurPos.x || lastPos.y != CurPos.y))
	{
		curCamera->camYaw += (CurPos.x-lastPos.x) * 0.01f;

		curCamera->camPitch += (CurPos.y-lastPos.y)  * 0.01f;
		RECT rect;
		GetWindowRect(mWindow, &rect);
		if (CurPos.x >= rect.right)
		{
			CurPos.x = rect.left;
		}
		else if (CurPos.x <= rect.left)
		{
			CurPos.x = rect.right;
		}
		else if (CurPos.y <= rect.top)
		{
			CurPos.y = rect.bottom;
		}
		else if (CurPos.y >= rect.bottom - 1)
		{
			CurPos.y = rect.top;
		}
		SetCursorPos(CurPos.x, CurPos.y);

		lastPos.x = CurPos.x;
		lastPos.y = CurPos.y;
	}
	curCamera->UpdateCamera();

	if (lookAT)
	{
		if (GetAsyncKeyState(VK_UP) & 0x1)
		{
			if (curObj < objPos.size())
				curObj++;
			if (curObj == objPos.size())
				curObj = 0;
		}
		objPos[0] = trianglePos;
		objPos[1] = rockPos;
		objPos[2] = flagPos;
		objPos[3] = trianglePos;
		curCamera->camTarget = XMLoadFloat4(&objPos[curObj]);
	}

	XMStoreFloat4(&camPos, curCamera->camPosition);
	if (curCamera == &firstCam)
	{
		curCamera->LoadViewMatrix(myMatricies.g_View);
	}

	if(curCamera==&secCam)
	{
		curCamera->LoadViewMatrix(mySecWorld.g_View);
	}
}

void CreateSphere(int LatLines, int LongLines)
{
	int NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	int NumSphereFaces = ((LatLines - 3) * (LongLines) * 2) + (LongLines * 2);
	spVertexNum = NumSphereFaces*3;
	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<SimpleVertex> vertices(NumSphereVertices);

	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	vertices[0].Pos.x = 0.0f;
	vertices[0].Pos.y = 0.0f;
	vertices[0].Pos.z = 1.0f;

	for (unsigned int i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14 / (LatLines - 1));
		Rotationx = XMMatrixRotationX(spherePitch);
		for (unsigned int j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * (6.28 / (LongLines));
			Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);
			vertices[i * LongLines + j + 1].Pos.x = XMVectorGetX(currVertPos);
			vertices[i * LongLines + j + 1].Pos.y = XMVectorGetY(currVertPos);
			vertices[i * LongLines + j + 1].Pos.z = XMVectorGetZ(currVertPos);
		}
	}

	vertices[NumSphereVertices - 1].Pos.x = 0.0f;
	vertices[NumSphereVertices - 1].Pos.y = 0.0f;
	vertices[NumSphereVertices - 1].Pos.z = 1.0f;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = vertices.data();
	HRESULT hr = mDev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &spVertBuffer);


	vector<unsigned int> indices(NumSphereFaces * 3);

	int k = 0;
	for (unsigned int l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for (unsigned int i = 0; i < LatLines - 3; ++i)
	{
		for (unsigned int j = 0; j < LongLines - 1; ++j)
		{
			indices[k] = i * LongLines + j + 1;
			indices[k + 1] = i * LongLines + j + 2;
			indices[k + 2] = (i + 1) * LongLines + j + 1;

			indices[k + 3] = (i + 1) * LongLines + j + 1;
			indices[k + 4] = i * LongLines + j + 2;
			indices[k + 5] = (i + 1) * LongLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i * LongLines) + LongLines;
		indices[k + 1] = (i * LongLines) + 1;
		indices[k + 2] = ((i + 1) * LongLines) + LongLines;

		indices[k + 3] = ((i + 1) * LongLines) + LongLines;
		indices[k + 4] = (i * LongLines) + 1;
		indices[k + 5] = ((i + 1) * LongLines) + 1;

		k += 6;
	}

	for (unsigned int l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = NumSphereVertices - 1;
		indices[k + 1] = (NumSphereVertices - 1) - (l + 1);
		indices[k + 2] = (NumSphereVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = NumSphereVertices - 1;
	indices[k + 1] = (NumSphereVertices - 1) - LongLines;
	indices[k + 2] = NumSphereVertices - 2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices.data();
	mDev->CreateBuffer(&indexBufferDesc, &iinitData, &spIndexBuffer);

}

bool loadObject(const char* path, std::vector <SimpleMesh>& outVertices)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector<XMFLOAT3> tempVerts;
	std::vector<XMFLOAT3> tempUVs;
	std::vector<XMFLOAT3> tempNormals;
	FILE* file;
    fopen_s(&file,path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (true)
	{
		char line[128];
		int res = fscanf_s(file, "%s",line,sizeof(line));
		if (res == EOF)
			break;
		if (strcmp(line, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex.z = -vertex.z;
			tempVerts.push_back(vertex);
		}
		else if (strcmp(line, "vt") == 0)
		{
			XMFLOAT3 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = 1 - uv.y;
			tempUVs.push_back(uv);
		}
		else if (strcmp(line, "vn") == 0)
		{
			XMFLOAT3 norm;
			fscanf_s(file, "%f %f %f\n", &norm.x, &norm.y, &norm.z);
				norm.z = -norm.z;
			tempNormals.push_back(norm);
		}
		else if (strcmp(line, "f") == 0) 
		{
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[0], &uvIndex[0], &normalIndex[0]);
			if (matches != 9) {
				printf("File can't be read by this simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{

		SimpleMesh temp;
		temp.Pos = tempVerts[vertexIndices[i] - 1];
		temp.Tex = tempUVs[uvIndices[i] - 1];
		temp.Norm = tempNormals[normalIndices[i] - 1];
		temp.Tangent = {0,0,0,0};
		outVertices.push_back(temp);
	}

	for (size_t i = 0; i < outVertices.size(); i++)
	{

		if (i % 3 == 0)
		{

			XMFLOAT3 edge1 = { outVertices[i + 1].Pos.x - outVertices[i].Pos.x,outVertices[i + 1].Pos.y - outVertices[i].Pos.y,outVertices[i + 1].Pos.z - outVertices[i].Pos.z };
			XMFLOAT3 edge2 = { outVertices[i + 2].Pos.x - outVertices[i].Pos.x,outVertices[i + 2].Pos.y - outVertices[i].Pos.y,outVertices[i + 2].Pos.z - outVertices[i].Pos.z };
			float txU1 = outVertices[i + 1].Tex.x - outVertices[i].Tex.x;
			float txV1 = outVertices[i + 1].Tex.y - outVertices[i].Tex.y;
			float txU2 = outVertices[i + 2].Tex.x - outVertices[i].Tex.x;
			float txV2 = outVertices[i + 2].Tex.y - outVertices[i].Tex.y;
			float ratio = 1.0f / (txU1 * txV2 - txU2 * txV1);
			XMFLOAT3 uDirection = {
				(txV2 * edge1.x - txV1 * edge2.x) * ratio,
				(txV2 * edge1.y - txV1 * edge2.y) * ratio,
				(txV2 * edge1.z - txV1 * edge2.z) * ratio
			};

			XMFLOAT3 vDirection = {
				(txU1 * edge2.x - txU2 * edge1.x) * ratio,
				(txU1 * edge2.y - txU2 * edge1.y) * ratio,
				(txU1 * edge2.z - txU2 * edge1.z) * ratio
			};

			XMVECTOR uDirNor = XMVector3Normalize(XMLoadFloat3(&uDirection));
			XMVECTOR dotResult = XMVector3Dot(XMLoadFloat3(&outVertices[i].Norm), uDirNor);
			XMVECTOR tan = uDirNor - XMLoadFloat3(&outVertices[i].Norm) * dotResult;
			tan = XMVector3Normalize(tan);
			XMFLOAT3 temp;
			XMStoreFloat3(&temp, tan);
			outVertices[i].Tangent.x = temp.x;
			outVertices[i].Tangent.y = temp.y;
			outVertices[i].Tangent.z = temp.z;
			XMVECTOR vDirNor = XMVector3Normalize(XMLoadFloat3(&vDirection));
			XMVECTOR crossResult = XMVector3Cross(XMLoadFloat3(&outVertices[i].Norm), XMLoadFloat3(&uDirection));
			XMVECTOR dotResult1 = XMVector3Dot(crossResult, vDirNor);
			XMFLOAT3 dot;
			XMStoreFloat3(&dot, dotResult1);
		}

	}
	return true;
}
void WindowResize(UINT _width, UINT _height)
{
	aspectRatio = _width/(float)_height;
	if (multiviewPort)
		aspectRatio = (_width / 2) / (float)_height;
	XMStoreFloat4x4(&myMatricies.g_Projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, nPlane, fPlane));
}


void LoadGameObject()
{
	/////Theme One
	island.CreateGameObject(mDev, "Assets/island.obj", VertexMeshShader, sizeof(VertexMeshShader));
	CreateDDSTextureFromFile(mDev, L"Assets/Textures/island_Sand.dds", NULL, &island.pGO_SRV_Texture);
	mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &island.pGO_VS);
	mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &island.pGO_PS);

	player.CreateGameObject(mDev, "Assets/player.obj", VertexMeshShader, sizeof(VertexMeshShader));
	CreateDDSTextureFromFile(mDev, L"Assets/Textures/player_Diffuse.dds", NULL, &player.pGO_SRV_Texture);
	mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &player.pGO_VS);
	mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &player.pGO_PS);
	
	fussy.CreateGameObject(mDev, "Assets/fussy.obj", VertexMeshShader, sizeof(VertexMeshShader));
	CreateDDSTextureFromFile(mDev, L"Assets/Textures/fussy_Diffuse.dds", NULL, &fussy.pGO_SRV_Texture);
	mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &fussy.pGO_VS);
	mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &fussy.pGO_PS);

	ground.CreateGameObject(mDev, "Assets/ocean.obj", VertexMeshShader, sizeof(VertexMeshShader));
	CreateDDSTextureFromFile(mDev, L"Assets/Textures/ocean_Diffuse.dds", NULL, &ground.pGO_SRV_Texture);
	mDev->CreateVertexShader(VertexWaveShader, sizeof(VertexWaveShader), nullptr, &ground.pGO_VS);
	mDev->CreatePixelShader(Water_PS, sizeof(Water_PS), nullptr, &ground.pGO_PS);

	palmTree.CreateGameObject(mDev, "Assets/palmTree.obj", VertexMeshShader, sizeof(VertexMeshShader));
	CreateDDSTextureFromFile(mDev, L"Assets/Textures/palm_Diffuse.dds", NULL, &palmTree.pGO_SRV_Texture);
	mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &palmTree.pGO_VS);
	mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &palmTree.pGO_PS);

	palmTree1.CreateGameObject(mDev, "Assets/palmTree1.obj", VertexMeshShader, sizeof(VertexMeshShader));
	CreateDDSTextureFromFile(mDev, L"Assets/Textures/palm1_Diffuse.dds", NULL, &palmTree1.pGO_SRV_Texture);
	mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &palmTree1.pGO_VS);
	mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &palmTree1.pGO_PS);

	treasureChest.CreateGameObject(mDev, "Assets/treasureChest.obj", VertexMeshShader, sizeof(VertexMeshShader));
	CreateDDSTextureFromFile(mDev, L"Assets/Textures/Chest_Diffuse.dds", NULL, &treasureChest.pGO_SRV_Texture);
	mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &treasureChest.pGO_VS);
	mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &treasureChest.pGO_PS);


	///Theme Two
	spaceSkybox.CreateGameObject(mDev, "Assets/spaceSkybox.obj", SkyVertexShader, sizeof(SkyVertexShader));
	CreateDDSTextureFromFile(mDev, L"Assets/Textures/spaceSkybox.dds", NULL, &spaceSkybox.pGO_SRV_Texture);
	mDev->CreateVertexShader(SkyVertexShader, sizeof(SkyVertexShader), nullptr, &spaceSkybox.pGO_VS);
	mDev->CreatePixelShader(SkyPixelShader, sizeof(SkyPixelShader), nullptr, &spaceSkybox.pGO_PS);

	sun.CreateGameObject(mDev, "Assets/Planet.obj", VertexMeshShader, sizeof(VertexMeshShader));
	CreateDDSTextureFromFile(mDev, L"Assets/Textures/sun_Diffuse.dds", NULL, &sun.pGO_SRV_Texture);
	mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &sun.pGO_VS);
	mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &sun.pGO_PS);

	earth.CreateGameObject(mDev, "Assets/Planet.obj", VertexMeshShader, sizeof(VertexMeshShader));
	CreateDDSTextureFromFile(mDev, L"Assets/Textures/Earth.dds", NULL, &earth.pGO_SRV_Texture);
	mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &earth.pGO_VS);
	mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &earth.pGO_PS);

	 moon.CreateGameObject(mDev, "Assets/moon.obj", VertexMeshShader, sizeof(VertexMeshShader));
	 CreateDDSTextureFromFile(mDev, L"Assets/Textures/moon_Diffuse.dds", NULL, &moon.pGO_SRV_Texture);
	 CreateDDSTextureFromFile(mDev, L"Assets/Textures/moon_Spec.dds", NULL, &moon.pGO_SRV_secTexture);
	 mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &moon.pGO_VS);
	 mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &moon.pGO_PS);

	 mars.CreateGameObject(mDev, "Assets/Planet.obj", VertexMeshShader, sizeof(VertexMeshShader));
	 CreateDDSTextureFromFile(mDev, L"Assets/Textures/Mars.dds", NULL, &mars.pGO_SRV_Texture);
	 mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &mars.pGO_VS);
	 mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &mars.pGO_PS);

	 satellite.CreateGameObject(mDev, "Assets/satellite.obj", VertexMeshShader, sizeof(VertexMeshShader));
	 CreateDDSTextureFromFile(mDev, L"Assets/Textures/Satellite_Diffuse.dds", NULL, &satellite.pGO_SRV_Texture);
	 CreateDDSTextureFromFile(mDev, L"Assets/Textures/Satellite_Spec.dds", NULL, &satellite.pGO_SRV_secTexture);
	 CreateDDSTextureFromFile(mDev, L"Assets/Textures/Satellite_Norm.dds", NULL, &satellite.pGO_SRV_NormTex);
	 mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &satellite.pGO_VS);
	 mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &satellite.pGO_PS);

	 spaceShip.CreateGameObject(mDev, "Assets/spaceShip.obj", VertexMeshShader, sizeof(VertexMeshShader));
	 CreateDDSTextureFromFile(mDev, L"Assets/Textures/spaceShip_talon.dds", NULL, &spaceShip.pGO_SRV_Texture);
	 mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &spaceShip.pGO_VS);
	 mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &spaceShip.pGO_PS);
}




void CleanupDevice()
{
	if(mRTV)mRTV->Release();
	if(mContext)mContext->Release();
	if(mSwap)mSwap->Release();
	if (mDev)mDev->Release();
	if (mDeferredContext)mDeferredContext->Release();
	if (mCommandList)mCommandList->Release();
	if (mBlendState)mBlendState->Release();
	if (RSCullNone)RSCullNone->Release();
	if (zBuffer)zBuffer->Release();
	if (zBufferView)zBufferView->Release();

	if (cLightBuff)cLightBuff->Release();
	if (cBuff)cBuff->Release();
	if (timerBuff)timerBuff->Release();
	if (camBuff)camBuff->Release();
	//triangle
	if(vBuff)vBuff->Release();
	if (vShader)vShader->Release();
	if (pShader)pShader->Release();
	if (vLayout)vLayout->Release();

	//Sphere
	if(spIndexBuffer)spIndexBuffer->Release();
	if(spVertBuffer)spVertBuffer->Release();
	if(spLayout) spLayout->Release();
	if(spVShader)spVShader->Release(); //HLSL
	if(spPShader)spPShader->Release();  //HLSL

	//skybox
	if (vSkyBuff)vSkyBuff->Release();
	if (iSkyBuff)iSkyBuff->Release();
	if (vSkyShader)vSkyShader->Release();
	if (pSkyShader)pSkyShader->Release();
	if (skyLayout)skyLayout->Release();
	if (skyBoxTextureRV)skyBoxTextureRV->Release();
	if (skyBoxSamplerState)skyBoxSamplerState->Release();

	//rock
	if (vRockBuff)vRockBuff->Release();
	if (vRockShader)vRockShader->Release();
	if (pRockShader)pRockShader->Release();
	if (rockSamplerState)rockSamplerState->Release();
	if (rockTextureRV)rockTextureRV->Release();
	if (rockTexture)rockTexture->Release();
	if (vRockLayout)vRockLayout->Release();
	if (rockTextureRV1)rockTextureRV1->Release();

	//flag
	if (vFlagBuff)vFlagBuff->Release();
	if(vFlagShader)		 vFlagShader->Release();
	if (pFlagShader)		  pFlagShader->Release();
	if (flagTextureRV)	 flagTextureRV ->Release();
	if (flagTexture)		flagTexture ->Release();
	if (vFlagLayout)		  vFlagLayout ->Release();
	if (flagSamplerState)	   flagSamplerState ->Release();
	//stone
	if (vStoneBuff)vStoneBuff->Release();
	if (iStoneBuff)iStoneBuff->Release();
	if (vStoneShader)vStoneShader->Release();
	if (pStoneShader)pStoneShader->Release();
	if (stoneTextureRV)stoneTextureRV->Release();
	if (stoneTexture)stoneTexture->Release();
	if (stoneSamplerState)stoneSamplerState->Release();
	if (vStoneLayout)vStoneLayout->Release();

	if(treeInstanceBuff)treeInstanceBuff->Release();
	if(treeVertBuff)treeVertBuff->Release();
	if (treeInsLayout) treeInsLayout->Release();
	if (treeTextureRV)treeTextureRV->Release();
	if (treeVShader)treeVShader->Release();
	if (treePShader)treePShader->Release();
}	   