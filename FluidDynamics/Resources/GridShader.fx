cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
};

float4 VSMain( VS_INPUT input ) : SV_POSITION
{
    float4 output = float4(0, 0, 0, 0);
    output = input.Pos;
    output = mul(output, World);
    output = mul(output, View);
    output = mul(output, Projection);
    
    return output;
}