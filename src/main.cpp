#include "GLFW/glfw3.h"
#define WIN32_LEAN_AND_MEAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include <iostream>
#include <d3d11.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl.h>

#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }
#define HR(x, s) { HRESULT hr = (x); if (FAILED(hr)) { MessageBox(nullptr, s, L"Error", MB_OK); } }

using Microsoft::WRL::ComPtr;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMFLOAT4X4;

GLFWwindow* window;
HWND hWnd;
ComPtr<ID3D11Device> m_pd3dDevice;
ComPtr<ID3D11DeviceContext> m_pd3dDeviceContext;
ComPtr<IDXGISwapChain> m_pDXGISwapChain;
ComPtr<ID3D11Texture2D> m_pBackBuffer;
ComPtr<ID3D11RenderTargetView> m_pRenderTarget;
D3D11_TEXTURE2D_DESC m_bbDesc;
ComPtr<ID3D11Texture2D> m_pDepthStencil;
ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
D3D11_VIEWPORT m_viewport;
ComPtr<ID3D11VertexShader> m_pVertexShader;
ComPtr<ID3D11PixelShader> m_pPixelShader;
ComPtr<ID3D11InputLayout> m_pVertexLayout;
float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

typedef struct _mvpConstantBuffer {
    XMFLOAT4X4 world;
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;
} MvpConstantBuffer;

typedef struct _vertexPosColor {
    XMFLOAT3 pos;
    XMFLOAT4 color;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
} VertexPosColor;

const D3D11_INPUT_ELEMENT_DESC VertexPosColor::inputLayout[2] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

void onError(int code, const char* description) {
    std::cerr << "GLFW Error code " << code << ": " << description << std::endl;
}

HRESULT createShaderFromFile(
    const wchar_t* csoFilename,
    const wchar_t* hlslFilename,
    LPCSTR entrypoint,
    LPCSTR shaderModel,
    ID3DBlob** ppBlobOut
) {
    HRESULT hr = S_OK;

    if (csoFilename != nullptr && D3DReadFileToBlob(csoFilename, ppBlobOut) == S_OK)
        return hr;
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG
    ID3DBlob* errorBlob = nullptr;
    hr = D3DCompileFromFile(hlslFilename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint, shaderModel,
        dwShaderFlags, 0, ppBlobOut, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob != nullptr) {
            OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
        }
        SAFE_RELEASE(errorBlob);
        return hr;
    }
    if (csoFilename != nullptr) {
        return D3DWriteBlobToFile(*ppBlobOut, csoFilename, FALSE);
    }
    return hr;
}

bool initEffect() {
    ComPtr<ID3DBlob> blob;
    HR(createShaderFromFile(L"res/shaders/VertexShader.cso", L"res/shaders/VertexShader.hlsl", "main", "vs_5_0", blob.ReleaseAndGetAddressOf()), L"Failed to load vertex shader!");
    HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()), L"Failed to create vertex shader!");
    HR(m_pd3dDevice->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
        blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()), L"Failed to create input layout!");
    HR(createShaderFromFile(L"res/shaders/PixelShader.cso", L"res/shaders/PixelShader.hlsl", "main", "ps_5_0", blob.ReleaseAndGetAddressOf()), L"Failed to load pixel shader!");
    HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()), L"Failed to create pixel shader!");
    return true;
}

void initResource() {
    VertexPosColor vertices[] = {
        { XMFLOAT3(0.0f, 0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }
    };
    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(vertices);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    ComPtr<ID3D11Buffer> m_pVertexBuffer;
    HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()), L"Failed to create vertex buffer!");

    UINT stride = sizeof(VertexPosColor);
    UINT offset = 0;
    m_pd3dDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
    m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pd3dDeviceContext->IASetInputLayout(m_pVertexLayout.Get());
    m_pd3dDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
    m_pd3dDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
}

void initDX() {
    hWnd = glfwGetWin32Window(window);
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_11_1
    };

    // This flag adds support for surfaces with a color-channel ordering different
    // from the API default. It is required for compatibility with Direct2D.
    UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // Create the Direct3D 11 API device object and a corresponding context.
    ComPtr<ID3D11Device>        device;
    ComPtr<ID3D11DeviceContext> context;

    D3D_FEATURE_LEVEL m_featureLevel;
    HRESULT hr = D3D11CreateDevice(
        nullptr,                    // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
        0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        deviceFlags,                // Set debug and Direct2D compatibility flags.
        levels,                     // List of feature levels this app can support.
        ARRAYSIZE(levels),          // Size of the list above.
        D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
        &device,                    // Returns the Direct3D device created.
        &m_featureLevel,            // Returns feature level of device created.
        &context                    // Returns the device immediate context.
    );

    if (FAILED(hr)) {
        // Handle device interface creation failure if it occurs.
        // For example, reduce the feature level requirement, or fail over 
        // to WARP rendering.
        MessageBox(nullptr, L"Failed to create the D3D device", L"Error", MB_OK);
    }

    // Store pointers to the Direct3D 11.1 API device and immediate context.
    device.As(&m_pd3dDevice);
    context.As(&m_pd3dDeviceContext);

    DXGI_SWAP_CHAIN_DESC desc;
    ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
    desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
    desc.BufferCount = 2;
    desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SampleDesc.Count = 1;      //multisampling setting
    desc.SampleDesc.Quality = 0;    //vendor-specific flag
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.OutputWindow = hWnd;

    // Create the DXGI device object to use in other factories, such as Direct2D.
    ComPtr<IDXGIDevice3> dxgiDevice;
    m_pd3dDevice.As(&dxgiDevice);

    // Create swap chain.
    ComPtr<IDXGIAdapter> adapter;
    ComPtr<IDXGIFactory> factory;

    hr = dxgiDevice->GetAdapter(&adapter);

    if (SUCCEEDED(hr)) {
        adapter->GetParent(IID_PPV_ARGS(&factory));

        hr = factory->CreateSwapChain(
            m_pd3dDevice.Get(),
            &desc,
            &m_pDXGISwapChain
        );
    }

    hr = m_pDXGISwapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        (void**)&m_pBackBuffer);

    hr = m_pd3dDevice->CreateRenderTargetView(
        m_pBackBuffer.Get(),
        nullptr,
        m_pRenderTarget.GetAddressOf()
    );

    m_pBackBuffer->GetDesc(&m_bbDesc);

    CD3D11_TEXTURE2D_DESC depthStencilDesc(
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        static_cast<UINT> (m_bbDesc.Width),
        static_cast<UINT> (m_bbDesc.Height),
        1, // This depth stencil view has only one texture.
        1, // Use a single mipmap level.
        D3D11_BIND_DEPTH_STENCIL
    );

    m_pd3dDevice->CreateTexture2D(
        &depthStencilDesc,
        nullptr,
        &m_pDepthStencil
    );

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

    m_pd3dDevice->CreateDepthStencilView(
        m_pDepthStencil.Get(),
        &depthStencilViewDesc,
        &m_pDepthStencilView
    );

    ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
    m_viewport.Height = (float)m_bbDesc.Height;
    m_viewport.Width = (float)m_bbDesc.Width;
    m_viewport.MinDepth = 0;
    m_viewport.MaxDepth = 1;

    m_pd3dDeviceContext->RSSetViewports(
        1,
        &m_viewport
    );

    initEffect();
    initResource();
}

void render() {
    m_pd3dDeviceContext->OMSetRenderTargets(1, m_pRenderTarget.GetAddressOf(), m_pDepthStencilView.Get());
    m_pd3dDeviceContext->ClearRenderTargetView(m_pRenderTarget.Get(), clearColor);
    m_pd3dDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pd3dDeviceContext->Draw(3, 0);
    m_pDXGISwapChain->Present(0, 0);
}

void cleanup() {
    if (m_pd3dDeviceContext)
        m_pd3dDeviceContext->ClearState();
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
) {
    glfwSetErrorCallback(onError);
    if (!glfwInit()) {
        MessageBox(nullptr, L"Unable to initialze GLFW", L"Error", MB_OK);
        return 0;
    }
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "FreeSolitaire", nullptr, nullptr);
    if (window == nullptr) {
        MessageBox(nullptr, L"Failed to create the GLFW window", L"Error", MB_OK);
        return 0;
    }
    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (vidmode != nullptr) {
        glfwSetWindowPos(window, (vidmode->width - 800) >> 1, (vidmode->height - 600) >> 1);
    }

    initDX();

    glfwShowWindow(window);
    while (!glfwWindowShouldClose(window)) {
        render();
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
