// AssimpLoader 关联的顶点着色器

// 顶点位置属性，用于接收顶点坐标信息
attribute vec3 vertexPosition;

// 顶点纹理坐标属性，用于接收顶点的纹理坐标信息
attribute vec2 vertexUV;

// 插值后的纹理坐标，用于传递给片元着色器进行纹理采样
varying vec2 textureCoords;

// 模型视图投影矩阵，用于将顶点坐标转换到裁剪空间
uniform mat4 mvpMat;

// 主函数，用于计算顶点的位置和纹理坐标
void main()
{
    // 将顶点坐标乘以模型视图投影矩阵，得到裁剪空间中的位置
    gl_Position = mvpMat * vec4(vertexPosition, 1.0);

    // 将顶点的纹理坐标直接赋值给插值后的纹理坐标变量
    textureCoords = vertexUV;
}
