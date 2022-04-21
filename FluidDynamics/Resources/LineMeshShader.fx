cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer GridBuffer : register(b1)
{
    float3 gridDimensions;
    float3 selectedItem;
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

Texture3D txDensity : register(t0);
Texture3D txVelocity : register(t1);
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
    if (selectedItem.x == input.gridPos.x && selectedItem.y == input.gridPos.y)
    {
        return float4(255, 255, 0, 255);
    }
    
    input.gridPos.x /= gridDimensions.x;
    input.gridPos.y /= gridDimensions.y;
    
    float yeet = txDensity.Sample(sam, input.gridPos.xyz);
    float3 yeet2 = txVelocity.Sample(sam, input.gridPos.xyz).xyz;
    
    //yeet *= gridDimensions.x * gridDimensions.y * gridDimensions.z;
    //yeet *= gridDimensions.x * gridDimensions.y * gridDimensions.z;
    
    float mag = sqrt(pow(yeet2.x, 2) + pow(yeet2.y, 2) + pow(yeet2.z, 2));
    
    mag *= 255;
            
    return float4(yeet, 0, 0, 255);
    


}