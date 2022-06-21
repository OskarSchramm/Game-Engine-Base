struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;

struct RenderTarget
{
	ID3D11RenderTargetView* myRenderTargetView;
	ID3D11ShaderResourceView* myShaderResource;
};