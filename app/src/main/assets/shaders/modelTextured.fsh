// AssimpLoader 关联的着色器

precision mediump float; // required in GLSL ES 1.00
// 设置着色器的浮点精度为 mediump，这是 GLSL ES 1.00 版本所要求的

varying vec2 textureCoords;
// 插值后的纹理坐标，用于从纹理采样颜色

uniform sampler2D textureSampler;
// 纹理采样器，用于从纹理中获取颜色值

// 主函数，计算片元的颜色值
void main()
{
    // 从纹理采样器中获取纹理坐标为 textureCoords 处的颜色值，并将其作为片元的颜色值
    // 注意：这段代码没有指定 gl_FragColor 的 alpha 分量，因此默认为 1.0（不透明）
    gl_FragColor.xyz = texture2D(textureSampler, textureCoords).xyz;
}
