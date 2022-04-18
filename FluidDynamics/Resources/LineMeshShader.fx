cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer GridBuffer : register(b1)
{
    float3 gridDimensions;
}

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
	float3 instancePosition : TEXCOORD1;
    float3 instanceGridPos : TEXCOORD2;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
    float3 gridPos : LOCALPOSITION;
};

Texture3D txCFD;
SamplerState sam;

PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT) 0;

    input.Pos.x += input.instancePosition.x;
    input.Pos.y += input.instancePosition.y;
    input.Pos.z += input.instancePosition.z;

    output.gridPos = input.instanceGridPos;
    
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
  
    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{   
    float3 xyz = float3(0, 0, 0);
    
    input.gridPos.x /= gridDimensions.x;    
    input.gridPos.y /= gridDimensions.y;
    
    float yeet = txCFD.Sample(sam, input.gridPos.xyz);
    
    yeet *= gridDimensions.x * gridDimensions.y * gridDimensions.z;
    
    return float4(yeet, 0, 0, 1);
}