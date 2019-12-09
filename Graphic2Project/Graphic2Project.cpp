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

void createSphere(float fRadius, UINT uSlices, UINT uStacks);

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
		UpdateCamera();
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   
   RECT mWinR;
   GetClientRect(hWnd, &mWinR);

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
   swap.OutputWindow = hWnd;
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

   ID3D11Resource* backBuffer;
   hr = mSwap->GetBuffer(0, __uuidof(backBuffer), (void**)&backBuffer);
   hr = mDev->CreateRenderTargetView(backBuffer, NULL, &mRTV);

   backBuffer->Release();


   mPort.Height = swap.BufferDesc.Height;
   mPort.Width = swap.BufferDesc.Width;
   mPort.TopLeftX = mPort.TopLeftY = 0;
   mPort.MinDepth = 0;
   mPort.MaxDepth = 1;

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
   D3D11_BUFFER_DESC bDesc;
   D3D11_SUBRESOURCE_DATA subData;
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


   //Constant Buffer
   ZeroMemory(&bDesc, sizeof(bDesc));

   bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
   bDesc.ByteWidth = sizeof(WVP);
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_DYNAMIC;

   hr= mDev->CreateBuffer(&bDesc, nullptr, &cBuff);

   //createSphere(0.2f, 20, 20);



   //Load Rock Mesh
   ZeroMemory(&bDesc, sizeof(bDesc));
   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(Rock_data);
   bDesc.MiscFlags = 0;
   bDesc.CPUAccessFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;

   subData.pSysMem = Rock_data;

   hr=mDev->CreateBuffer(&bDesc, &subData, &vRockBuff);

   //Index Buffer mesh
   bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bDesc.ByteWidth = sizeof(Rock_indicies);
   subData.pSysMem = Rock_indicies;
   hr = mDev->CreateBuffer(&bDesc, &subData, &iRockBuff);

   //Load New Mesh
   hr = mDev->CreateVertexShader(VertexMeshShader, sizeof(VertexMeshShader), nullptr, &vRockShader);
   hr = mDev->CreatePixelShader(PixelMeshShader, sizeof(PixelMeshShader), nullptr, &pRockShader);
   D3D11_INPUT_ELEMENT_DESC meshInputDesc[] =
   {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   };
   // Load the Texture
   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/Rock_Diffuse.dds", (ID3D11Resource**)&rockTexture, &rockTextureRV);

   // Create the sample state
   D3D11_SAMPLER_DESC sampDesc = {};
   sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
   sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
   sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
   sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
   sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
   sampDesc.MinLOD = 0;
   sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
   hr = mDev->CreateSamplerState(&sampDesc, &rockSamplerState);

   hr = mDev->CreateInputLayout(meshInputDesc, 3, VertexMeshShader, sizeof(VertexMeshShader), &vRockLayout);

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
   D3D11_INPUT_ELEMENT_DESC meshInputDesc2[] =
   {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   };
   // Load the Texture
   hr = CreateDDSTextureFromFile(mDev, L"Assets/Textures/StoneHenge.dds", (ID3D11Resource**)&stoneTexture, &stoneTextureRV);

   // Create the sample state
   ZeroMemory(&sampDesc, sizeof(sampDesc));
   sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
   sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
   sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
   sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
   sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
   sampDesc.MinLOD = 0;
   sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
   hr = mDev->CreateSamplerState(&sampDesc, &stoneSamplerState);

   hr = mDev->CreateInputLayout(meshInputDesc2, 3, VertexMeshShader, sizeof(VertexMeshShader), &vStoneLayout);


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
   

   // Initialize the world matrices
   XMMATRIX temp = XMMatrixIdentity();
   XMStoreFloat4x4(&myMatricies.g_World,temp);

   // Initialize the view matrix
   XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
   XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
   XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
   temp = XMMatrixLookAtLH(Eye, At, Up);
   XMStoreFloat4x4(&myMatricies.g_View, temp);

   temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, 0.1f, 1000);
   XMStoreFloat4x4(&myMatricies.g_Projection, temp);


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
	// Update our time
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		t = (timeCur - timeStart) / 1000.0f;
	}

	float color[] = { 0.5, 0.5, 0.5, 1 };
	mContext->ClearRenderTargetView(mRTV, color);

	mContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);


	//Setup the pipeline
	//output merger
	ID3D11RenderTargetView* tempRTV[] = { mRTV };
	mContext->OMSetRenderTargets(1, tempRTV, zBufferView);
	// rasterizer
	mContext->RSSetViewports(1, &mPort);
	// Input Assembler

	mContext->IASetInputLayout(vLayout);
	UINT strides[] = { sizeof(SimpleVertex) };
	UINT offsets[] = { 0 };
	ID3D11Buffer* tempVB[] = { vBuff };
	mContext->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mContext->VSSetShader(vShader, 0, 0);
	mContext->PSSetShader(pShader, 0, 0);

	XMMATRIX temp = XMMatrixIdentity();
	temp = XMMatrixTranslation(10, 0, 4);
	XMMATRIX temp2 = XMMatrixRotationY(t);
	temp = XMMatrixMultiply(temp2, temp);
	XMStoreFloat4x4(&myMatricies.g_World, temp);

	D3D11_MAPPED_SUBRESOURCE gpuBuffer;
	HRESULT hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatricies;
	mContext->Unmap(cBuff, 0);

	ID3D11Buffer* constants[] = { cBuff };
	mContext->VSSetConstantBuffers(0, 1, constants);

	mContext->Draw(numVerts, 0);

	//mContext->IASetInputLayout(vLayout);
	//tempVB[0] = { vBuff1 };
	//mContext->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
	//mContext->IASetIndexBuffer(iBuff1, DXGI_FORMAT_R32_UINT, 0);
	//mContext->VSSetShader(vShader1, 0, 0);
	//mContext->PSSetShader(pShader1, 0, 0);
	//temp = XMMatrixIdentity();
	//temp = XMMatrixTranslation(-5, 5, 4);
	//XMStoreFloat4x4(&myMatricies.g_World, temp);
	//hr = mContext->Map(cBuff1, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	//*((WVP*)(gpuBuffer.pData)) = myMatricies;
	//mContext->Unmap(cBuff1, 0);
	//constants[0] = { cBuff1 };
	//mContext->VSSetConstantBuffers(0, 1, constants);
	//mContext->DrawIndexed(2280, 0,0);


	//Draw Rock
	ID3D11ShaderResourceView* texView[] = { rockTextureRV };
	mContext->PSSetShaderResources(0, 1, texView);
	mContext->PSSetSamplers(0, 1, &rockSamplerState);

	//Set Pipline
	UINT mesh_strides[] = { sizeof(_OBJ_VERT_) };
	UINT mesh_offsets[] = { 0 };
	ID3D11Buffer* meshVB[] = { vRockBuff };
	mContext->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
	mContext->IASetIndexBuffer(iRockBuff, DXGI_FORMAT_R32_UINT, 0);
	mContext->VSSetShader(vRockShader, 0, 0);
	mContext->PSSetShader(pRockShader, 0, 0);
	mContext->IASetInputLayout(vRockLayout);

	temp = XMMatrixIdentity();
	temp = XMMatrixTranslation(-10,0,-2);
	XMStoreFloat4x4(&myMatricies.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatricies;
	mContext->Unmap(cBuff, 0);

	mContext->DrawIndexed(1908, 0, 0);


	texView[0] = { stoneTextureRV };
	mContext->PSSetShaderResources(0, 1, texView);
	mContext->PSSetSamplers(0, 1, &stoneSamplerState);

	//Set Pipline
	mesh_strides[0] = { sizeof(_OBJ_VERT_) };
	mesh_offsets[0] = { 0 };
	meshVB[0] = { vStoneBuff };
	mContext->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
	mContext->IASetIndexBuffer(iStoneBuff, DXGI_FORMAT_R32_UINT, 0);
	mContext->VSSetShader(vStoneShader, 0, 0);
	mContext->PSSetShader(pStoneShader, 0, 0);
	mContext->IASetInputLayout(vStoneLayout);

	temp = XMMatrixIdentity();
	XMStoreFloat4x4(&myMatricies.g_World, temp);
	hr = mContext->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	*((WVP*)(gpuBuffer.pData)) = myMatricies;
	mContext->Unmap(cBuff, 0);
	mContext->DrawIndexed(2532, 0, 0);

	mSwap->Present(1, 0);
}
void UpdateCamera()
{
	
	delta_time=mTimer.Delta();
	if (GetAsyncKeyState('W'))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -cameraSpeed * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('S'))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, cameraSpeed * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('D'))
	{

		XMMATRIX translation = XMMatrixTranslation(-cameraSpeed * 2*delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('A'))
	{
		XMMATRIX translation = XMMatrixTranslation(cameraSpeed * 2*delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('X'))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, -cameraSpeed * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('Q'))
	{
		XMMATRIX rotation = XMMatrixRotationY(-cameraSpeed * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(rotation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState('E'))
	{
		XMMATRIX rotation = XMMatrixRotationY(cameraSpeed * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(rotation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, cameraSpeed * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&myMatricies.g_View);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&myMatricies.g_View, result);
	}
}

void createSphere(float fRadius, UINT uSlices, UINT uStacks)
{
	UINT cFaces = 2 * (uStacks - 1) * uSlices;
	UINT cVertices = (uStacks - 1) * uSlices + 2;

	SimpleVertex* vertices = new SimpleVertex[cVertices];


	UINT i, j;

	const int CACHE_SIZE = 240 * 2;

	// Sin/Cos caches
	float sinI[CACHE_SIZE], cosI[CACHE_SIZE];
	float sinJ[CACHE_SIZE], cosJ[CACHE_SIZE];

	for (i = 0; i < uSlices; i++) {
		sinI[i] = sinf((float)(2.0f * XM_PI * i / uSlices));
		cosI[i] = cosf((float)(2.0f * XM_PI * i / uSlices));
		// sincosf( 2.0f * D3DX_PI * i / uSlices, sinI + i, cosI + i );
	}
	for (j = 0; j < uStacks; j++) {
		sinJ[j] = sinf((float)(XM_PI * j / uStacks));
		cosJ[j] = cosf((float)(XM_PI * j / uStacks));
		// sincosf( D3DX_PI * j / uStacks, sinJ + j, cosJ + j );
	}

	// Generate vertices
	SimpleVertex* pVertex = vertices;

	// +Z pole
	pVertex->Pos = XMFLOAT4(0.0f, 0.0f, fRadius,1.0f);
	pVertex->Color = XMFLOAT4(0.0f, 0.0f, 1.0f,1.0f);
	pVertex++;

	// Stacks
	for (j = 1; j < uStacks; j++)
	{
		for (i = 0; i < uSlices; i++)
		{
			XMFLOAT3 norm(sinI[i] * sinJ[j], cosI[i] * sinJ[j], cosJ[j]);
			XMFLOAT4 pos;
			pos.x = norm.x * fRadius;
			pos.y = norm.y * fRadius;
			pos.z = norm.z * fRadius;

			pVertex->Pos = pos; //norm * fRadius;
			pVertex->Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

			pVertex++;
		}
	}

	// Z- pole
	pVertex->Pos = XMFLOAT4(0.0f, 0.0f, -fRadius,1.0f);
	pVertex->Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	pVertex++;


	UINT* indices = new UINT[cFaces * 3];

	// Generate indices
	UINT* pwFace = indices;

	// Z+ pole
	UINT uRowA = 0;
	UINT uRowB = 1;

	for (i = 0; i < uSlices - 1; i++)
	{
		pwFace[0] = (WORD)(uRowA);
		pwFace[1] = (WORD)(uRowB + i + 1);
		pwFace[2] = (WORD)(uRowB + i);
		pwFace += 3;
	}

	pwFace[0] = (WORD)(uRowA);
	pwFace[1] = (WORD)(uRowB);
	pwFace[2] = (WORD)(uRowB + i);
	pwFace += 3;

	// Interior stacks
	for (j = 1; j < uStacks - 1; j++)
	{
		uRowA = 1 + (j - 1) * uSlices;
		uRowB = uRowA + uSlices;

		for (i = 0; i < uSlices - 1; i++)
		{
			pwFace[0] = (WORD)(uRowA + i);
			pwFace[1] = (WORD)(uRowA + i + 1);
			pwFace[2] = (WORD)(uRowB + i);
			pwFace += 3;

			pwFace[0] = (WORD)(uRowA + i + 1);
			pwFace[1] = (WORD)(uRowB + i + 1);
			pwFace[2] = (WORD)(uRowB + i);
			pwFace += 3;
		}

		pwFace[0] = (WORD)(uRowA + i);
		pwFace[1] = (WORD)(uRowA);
		pwFace[2] = (WORD)(uRowB + i);
		pwFace += 3;

		pwFace[0] = (WORD)(uRowA);
		pwFace[1] = (WORD)(uRowB);
		pwFace[2] = (WORD)(uRowB + i);
		pwFace += 3;
	}

	// Z- pole
	uRowA = 1 + (uStacks - 2) * uSlices;
	uRowB = uRowA + uSlices;

	for (i = 0; i < uSlices - 1; i++)
	{
		pwFace[0] = (WORD)(uRowA + i);
		pwFace[1] = (WORD)(uRowA + i + 1);
		pwFace[2] = (WORD)(uRowB);
		pwFace += 3;
	}

	pwFace[0] = (WORD)(uRowA + i);
	pwFace[1] = (WORD)(uRowA);
	pwFace[2] = (WORD)(uRowB);
	pwFace += 3;


	D3D11_BUFFER_DESC bDesc;
	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(SimpleVertex);
	bDesc.MiscFlags = 0;
	bDesc.CPUAccessFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subData.pSysMem = pVertex;

	mDev->CreateBuffer(&bDesc, &subData, &vBuff1);


	//Index Buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(UINT)* cFaces * 3;
	subData.pSysMem = pwFace;
	mDev->CreateBuffer(&bDesc, &subData, &iBuff1);


	mDev->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vShader1);
	mDev->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pShader1);

	//D3D11_INPUT_ELEMENT_DESC ieDesc[] =
	//{
	//	 { "POSITION" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	 { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};
	//mDev->CreateInputLayout(ieDesc, 2, VertexShader, sizeof(VertexShader), &vLayout);


	//Constant Buffer
	ZeroMemory(&bDesc, sizeof(bDesc));

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(WVP);
	bDesc.MiscFlags = 0;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;

	mDev->CreateBuffer(&bDesc, nullptr, &cBuff1);

	delete[] vertices;
	delete[] indices;
}

void CleanupDevice()
{
	if(mRTV)mRTV->Release();
	if(mContext)mContext->Release();
	if(mSwap)mSwap->Release();
	if (mDev)mDev->Release();

	if (zBuffer)zBuffer->Release();
	if (zBufferView)zBufferView->Release();

	if(vBuff)vBuff->Release();
	if(cBuff)cBuff->Release();
	if (vShader)vShader->Release();
	if (pShader)pShader->Release();
	if (vLayout)vLayout->Release();

	if (vBuff1)vBuff1->Release();
	if (iBuff1)iBuff1->Release();
	if (cBuff1)cBuff1->Release();
	if (vShader1)vShader1->Release();
	if (pShader1)pShader1->Release();
	//if (vLayout1)vLayout1->Release();
	//rock
	if (vRockBuff)vRockBuff->Release();
	if (iRockBuff)iRockBuff->Release();
	if (vRockShader)vRockShader->Release();
	if (pRockShader)pRockShader->Release();
	if (rockSamplerState)rockSamplerState->Release();
	if (rockTextureRV)rockTextureRV->Release();
	if (rockTexture)rockTexture->Release();
	if (vRockLayout)vRockLayout->Release();

	//stone
	if (vStoneBuff)vStoneBuff->Release();
	if (iStoneBuff)iStoneBuff->Release();
	if (vStoneShader)vStoneShader->Release();
	if (pStoneShader)pStoneShader->Release();
	if (stoneTextureRV)stoneTextureRV->Release();
	if (stoneTexture)stoneTexture->Release();
	if (stoneSamplerState)stoneSamplerState->Release();
	if (vStoneLayout)vStoneLayout->Release();


}